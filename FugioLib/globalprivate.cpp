#include "globalprivate.h"

#include <QDateTime>
#include <QTimer>
#include <QPluginLoader>
#include <QNetworkAccessManager>
#include <QApplication>
#include <QStandardPaths>

#include <QXmlStreamWriter>
#include <QXmlStreamReader>

#include <QDebug>

#include "nodeprivate.h"
#include "pinprivate.h"
#include "contextprivate.h"

#include <fugio/plugin_interface.h>
#include <fugio/device_factory_interface.h>
#include <fugio/global_signals.h>

GlobalPrivate *GlobalPrivate::mInstance = 0;

GlobalPrivate::GlobalPrivate( QObject * ) :
	GlobalSignals( this ), mEditTarget( nullptr ), mPause( false )
{
	mGlobalTimer.start();

	qDebug() << "Global Timer Monotonic:" << mGlobalTimer.isMonotonic();

	mLastTime   = 0;
	mFrameCount = 0;
	mMainWindow = 0;

	mCommandLineParser.addHelpOption();
	mCommandLineParser.addVersionOption();
}

GlobalPrivate::~GlobalPrivate( void )
{
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
	int		ResCnt = mPluginInitList.size();

	while( ResCnt > 0 )
	{
		ResCnt = 0;

		for( int i = 0 ; i < mPluginInitList.size() ; )
		{
			QObject							*PlgObj = mPluginInitList.at( i );
			fugio::PluginInterface			*PlgInt = qobject_cast<fugio::PluginInterface *>( PlgObj );

			qDebug() << "Initialising plugin" << PlgObj->metaObject()->className();

			qApp->processEvents();

			fugio::PluginInterface::InitResult		 PlgRes = PlgInt->initialise( this );

			if( PlgRes == fugio::PluginInterface::INIT_OK )
			{
				mPluginInstances.append( PlgObj );

				mPluginInitList.removeAt( i );

				ResCnt++;

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
	}

	qDeleteAll( mPluginInitList );

	mPluginInitList.clear();

	qDebug() << tr( "Plugins loaded: %1" ).arg( mPluginInstances.size() );

	qDebug() << tr( "Nodes registered: %1" ).arg( mNodeMap.size() );
}

void GlobalPrivate::loadPlugins( QDir &pDir )
{
#if defined( Q_OS_MACX )
	for( QString dirName : pDir.entryList( QDir::Dirs | QDir::NoDotAndDotDot ) )
	{
		QDir		PlgDir = pDir.absoluteFilePath( dirName );

		PlgDir.cd( "Contents" );
		PlgDir.cd( "MacOS" );

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
#else
	for( QString fileName : pDir.entryList( QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot ) )
	{
		QFileInfo	File( pDir.absoluteFilePath( fileName ) );

		if( File.isDir() )
		{
			QString		CurDir = QDir::currentPath();
			QString		NxtDir = pDir.absoluteFilePath( fileName );

			QDir::setCurrent( NxtDir );

			loadPlugins( QDir::current() );

			QDir::setCurrent( CurDir );

			continue;
		}

		if( !File.isReadable() )
		{
			continue;
		}

		if( File.suffix().toLower() != "dll" )
		{
			continue;
		}

		loadPlugin( pDir.absoluteFilePath( fileName ) );
	}
#endif
}

void GlobalPrivate::unloadPlugins()
{
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

		Q_ASSERT( !mNodeMap.contains( E.mUuid ) );

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
	Q_ASSERT( !mPinClassMap.contains( pUUID ) );

	mPinClassMap.insert( pUUID, pMetaObject );
	mPinNameMap.insert( pUUID, pName );

	return( true );
}

void GlobalPrivate::unregisterPinClass( const QUuid &pUUID )
{
	mPinClassMap.remove( pUUID );
	mPinNameMap.remove( pUUID );
}

QSharedPointer<fugio::NodeInterface> GlobalPrivate::createNode( fugio::ContextInterface *pContext, const QString &pName, const QUuid &pUUID, const QUuid &pOrigId, const QVariantHash &pSettings )
{
	if( !mNodeMap.contains( pUUID ) )
	{
		qWarning() << "Unknown NodeControlInterface" << pUUID;
	}

	NodePrivate	*NODE = new NodePrivate();

	if( !NODE )
	{
		return( QSharedPointer<fugio::NodeInterface>() );
	}

	NODE->setContext( pContext );
	NODE->setName( pName );
	NODE->setControlUuid( pUUID );
	NODE->setSettings( pSettings );

	QSharedPointer<fugio::NodeInterface>		NODE_PTR = QSharedPointer<fugio::NodeInterface>( NODE );

	pContext->registerNode( NODE_PTR, pOrigId );

	if( mNodeMap.contains( pUUID ) )
	{
		QObject		*ClassInstance = mNodeMap.value( pUUID ).mMetaObject->newInstance( Q_ARG( QSharedPointer<fugio::NodeInterface>, NODE_PTR ) );

		if( ClassInstance )
		{
			fugio::NodeControlInterface	*NodeControl = qobject_cast<fugio::NodeControlInterface *>( ClassInstance );

			if( NodeControl )
			{
				NODE->setControl( QSharedPointer<fugio::NodeControlInterface>( NodeControl ) );
			}
		}
	}

	return( NODE_PTR );
}

QSharedPointer<fugio::PinInterface> GlobalPrivate::createPin( const QString &pName, const QUuid &pLocalId, QSharedPointer<fugio::NodeInterface> pNode, PinDirection pDirection, const QUuid &pControlUUID, const QVariantHash &pSettings )
{
	Q_ASSERT( !pLocalId.isNull() );

	PinPrivate						*P = new PinPrivate();

	if( !P )
	{
		return( QSharedPointer<fugio::PinInterface>() );
	}

	QSharedPointer<fugio::PinInterface>	PinPtr = QSharedPointer<fugio::PinInterface>( P );

	P->setContext( pNode->context() );
	P->setLocalId( pLocalId );
	P->setName( pName );
	P->setNode( pNode );
	P->setDirection( pDirection );
	P->setSettings( pSettings );
	P->setControlUuid( pControlUUID );

	if( !pControlUUID.isNull() )
	{
		QSharedPointer<fugio::PinControlInterface>		PinControl = createPinControl( pControlUUID, PinPtr );

		if( PinControl )
		{
			P->setControl( PinControl );
		}
	}

	return( PinPtr );
}

QSharedPointer<fugio::PinControlInterface> GlobalPrivate::createPinControl( const QUuid &pUUID, QSharedPointer<fugio::PinInterface> pPin )
{
	if( !mPinClassMap.contains( pUUID ) )
	{
		qWarning() << "Unknown fugio::PinControlInterface" << pUUID << pPin->name();

		return( QSharedPointer<fugio::PinControlInterface>() );
	}

	if( mPinClassMap.contains( pUUID ) )
	{
		const QMetaObject *SMO = mPinClassMap.value( pUUID, nullptr );

		if( SMO )
		{
			QObject		*ClassInstance = SMO->newInstance( Q_ARG( QSharedPointer<fugio::PinInterface>, pPin ) );

			if( ClassInstance )
			{
				fugio::PinControlInterface		*PinControl = qobject_cast<fugio::PinControlInterface *>( ClassInstance );

				if( PinControl )
				{
					return( QSharedPointer<fugio::PinControlInterface>( PinControl ) );
				}

				delete ClassInstance;
			}
			else
			{
				qWarning() << "Can't create Pin ClassInstance of" << mPinClassMap.value( pUUID )->className();
			}
		}
	}

	return( QSharedPointer<fugio::PinControlInterface>() );
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

void GlobalPrivate::timeout( void )
{
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

	QTimer::singleShot( 1, this, SLOT(timeout()) );
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

bool GlobalPrivate::updatePinControl( QSharedPointer<fugio::PinInterface> pPin, const QUuid &pPinControlUuid )
{
	QSharedPointer<fugio::PinControlInterface>		PinControl = createPinControl( pPinControlUuid, pPin );

	if( PinControl )
	{
		pPin->setControl( PinControl );

		return( true );
	}

	return( false );
}

void GlobalPrivate::clear()
{
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
	ContextPrivate		*CP = new ContextPrivate( this );

	if( CP == 0 )
	{
		return( QSharedPointer<fugio::ContextInterface>() );
	}

	QSharedPointer<fugio::ContextInterface>	C = QSharedPointer<fugio::ContextInterface>( CP );

	mContexts.append( C );

	emit contextAdded( C );

	return( C );
}

void GlobalPrivate::delContext( QSharedPointer<fugio::ContextInterface> pContext )
{
	mContexts.removeAll( pContext );

	emit contextRemoved( pContext );
}

QList<QSharedPointer<fugio::ContextInterface> > GlobalPrivate::contexts()
{
	return( mContexts );
}

void GlobalPrivate::setMainWindow( QMainWindow *pMainWindow )
{
	mMainWindow = pMainWindow;
}

QMainWindow *GlobalPrivate::mainWindow( void )
{
	return( mMainWindow );
}

void GlobalPrivate::setEditTarget( fugio::EditInterface *pEditTarget )
{
	if( mEditTarget != pEditTarget )
	{
		mEditTarget = pEditTarget;

		emit editTargetChanged( mEditTarget );
	}
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
