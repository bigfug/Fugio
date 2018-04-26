#include "globalprivate.h"

#include <QDateTime>
#include <QTimer>
#include <QPluginLoader>
#include <QNetworkAccessManager>
#include <QApplication>
#include <QStandardPaths>
#include <QMainWindow>
#include <QTranslator>
#include <QFileInfo>

#include <QXmlStreamWriter>
#include <QXmlStreamReader>

#include <QDebug>

#include "nodeprivate.h"
#include "pinprivate.h"
#include "contextprivate.h"

#include <fugio/plugin_interface.h>
#include <fugio/device_factory_interface.h>
#include <fugio/global_signals.h>

#if defined( Q_OS_WIN )
#include <Windows.h>
#endif

#include <QLibraryInfo>

GlobalPrivate *GlobalPrivate::mInstance = 0;

GlobalPrivate::GlobalPrivate( QObject * ) :
	GlobalSignals( this ), mPause( false )
#if defined( GLOBAL_THREADED )
  , mGlobalThread( nullptr )
#endif
{
	//-------------------------------------------------------------------------
	// Install translator

	static QTranslator		Translator;

	if( Translator.load( QLocale(), QLatin1String( "translations" ), QLatin1String( "_" ), ":/" ) )
	{
		qApp->installTranslator( &Translator );
	}

	//-------------------------------------------------------------------------

	mTimeSync = std::unique_ptr<fugio::TimeSync>( new fugio::TimeSync( this ) );

	mLastTime   = 0;
	mFrameCount = 0;

	connect( this, SIGNAL(frameEnd()), &mUniverse, SLOT(cast()) );

#if !defined( GLOBAL_THREADED )
	connect( &mGlobalTimer, &QTimer::timeout, this, &GlobalPrivate::executeFrame );
#endif
}

GlobalPrivate::~GlobalPrivate( void )
{
	stop();

	mTimeSync.reset();

	for( UuidClassEntryMap::const_iterator it = mNodeMap.constBegin() ; it != mNodeMap.constEnd() ; it++ )
	{
		qWarning() << "Node Class not removed:" << it.value().mMetaObject->className();
	}

	for( UuidClassMap::const_iterator it = mPinClassMap.constBegin() ; it != mPinClassMap.constEnd() ; it++ )
	{
		qWarning() << "Pin Class not removed:" << it.value()->className();
	}
}

void GlobalPrivate::registerPlugin( QObject *pPluginInstance )
{
	mPluginInitList.append( pPluginInstance );
}

void GlobalPrivate::initialisePlugins()
{
	bool	LastChance = false;
	int		ResCnt = mPluginInitList.size();

	while( ResCnt > 0 )
	{
		ResCnt = 0;

		for( int i = 0 ; i < mPluginInitList.size() ; )
		{
			QObject							*PlgObj = mPluginInitList.at( i );
			fugio::PluginInterface			*PlgInt = qobject_cast<fugio::PluginInterface *>( PlgObj );

			if( !LastChance )
			{
				qDebug() << "Initialising plugin" << PlgObj->metaObject()->className();
			}
			else
			{
				qDebug() << "Initialising plugin" << PlgObj->metaObject()->className() << "(Last Chance)";
			}

			qApp->processEvents();

			fugio::PluginInterface::InitResult		 PlgRes = PlgInt->initialise( this, LastChance );

			if( PlgRes == fugio::PluginInterface::INIT_OK )
			{
				mPluginInstances.append( PlgObj );

				mPluginInitList.removeAt( i );

				ResCnt++;

				LastChance = false;

				continue;
			}

			if( PlgRes == fugio::PluginInterface::INIT_FAILED )
			{
				qWarning() << "Can't initialise plugin" << PlgObj->metaObject()->className();

				PlgObj->deleteLater();

				mPluginInitList.removeAt( i );

				ResCnt++;

				continue;
			}

			i++;
		}

		if( !ResCnt && !LastChance )
		{
			ResCnt++;

			LastChance = true;
		}
	}

	qDeleteAll( mPluginInitList );

	mPluginInitList.clear();

	qDebug() << tr( "Plugins loaded: %1" ).arg( mPluginInstances.size() );

	qDebug() << tr( "Nodes registered: %1" ).arg( mNodeMap.size() );
}

QString GlobalPrivate::sharedDataPath() const
{
#if defined( QT_DEBUG )
	// In debug mode go relative to this source file

	QDir		TmpDir = QDir( __FILE__ );

	TmpDir.cdUp();
	TmpDir.cdUp();

	return( TmpDir.absoluteFilePath( "share" ) );
#endif

#if defined( Q_OS_WIN ) && !defined( QT_DEBUG )
	return( QDir( QApplication::applicationDirPath() ).absoluteFilePath( "share" ) );
#endif

#if defined( Q_OS_MAC ) && !defined( QT_DEBUG )
	QDir		TmpDir = QDir( QApplication::applicationDirPath() );

	// Get out of the app bundle

	TmpDir.cdUp();
	TmpDir.cdUp();
	TmpDir.cdUp();

	return( TmpDir.absoluteFilePath( "share" ) );
#endif

#if defined( Q_OS_LINUX ) && !defined( QT_DEBUG )
	QDir		TmpDir = QDir( QApplication::applicationDirPath() );

	TmpDir.cdUp();

	TmpDir.cd( "share" );

	return( TmpDir.absoluteFilePath( "fugio" ) );
#endif
}

void GlobalPrivate::loadPlugins( QDir pDir )
{
#if defined( Q_OS_MACX )
	for( QString dirName : pDir.entryList( QDir::Dirs | QDir::NoDotAndDotDot ) )
	{
		QDir		PlgDir = pDir.absoluteFilePath( dirName );

		if( PlgDir.cd( "Contents/MacOS" ) )
		{
			for( QString fileName : PlgDir.entryList( QDir::Files ) )
			{
				QFileInfo	File( PlgDir.absoluteFilePath( fileName ) );

				if( !File.isReadable() )
				{
					continue;
				}

				loadPlugin( pDir.absoluteFilePath( File.absoluteFilePath() ) );
			}
		}
		else
		{
			loadPlugins( PlgDir );
		}
	}
#else
	for( QString fileName : pDir.entryList( QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot ) )
	{
		QFileInfo	File( pDir.absoluteFilePath( fileName ) );

		if( File.isDir() )
		{
			QString		NxtDir = pDir.absoluteFilePath( fileName );

#if defined( Q_OS_WIN )
			SetDllDirectory( (LPCTSTR)NxtDir.utf16() );

			loadPlugins( QDir( NxtDir ) );

			SetDllDirectory( NULL );
#else
			QString		CurDir = QDir::currentPath();

			QDir::setCurrent( NxtDir );

			loadPlugins( QDir::current() );

			QDir::setCurrent( CurDir );
#endif

			continue;
		}

		if( !File.isReadable() )
		{
			continue;
		}

#if defined( Q_OS_LINUX )
		if( File.suffix().toLower() != "so" )
		{
			continue;
		}
#elif defined( Q_OS_WIN )
		if( File.suffix().toLower() != "dll" )
		{
			continue;
		}
#endif

		loadPlugin( pDir.absoluteFilePath( fileName ) );
	}
#endif
}

void GlobalPrivate::unloadPlugins()
{
	std::reverse( mPluginInstances.begin(), mPluginInstances.end() );

	for( QList<QObject *>::iterator it = mPluginInstances.begin() ; it != mPluginInstances.end() ; it++ )
	{
		fugio::PluginInterface	*Plugin = qobject_cast<fugio::PluginInterface *>( *it );

		if( Plugin )
		{
			Plugin->deinitialise();
		}
	}

	qDeleteAll( mPluginInstances );

	mPluginInstances.clear();

	qDeleteAll( mPluginInitList );

	mPluginInitList.clear();
}

bool GlobalPrivate::loadPlugin( const QString &pFileName )
{
	QFileInfo		FileInfo( pFileName );
	QString			BaseName = FileInfo.baseName();

	if( !mEnabledPlugins.isEmpty() )
	{
		bool		PluginFound = false;

		for( QString S : mEnabledPlugins )
		{
			if( BaseName.contains( S ) )
			{
				PluginFound = true;

				break;
			}
		}

		if( !PluginFound )
		{
			return( false );
		}
	}

	for( QString S : mDisabledPlugins )
	{
		if( BaseName.contains( S ) )
		{
			qInfo() << "Skipping" << pFileName;

			return( false );
		}
	}

	QPluginLoader	Loader( pFileName );

	if( !Loader.load() )
	{
		qWarning() << pFileName << ":" << Loader.errorString();

		return( false );
	}

	QObject			*PluginInstance = Loader.instance();

	if( !PluginInstance )
	{
		qWarning() << pFileName << ":" << Loader.errorString();

		return( false );
	}

	fugio::PluginInterface	*Plugin = qobject_cast<fugio::PluginInterface *>( PluginInstance );

	if( !Plugin )
	{
		qWarning() << pFileName << "No PluginInterface";

		delete PluginInstance;

		return( false );
	}

	qDebug() << "Loading plugin:" << pFileName;

	registerPlugin( PluginInstance );

	mLoadedPlugins << FileInfo.baseName();

	return( true );
}

void GlobalPrivate::registerInterface(const QUuid &pUuid, QObject *pInterface)
{
	mInterfaceMap.remove( pUuid );

	mInterfaceMap.insert( pUuid, pInterface );
}

void GlobalPrivate::unregisterInterface(const QUuid &pUuid)
{
	mInterfaceMap.remove( pUuid );
}

QObject *GlobalPrivate::findInterface(const QUuid &pUuid)
{
	return( mInterfaceMap.value( pUuid, nullptr ) );
}

void GlobalPrivate::registerNodeClasses( const fugio::ClassEntryList &pNodes )
{
	for( const fugio::ClassEntry &E : pNodes )
	{
		registerNodeClass( E );
	}
}

void GlobalPrivate::unregisterNodeClasses( const fugio::ClassEntryList &pNodes )
{
	foreach( const fugio::ClassEntry &E, pNodes )
	{
		unregisterNodeClass( E.mUuid );
	}
}

void GlobalPrivate::registerNodeClasses( const fugio::ClassEntry pNodes[] )
{
	for( int i = 0 ; pNodes[ i ].mMetaObject != 0 ; i++ )
	{
		const fugio::ClassEntry &E = pNodes[ i ];

		registerNodeClass( E );
	}
}

void GlobalPrivate::unregisterNodeClasses( const fugio::ClassEntry pNodes[] )
{
	for( int i = 0 ; pNodes[ i ].mMetaObject != 0 ; i++ )
	{
		const fugio::ClassEntry &E = pNodes[ i ];

		unregisterNodeClass( E.mUuid );
	}
}

void GlobalPrivate::registerPinClasses( const fugio::ClassEntryList &pNodes )
{
	foreach( const fugio::ClassEntry &E, pNodes )
	{
		registerPinClass( E.mName, E.mUuid, E.mMetaObject );
	}
}

void GlobalPrivate::unregisterPinClasses( const fugio::ClassEntryList &pNodes )
{
	foreach( const fugio::ClassEntry &E, pNodes )
	{
		unregisterPinClass( E.mUuid );
	}
}

void GlobalPrivate::registerPinClasses( const fugio::ClassEntry pNodes[] )
{
	for( int i = 0 ; pNodes[ i ].mMetaObject ; i++ )
	{
		const fugio::ClassEntry &E = pNodes[ i ];

		registerPinClass( E.mName, E.mUuid, E.mMetaObject );
	}
}

void GlobalPrivate::unregisterPinClasses( const fugio::ClassEntry pNodes[] )
{
	for( int i = 0 ; pNodes[ i ].mMetaObject ; i++ )
	{
		const fugio::ClassEntry &E = pNodes[ i ];

		unregisterPinClass( E.mUuid );
	}
}

bool GlobalPrivate::registerNodeClass( const fugio::ClassEntry &E )
{
	if( mNodeMap.contains( E.mUuid ) )
	{
		for( UuidClassEntryMap::const_iterator it = mNodeMap.begin() ; it != mNodeMap.end() ; it++ )
		{
			if( it.key() == E.mUuid )
			{
				qWarning() << E.mName << "has same UUID as" << it.value().mMetaObject->className();
			}
		}

		return( false );
	}

	mNodeMap.insert( E.mUuid, E );

	if( !E.mFlags.testFlag( fugio::ClassEntry::Deprecated ) )
	{
		emit nodeClassAdded( E );
	}

	return( true );
}

void GlobalPrivate::unregisterNodeClass( const QUuid &pUUID )
{
	mNodeMap.remove( pUUID );
}

bool GlobalPrivate::registerPinClass( const QString &pName, const QUuid &pUUID, const QMetaObject *pMetaObject )
{
	if( mPinClassMap.contains( pUUID ) )
	{
		qWarning() << "Pin Class" << pName << pUUID << pMetaObject->className() << "is already registered";

		return( false );
	}

	mPinClassMap.insert( pUUID, pMetaObject );
	mPinNameMap.insert( pUUID, pName );

	return( true );
}

void GlobalPrivate::unregisterPinClass( const QUuid &pUUID )
{
	mPinClassMap.remove( pUUID );
	mPinNameMap.remove( pUUID );
}


const QMetaObject *GlobalPrivate::findNodeMetaObject( const QString &pClassName ) const
{
	QUuid		NodeUuid = findNodeByClass( pClassName );

	if( NodeUuid.isNull() )
	{
		return( 0 );
	}

	return( mNodeMap.value( NodeUuid ).mMetaObject );
}

fugio::ClassEntry GlobalPrivate::findNodeClassEntry( const QUuid &pNodeUuid ) const
{
	return( mNodeMap.value( pNodeUuid ) );
}

const QMetaObject *GlobalPrivate::findNodeMetaObject( const QUuid &pNodeUuid ) const
{
	return( mNodeMap.value( pNodeUuid ).mMetaObject );
}

const QMetaObject *GlobalPrivate::findPinMetaObject( const QUuid &pPinUuid ) const
{
	return( mPinClassMap.value( pPinUuid ) );
}

QString GlobalPrivate::nodeName(const QUuid &pUuid) const
{
	return( mNodeMap.value( pUuid ).mName );
}

QString GlobalPrivate::pinName(const QUuid &pUuid) const
{
	return( mPinNameMap.value( pUuid ) );
}

//-----------------------------------------------------------------------------
// MAIN TIMER ENTRY POINT

void GlobalPrivate::executeFrame( void )
{
	QMutexLocker	Lock( &mContextMutex );

	const qint64		TimeStamp = timestamp();

	if( !mPause )
	{
		emit frameInitialise();
		emit frameInitialise( TimeStamp );

		emit frameStart();
		emit frameStart( TimeStamp );

		for( QSharedPointer<fugio::ContextInterface> CP : mContexts )
		{
			QSharedPointer<ContextPrivate>	C = qSharedPointerCast<ContextPrivate>( CP );

			if( C && C->active() )
			{
				C->doFrameInitialise( TimeStamp );
			}
		}

		for( QSharedPointer<fugio::ContextInterface> CP : mContexts )
		{
			QSharedPointer<ContextPrivate>	C = qSharedPointerCast<ContextPrivate>( CP );

			if( C && C->active() )
			{
				C->doFrameStart( TimeStamp );
			}
		}

		for( QSharedPointer<fugio::ContextInterface> CP : mContexts )
		{
			QSharedPointer<ContextPrivate>	C = qSharedPointerCast<ContextPrivate>( CP );

			if( C && C->active() )
			{
				C->doFrameProcess( TimeStamp );
			}
		}

		for( QSharedPointer<fugio::ContextInterface> CP : mContexts )
		{
			QSharedPointer<ContextPrivate>	C = qSharedPointerCast<ContextPrivate>( CP );

			if( C && C->active() )
			{
				C->doFrameEnd( TimeStamp );
			}
		}

		emit frameEnd();
		emit frameEnd( TimeStamp );

		mUniverse.clearData( universalTimestamp() );

		mFrameCount++;
	}

	if( TimeStamp - mLastTime >= 1000 )
	{
		emit fps( qreal( mFrameCount ) );

		while( mLastTime + 1000 <= TimeStamp )
		{
			mLastTime += 1000;
		}

		mFrameCount = 0;
	}
}

QUuid GlobalPrivate::findNodeByClass( const QString &pClassName ) const
{
	for( UuidClassEntryMap::const_iterator it = mNodeMap.constBegin() ; it != mNodeMap.constEnd() ; it++ )
	{
		if( it.value().mName == pClassName )
		{
			return( it.key() );
		}
	}

	return( QUuid() );
}

QUuid GlobalPrivate::findPinByClass(const QString &pClassName) const
{
	return( mPinNameMap.key( pClassName ) );
}

QStringList GlobalPrivate::pinNames() const
{
	return( mPinNameMap.values() );
}

void GlobalPrivate::clear()
{
	QMutexLocker	Lock( &mContextMutex );

	for( QList< QSharedPointer<fugio::ContextInterface> >::iterator it = mContexts.begin() ; it != mContexts.end() ; it++ )
	{
		QSharedPointer<fugio::ContextInterface>	CP = *it;

		if( CP == 0 )
		{
			continue;
		}

		QSharedPointer<ContextPrivate>	C = qSharedPointerCast<ContextPrivate>( CP );

		if( C != 0 )
		{
			C->clear();
		}
	}
}

QSharedPointer<fugio::ContextInterface> GlobalPrivate::newContext( void )
{
	QMutexLocker	Lock( &mContextMutex );

	ContextPrivate		*CP = new ContextPrivate( this );

	if( !CP )
	{
		return( QSharedPointer<fugio::ContextInterface>() );
	}

	CP->moveToThread( thread() );

	QSharedPointer<fugio::ContextInterface>	C = QSharedPointer<fugio::ContextInterface>( CP );

	mContexts.append( C );

	Lock.unlock();

	emit contextAdded( C );

	return( C );
}

void GlobalPrivate::delContext( QSharedPointer<fugio::ContextInterface> pContext )
{
	QMutexLocker	Lock( &mContextMutex );

	mContexts.removeAll( pContext );

	emit contextRemoved( pContext );
}

QList<QSharedPointer<fugio::ContextInterface> > GlobalPrivate::contexts()
{
	return( mContexts );
}

void GlobalPrivate::registerDeviceFactory( fugio::DeviceFactoryInterface *pFactory )
{
	mDeviceFactories.append( pFactory );
}

void GlobalPrivate::unregisterDeviceFactory( fugio::DeviceFactoryInterface *pFactory )
{
	mDeviceFactories.removeAll( pFactory );
}

QStringList GlobalPrivate::deviceFactoryMenuTextList() const
{
	QStringList		MenuText;

	for( fugio::DeviceFactoryInterface *DevFac : mDeviceFactories )
	{
		MenuText.append( DevFac->deviceConfigMenuText() );
	}

	return( MenuText );
}

void GlobalPrivate::deviceFactoryGui( QWidget *pParent, const QString &pMenuText )
{
	foreach( fugio::DeviceFactoryInterface *DevFac, mDeviceFactories )
	{
		if( DevFac->deviceConfigMenuText() == pMenuText )
		{
			DevFac->deviceConfigGui( pParent );

			return;
		}
	}
}

void GlobalPrivate::loadConfig( QSettings &pSettings )
{
	emit configLoad( pSettings );
}

void GlobalPrivate::saveConfig( QSettings &pSettings ) const
{
	emit configSave( pSettings );
}

QUuid GlobalPrivate::instanceId() const
{
	QSettings		Settings( FUGIO_GLOBAL_SETTINGS );

	QUuid			InstanceId = QUuid( Settings.value( "instance" ).toString() );

	if( InstanceId.isNull() )
	{
		InstanceId = QUuid::createUuid();

		Settings.setValue( "instance", InstanceId.toString() );
	}

	return( InstanceId );
}

GlobalPrivate *GlobalPrivate::instance()
{
	if( !mInstance )
	{
		mInstance = new GlobalPrivate();

		Q_ASSERT( mInstance );
	}

	return( mInstance );
}

void GlobalPrivate::registerPinSplitter(const QUuid &pPinId, const QUuid &pNodeId)
{
	mPinSplitters.insert( pPinId, pNodeId );
}

void GlobalPrivate::registerPinJoiner(const QUuid &pPinId, const QUuid &pNodeId)
{
	mPinJoiners.insert( pPinId, pNodeId );
}

QList<QUuid> GlobalPrivate::pinSplitters(const QUuid &pPinId) const
{
	return( mPinSplitters.values( pPinId ) );
}

QList<QUuid> GlobalPrivate::pinJoiners(const QUuid &pPinId) const
{
	return( mPinJoiners.values( pPinId ) );
}

QUuid GlobalPrivate::findPinForMetaType(QMetaType::Type pType) const
{
	return( mMetaTypeToPinUuid.value( pType ) );
}

void GlobalPrivate::registerPinForMetaType(const QUuid &pUuid, QMetaType::Type pType)
{
	mMetaTypeToPinUuid.insert( pType, pUuid );
}

void GlobalPrivate::start()
{
#if defined( GLOBAL_THREADED )
	if( !mGlobalThread )
	{
		mGlobalThread = new QThread( this );

		mGlobalTimer.moveToThread( mGlobalThread );

		moveToThread( mGlobalThread );

		GlobalThread	*GlobalWorker = new GlobalThread( this );

		GlobalWorker->moveToThread( mGlobalThread );

		connect( this, SIGNAL(signalFrameExecute()), GlobalWorker, SLOT(update()) );

		connect( &mGlobalTimer, &QTimer::timeout, GlobalWorker, &GlobalThread::update );

		connect( mGlobalThread, &QThread::finished, GlobalWorker, &GlobalThread::deleteLater );

		mGlobalThread->start();
	}
#endif

	mGlobalTimer.start( 10 );
}

void GlobalPrivate::stop()
{
	mGlobalTimer.stop();

#if defined( GLOBAL_THREADED )
	if( mGlobalThread )
	{
		mGlobalTimer.moveToThread( QCoreApplication::instance()->thread() );

		moveToThread( QCoreApplication::instance()->thread() );

		mGlobalThread->quit();
		mGlobalThread->wait();

		delete mGlobalThread;

		mGlobalThread = nullptr;
	}
#endif
}
