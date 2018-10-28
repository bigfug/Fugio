#include "contextprivate.h"

#include <fugio/global_interface.h>
#include <fugio/node_interface.h>
#include <fugio/node_control_interface.h>
#include <fugio/pin_interface.h>
#include "fugio/playhead_interface.h"

#include <QSettings>
#include <QStringList>
#include <QDebug>
#include <QTimer>
#include <QtConcurrent/QtConcurrentRun>
#include <QFile>
#include <QDir>
#include <QTemporaryDir>
#include <QDateTime>
#include <QCoreApplication>
#include <QThread>

#include "nodeprivate.h"
#include "pinprivate.h"

#include <fugio/utils.h>

//using namespace fugio;

//#define LOG_NODE_UPDATES
//#define DEBUG_CONNECTIONS

QMap<fugio::ContextInterface::MetaInfo,QString>	 ContextWorker::mMetaNameMap;

ContextPrivate::ContextPrivate( fugio::GlobalInterface *pApp, QObject *pParent )
	: QThread( pParent ), mApp( pApp ), mContextWorker( Q_NULLPTR )
{
#if defined( CONTEXT_THREADED )
	mContextWorker = new ContextWorker( this );

	mContextWorker->moveToThread( this );

	connect( this, &QThread::finished, mContextWorker, &ContextWorker::deleteLater );

	start();
#endif
}

ContextPrivate::~ContextPrivate( void )
{
#if defined( CONTEXT_THREADED )
	quit();
	wait();
#endif
}

bool ContextPrivate::isContextThread( void ) const
{
	return( QThread::currentThread() == this );
}

bool ContextPrivate::unload( QString pFileName )
{
	return( false );
}

bool ContextPrivate::loadData( QString pFileName )
{
	return( mContextWorker->loadData( pFileName ) );
}

bool ContextPrivate::saveData( QString pFileName ) const
{
	return( mContextWorker->saveData( pFileName ) );
}

void ContextPrivate::setActive( bool pActive )
{
	mContextWorker->setActive( pActive );
}

bool ContextPrivate::active( void ) const
{
	return( mContextWorker->active() );
}

void ContextPrivate::futureSync( const QFuture<void> &pFuture )
{
	mContextWorker->futureSync( pFuture );
}

QString ContextPrivate::metaInfo( MetaInfo pType ) const
{
	return( mContextWorker->metaInfo( pType ) );
}

void ContextPrivate::setMetaInfo( MetaInfo pType, const QString & pMetaData )
{
	mContextWorker->setMetaInfo( pType, pMetaData );
}

void ContextPrivate::registerInterface( const QUuid & pUuid, QObject * pInterface )
{
	mContextWorker->registerInterface( pUuid, pInterface );
}

void ContextPrivate::unregisterInterface( const QUuid & pUuid )
{
	mContextWorker->unregisterInterface( pUuid );
}

QObject *ContextPrivate::findInterface( const QUuid & pUuid )
{
	return( mContextWorker->findInterface( pUuid ) );
}

QSharedPointer<fugio::NodeInterface> ContextPrivate::createNode( const QString & pName, const QUuid & pGlobalId, const QUuid & pControlId, const QVariantHash & pSettings )
{
	QSharedPointer<fugio::NodeInterface>	RetVal;

	QMetaObject::invokeMethod( mContextWorker, "createNode", Qt::BlockingQueuedConnection,
		Q_RETURN_ARG( QSharedPointer<fugio::NodeInterface>, RetVal ),
		Q_ARG( QString, pName ),
		Q_ARG( QUuid, pGlobalId ),
		Q_ARG( QUuid, pControlId ),
		Q_ARG( QVariantHash, pSettings )
		);

	return( RetVal );
}

QSharedPointer<fugio::PinInterface> ContextPrivate::createPin( const QString & pName, const QUuid & pGlobalId, const QUuid & pLocalId, PinDirection pDirection, const QUuid & pControlId, const QVariantHash & pSettings )
{
	QSharedPointer<fugio::PinInterface>	RetVal;

	QMetaObject::invokeMethod( mContextWorker, "createPin", Qt::BlockingQueuedConnection,
		Q_RETURN_ARG( QSharedPointer<fugio::PinInterface>, RetVal ),
		Q_ARG( QString, pName ),
		Q_ARG( QUuid, pGlobalId ),
		Q_ARG( QUuid, pLocalId ),
		Q_ARG( PinDirection, pDirection ),
		Q_ARG( QUuid, pControlId ),
		Q_ARG( QVariantHash, pSettings )
	);

	return( RetVal );
}

QSharedPointer<fugio::PinControlInterface> ContextPrivate::createPinControl( const QUuid & pUUID, QSharedPointer<fugio::PinInterface> pPin )
{
	QSharedPointer<fugio::PinControlInterface>	RetVal;

	QMetaObject::invokeMethod( mContextWorker, "createPinControl", Qt::BlockingQueuedConnection,
		Q_RETURN_ARG( QSharedPointer<fugio::PinControlInterface>, RetVal ),
		Q_ARG( QUuid, pUUID ),
		Q_ARG( QSharedPointer<fugio::PinInterface>, pPin )
	);

	return( RetVal );
}

bool ContextPrivate::updatePinControl( QSharedPointer<fugio::PinInterface> pPin, const QUuid & pPinControlUuid )
{
	bool	RetVal;

	QMetaObject::invokeMethod( mContextWorker, "updatePinControl", Qt::BlockingQueuedConnection,
		Q_RETURN_ARG( bool, RetVal ),
		Q_ARG( QSharedPointer<fugio::PinInterface>, pPin ),
		Q_ARG( QUuid, pPinControlUuid )
	);

	return( RetVal );
}

void ContextPrivate::registerNode( QSharedPointer<fugio::NodeInterface> pNode )
{
	QMetaObject::invokeMethod( mContextWorker, "registerNode", Qt::BlockingQueuedConnection,
		Q_ARG( QSharedPointer<fugio::NodeInterface>, pNode )
	);
}

void ContextPrivate::unregisterNode( const QUuid & pUUID )
{
	QMetaObject::invokeMethod( mContextWorker, "unregisterNode", Qt::BlockingQueuedConnection,
		Q_ARG( QUuid, pUUID )
	);
}

void ContextPrivate::renameNode( const QUuid & pUUID1, const QUuid & pUUID2 )
{
	QMetaObject::invokeMethod( mContextWorker, "renameNode", Qt::BlockingQueuedConnection,
		Q_ARG( QUuid, pUUID1 ),
		Q_ARG( QUuid, pUUID2 )
	);
}

QSharedPointer<fugio::NodeInterface> ContextPrivate::findNode( const QUuid & pUUID ) const
{
	return( mContextWorker->findNode( pUUID ) );
}

QSharedPointer<fugio::NodeInterface> ContextPrivate::findNode( const QString & pName ) const
{
	return( mContextWorker->findNode( pName ) );
}

QList<QUuid> ContextPrivate::nodeList( void ) const
{
	return( mContextWorker->nodeList() );
}

void ContextPrivate::nodeInitialised( void )
{
	mContextWorker->nodeInitialised();
}

void ContextPrivate::updateNode( QSharedPointer<fugio::NodeInterface> pNode )
{
	QMetaObject::invokeMethod( mContextWorker, "updateNode", Qt::BlockingQueuedConnection,
		Q_ARG( QSharedPointer<fugio::NodeInterface>, pNode )
	);
}

void ContextPrivate::registerPin( QSharedPointer<fugio::PinInterface> pPin )
{
	QMetaObject::invokeMethod( mContextWorker, "registerPin", Qt::BlockingQueuedConnection,
		Q_ARG( QSharedPointer<fugio::PinInterface>, pPin )
	);
}

void ContextPrivate::unregisterPin( const QUuid & pUUID )
{
	QMetaObject::invokeMethod( mContextWorker, "unregisterPin", Qt::BlockingQueuedConnection,
		Q_ARG( QUuid, pUUID )
	);
}

void ContextPrivate::renamePin( const QUuid & pUUID1, const QUuid & pUUID2 )
{
	QMetaObject::invokeMethod( mContextWorker, "renamePin", Qt::BlockingQueuedConnection,
		Q_ARG( QUuid, pUUID1 ),
		Q_ARG( QUuid, pUUID2 )
	);
}

QSharedPointer<fugio::PinInterface> ContextPrivate::findPin( const QUuid & pUUID )
{
	return( mContextWorker->findPin( pUUID ) );
}

void ContextPrivate::pinUpdated( QSharedPointer<fugio::PinInterface> pPin, qint64 pGlobalTimestamp, bool pUpdatedConnectedNode )
{
	QMetaObject::invokeMethod( mContextWorker, "pinUpdated", Qt::BlockingQueuedConnection,
		Q_ARG( QSharedPointer<fugio::PinInterface>, pPin ),
		Q_ARG( qint64, pGlobalTimestamp ),
		Q_ARG( bool, pUpdatedConnectedNode )
	);
}

QList<QSharedPointer<fugio::PinInterface>> ContextPrivate::connections( const QUuid & pUUID )
{
	return( mContextWorker->connections( pUUID ) );
}

bool ContextPrivate::isConnected( const QUuid & pUUID )
{
	return( mContextWorker->isConnected( pUUID ) );
}

bool ContextPrivate::isConnectedTo( const QUuid & pUUID1, const QUuid & pUUID2 )
{
	return( mContextWorker->isConnectedTo( pUUID1, pUUID2 ) );
}

void ContextPrivate::connectPins( const QUuid & pUUID1, const QUuid & pUUID2 )
{
	QMetaObject::invokeMethod( mContextWorker, "connectPins", Qt::BlockingQueuedConnection,
		Q_ARG( QUuid, pUUID1 ),
		Q_ARG( QUuid, pUUID2 )
	);
}

void ContextPrivate::disconnectPins( const QUuid & pUUID1, const QUuid & pUUID2 )
{
	QMetaObject::invokeMethod( mContextWorker, "disconnectPins", Qt::BlockingQueuedConnection,
		Q_ARG( QUuid, pUUID1 ),
		Q_ARG( QUuid, pUUID2 )
	);
}

void ContextPrivate::disconnectPin( const QUuid & pUUID )
{
	QMetaObject::invokeMethod( mContextWorker, "disconnectPin", Qt::BlockingQueuedConnection,
		Q_ARG( QUuid, pUUID )
	);
}

void ContextPrivate::performance( QSharedPointer<fugio::NodeInterface> pNode, const QString & pName, qint64 pTimeStart, qint64 pTimeEnd )
{
	mContextWorker->performance( pNode, pName, pTimeStart, pTimeEnd );
}

QList<fugio::PerfData> ContextPrivate::perfdata( void )
{
	return( mContextWorker->perfdata() );
}

void ContextPrivate::registerPlayhead( fugio::PlayheadInterface * pPlayHead )
{
	mContextWorker->registerPlayhead( pPlayHead );
}

void ContextPrivate::unregisterPlayhead( fugio::PlayheadInterface * pPlayHead )
{
	mContextWorker->unregisterPlayhead( pPlayHead );
}

bool ContextPrivate::isPlaying( void ) const
{
	return( mContextWorker->isPlaying() );
}

void ContextPrivate::setPlayheadTimerEnabled( bool pEnabled )
{
	mContextWorker->setPlayheadTimerEnabled( pEnabled );
}

void ContextPrivate::playheadMove( qreal pTimeStamp )
{
	mContextWorker->playheadMove( pTimeStamp );
}

void ContextPrivate::playheadPlay( qreal pTimePrev, qreal pTimeCurr )
{
	mContextWorker->playheadPlay( pTimePrev, pTimeCurr );
}

void ContextPrivate::play( void )
{
	mContextWorker->play();
}

void ContextPrivate::stop( void )
{
	mContextWorker->stop();
}

void ContextPrivate::pause( void )
{
	mContextWorker->pause();
}

void ContextPrivate::resume( void )
{
	mContextWorker->resume();
}

void ContextPrivate::setPlayheadPosition( qreal pTimeStamp, bool pRefresh )
{
	mContextWorker->setPlayheadPosition( pTimeStamp, pRefresh );
}

fugio::ContextInterface::TimeState ContextPrivate::state( void ) const
{
	return( mContextWorker->state() );
}

qreal ContextPrivate::position( void ) const
{
	return( mContextWorker->position() );
}

qreal ContextPrivate::duration( void ) const
{
	return( mContextWorker->duration() );
}

void ContextPrivate::setDuration( qreal pDuration )
{
	mContextWorker->setDuration( pDuration );
}

void ContextPrivate::notifyAboutToPlay( void )
{
	mContextWorker->notifyAboutToPlay();
}

QUuid ContextPrivate::registerExternalAsset( const QString & pFileName )
{
	return( mContextWorker->registerExternalAsset( pFileName ) );
}

void ContextPrivate::unregisterExternalAsset( const QString & pFileName )
{
	mContextWorker->unregisterExternalAsset( pFileName );
}

void ContextPrivate::unregisterExternalAsset( const QUuid & pUuid )
{
	mContextWorker->unregisterExternalAsset( pUuid );
}

QString ContextPrivate::externalAsset( const QUuid & pUuid ) const
{
	return( mContextWorker->externalAsset( pUuid ) );
}

void ContextPrivate::clear()
{
	QMetaObject::invokeMethod( mContextWorker, "clear", Qt::BlockingQueuedConnection );
}

bool ContextPrivate::load( QString pFileName, bool pPartial )
{
	bool	RetVal;

	QMetaObject::invokeMethod( mContextWorker, "load", Qt::BlockingQueuedConnection,
		Q_RETURN_ARG( bool, RetVal ),
		Q_ARG( QString, pFileName ),
		Q_ARG( bool, pPartial )
	);

	return( RetVal );
}

bool ContextPrivate::save( QString pFileName, QList<QUuid> pNodeList ) const
{
	bool	RetVal;

	QMetaObject::invokeMethod( mContextWorker, "save", Qt::BlockingQueuedConnection,
		Q_RETURN_ARG( bool, RetVal ),
		Q_ARG( QString, pFileName ),
		Q_ARG( QList<QUuid>, pNodeList )
	);

	return( RetVal );
}

//bool ContextPrivate::load( const QString &pFileName, bool pPartial )
//{
//	if( isContextThread() )
//	{
//		return( mContextWorker->load( pFileName, pPartial ) );
//	}

//	bool	Loaded;

//	QMetaObject::invokeMethod( mContextWorker, "load", Qt::BlockingQueuedConnection,
//							   Q_RETURN_ARG( bool, Loaded ),
//							   Q_ARG( QString, pFileName ),
//							   Q_ARG( bool, pPartial ) );

//	return( Loaded );
//}

bool ContextWorker::unload( QString pFileName )
{
	Q_UNUSED( pFileName )

//	if( !QFile( pFileName ).exists() )
//	{
//		qWarning() << pFileName << "doesn't exist";

//		return( false );
//	}

//	QSettings										 CFG( pFileName, QSettings::IniFormat );
////	int												 VER = 1;
////	bool											 RET = false;

//	if( CFG.status() != QSettings::NoError )
//	{
//		qWarning() << pFileName << "can't load";

//		return( false );
//	}

//	if( CFG.format() != QSettings::IniFormat )
//	{
//		qWarning() << pFileName << "bad format";

//		return( false );
//	}

//	mActive = false;

//	mActive = true;

	return( true );
}

void ContextWorker::loadNodeSettings( QSettings &pSettings, QVariantHash &pVarHsh, QStringList &pVarBse ) const
{
	Q_ASSERT( isContextThread() );

	for( const QString &K : pSettings.childGroups() )
	{
		pSettings.beginGroup( K );

		pVarBse.append( K );

		loadNodeSettings( pSettings, pVarHsh, pVarBse );

		pVarBse.removeLast();

		pSettings.endGroup();
	}

	for( const QString &K : pSettings.childKeys() )
	{
		pVarBse.append( K );

		pVarHsh.insert( pVarBse.join( "/" ), pSettings.value( K ) );

		pVarBse.removeLast();
	}
}

bool ContextWorker::loadSettings( QSettings &pSettings, bool pPartial )
{
	Q_ASSERT( isContextThread() );

	QSettings		CFG( pSettings.fileName(), pSettings.format() );

	QList<QSharedPointer<fugio::NodeInterface>>		NodeLst;

	QMap<QUuid,QUuid>		NodeMap;
	QMap<QUuid,QUuid>		PinsMap;

	pSettings.beginGroup( "nodes" );

	for( const QString &K : pSettings.childKeys() )
	{
		QString			NodeName     = K;
		const QUuid		NodeOrigUuid = fugio::utils::string2uuid( K );
		const QUuid		NodeUuid     = ( findNode( NodeOrigUuid ).isNull() ? NodeOrigUuid : QUuid::createUuid() );
		const QUuid		ControlUuid  = fugio::utils::string2uuid( pSettings.value( K ).toString() );
		QVariantHash	NodeData;

		if( CFG.childGroups().contains( K ) )
		{
			CFG.beginGroup( K );

			NodeName = CFG.value( "name", NodeName ).toString();

			if( CFG.childGroups().contains( "settings" ) )
			{
				QStringList		VarBse;

				CFG.beginGroup( "settings" );

				loadNodeSettings( CFG, NodeData, VarBse );

				CFG.endGroup();
			}

			CFG.endGroup();
		}

		QSharedPointer<fugio::NodeInterface>	N = createNode( NodeName, NodeUuid, ControlUuid, NodeData );

		if( !N )
		{
			qWarning() << "Can't create node" << NodeName;

			continue;
		}

		NodeLst << N;

		NodeMap.insert( NodeUuid, NodeOrigUuid );
	}

	pSettings.endGroup();

	//-------------------------------------------------------------------------

	for( QSharedPointer<fugio::NodeInterface> N : NodeLst )
	{
		const QUuid		NodeUuid = N->uuid();
		const QUuid		OrigUuid = NodeMap.value( N->uuid() );

		// do this before we registerNode() to give everyone a chance to record the relabel
		// and look up data

		if( NodeUuid != OrigUuid )
		{
			emit qobject()->nodeRelabled( OrigUuid, NodeUuid );
		}

		registerNode( N );

		pSettings.beginGroup( fugio::utils::uuid2string( OrigUuid ) );

		N->loadSettings( pSettings, PinsMap, pPartial );

		pSettings.endGroup();
	}

	//-------------------------------------------------------------------------

//	CFG.beginGroup( "connections" );

//	foreach( const QString &NodeDst, CFG.childGroups() )
//	{
//		CF2.beginGroup( NodeDst );

//		foreach( const QString &PinDst, CFG.childKeys() )
//		{
//			QStringList		 SrcList = CFG.value( PinDst ).toString().split( '\\' );

//			if( SrcList.size() != 2 )
//			{
//				continue;
//			}

//			mConnectionMap.insert( ConnectionPair( SrcList.at( 0 ), SrcList.at( 1 ) ), ConnectionPair( NodeDst, PinDst ) );
//		}

//		CFG.endGroup();
//	}

//	CFG.endGroup();

	//-------------------------------------------------------------------------

	pSettings.beginGroup( "connections" );

	for( const QString &G : pSettings.childKeys() )
	{
		QUuid		SrcId = PinsMap.value( fugio::utils::string2uuid( G ) );

		PinHash::iterator		SrcIt = mPinHash.find( SrcId );

		if( SrcIt == mPinHash.end() )
		{
			qWarning() << "SRC PIN NOT FOUND" << SrcId;

			continue;
		}

		QSharedPointer<fugio::PinInterface>	SrcP = SrcIt.value();

		if( !SrcP )
		{
			continue;
		}

		QUuid		DstId = PinsMap.value( fugio::utils::string2uuid( pSettings.value( G ).toString() ) );

		PinHash::iterator		DstIt = mPinHash.find( DstId );

		if( DstIt == mPinHash.end() )
		{
			qWarning() << "DST PIN NOT FOUND" << DstId;

			continue;
		}

		QSharedPointer<fugio::PinInterface>	DstP = DstIt.value();

		if( !DstP )
		{
			continue;
		}

		connectPins( SrcP->globalId(), DstP->globalId() );
	}

	pSettings.endGroup();

	return( true );
}

bool ContextWorker::save( QString pFileName, QList<QUuid> pNodeList ) const
{
	Q_ASSERT( isContextThread() );

	QFileInfo				 FileInfo( pFileName );
	QString					 TmpFileName = FileInfo.absoluteFilePath().append( ".out" );

	if( true )
	{
		QSettings				 CFG( TmpFileName, QSettings::IniFormat );

		if( !CFG.isWritable() )
		{
			return( false );
		}

		CFG.clear();

		emit saveStart( CFG );

		CFG.beginGroup( "fugio" );

		CFG.setValue( "version", int( 2 ) );

		CFG.setValue( "duration", double( duration() ) );

		CFG.endGroup();

		//-------------------------------------------------------------------------

		CFG.beginGroup( "meta" );

		for( auto it = mMetaInfoMap.begin() ; it != mMetaInfoMap.end() ; it++ )
		{
			const QString	K = mMetaNameMap.value( it.key() );
			const QString	V = it.value();

			if( !K.isEmpty() && !V.isEmpty() )
			{
				CFG.setValue( K, V );
			}
		}

		CFG.endGroup();

		//-------------------------------------------------------------------------

		CFG.beginGroup( "nodes" );

		for( QSharedPointer<fugio::NodeInterface> N : mNodeHash.values() )
		{
			if( pNodeList.isEmpty() || pNodeList.contains( N->uuid() ) )
			{
				CFG.setValue( fugio::utils::uuid2string( N->uuid() ), fugio::utils::uuid2string( N->controlUuid() ) );
			}
		}

		CFG.endGroup();

		//-------------------------------------------------------------------------

		CFG.beginGroup( "connections" );

		for( QSharedPointer<fugio::NodeInterface> N : mNodeHash.values() )
		{
			if( pNodeList.isEmpty() || pNodeList.contains( N->uuid() ) )
			{
				for( QSharedPointer<fugio::PinInterface> P : N->enumInputPins() )
				{
					if( !P->isConnected() )
					{
						continue;
					}

					QSharedPointer<fugio::PinInterface>	ConPin = P->connectedPin();

					if( !ConPin || !ConPin->node() )
					{
						continue;
					}

					if( pNodeList.isEmpty() || pNodeList.contains( ConPin->node()->uuid() ) )
					{
						CFG.setValue( fugio::utils::uuid2string( P->globalId() ), fugio::utils::uuid2string( ConPin->globalId() ) );
					}
				}
			}
		}

		CFG.endGroup();

		//-------------------------------------------------------------------------

		for( QSharedPointer<fugio::NodeInterface> N : mNodeHash.values() )
		{
			if( pNodeList.isEmpty() || pNodeList.contains( N->uuid() ) )
			{
				CFG.beginGroup( fugio::utils::uuid2string( N->uuid() ) );

				N->saveSettings( CFG, false );

				CFG.endGroup();

				for( QSharedPointer<fugio::PinInterface> P : N->enumPins() )
				{
					CFG.beginGroup( fugio::utils::uuid2string( P->globalId() ) );

					P->saveSettings( CFG );

					CFG.endGroup();
				}
			}
		}

		//-------------------------------------------------------------------------

		CFG.beginGroup( "assets" );

		for( auto it : mAssetMap.toStdMap() )
		{
			QFileInfo		FI( CFG.fileName() );
			QDir			FD( FI.absolutePath() );
			QString			AP = FD.absoluteFilePath( it.second );

			qDebug() << AP;

			CFG.setValue( fugio::utils::uuid2string( it.first ), AP );
		}

		CFG.endGroup();

		//-------------------------------------------------------------------------

		emit saving( CFG );

		emit saveEnd( CFG );
	}

	QString		TmpOld;

	if( FileInfo.exists() )
	{
		TmpOld = FileInfo.dir().absoluteFilePath( FileInfo.completeBaseName() ).append( ".old" );

		if( !QFile::rename( pFileName, TmpOld ) )
		{
			qWarning() << "Couldn't rename output file";

			return( false );
		}
	}

	if( true )
	{
		QFile		SrcDat( TmpFileName );
		QFile		DstDat( pFileName );

		if( !SrcDat.open( QFile::ReadOnly ) )
		{
			qWarning() << "Couldn't open temporary file";

			return( false );
		}

		if( !DstDat.open( QFile::WriteOnly ) )
		{
			qWarning() << "Couldn't open output file";

			return( false );
		}

		QStringList	HdrLst;

		HdrLst << QString( ";-----------------------------------------------------------------" );
		HdrLst << QString( "; Created with Fugio %1" ).arg( QCoreApplication::applicationVersion() );

		for( auto it = mMetaInfoMap.begin() ; it != mMetaInfoMap.end() ; it++ )
		{
			const QString	K = mMetaNameMap.value( it.key() );
			const QString	V = it.value();

			if( !K.isEmpty() && !V.isEmpty() )
			{
				HdrLst << QString( "; %1: %2" ).arg( K ).arg( V );
			}
		}

		HdrLst << QString( ";-----------------------------------------------------------------" );
		HdrLst << QString( "" );
		HdrLst << QString( "" );

		QByteArray	TmpDat;

		TmpDat = HdrLst.join( "\n" ).toLatin1();

		while( !TmpDat.isEmpty() )
		{
			if( DstDat.write( TmpDat ) != TmpDat.size() )
			{
				qWarning() << "Couldn't write output data";

				return( false );
			}

			TmpDat = SrcDat.read( 1024 );
		}

		SrcDat.close();
		DstDat.close();
	}

	if( !QFile::remove( TmpFileName ) )
	{
		qWarning() << "Couldn't remove temporary file";
	}

	if( !TmpOld.isEmpty() && !QFile::remove( TmpOld ) )
	{
		qWarning() << "Couldn't remove old file";
	}

	return( true );
}

bool ContextWorker::loadData( QString pFileName )
{
	Q_ASSERT( isContextThread() );

	if( !QFile( pFileName ).exists() )
	{
		qWarning() << pFileName << "doesn't exist";

		return( false );
	}

	QSettings										 CFG( pFileName, QSettings::IniFormat );
	int												 VER = 1;
//	bool											 RET = false;

	if( CFG.status() != QSettings::NoError )
	{
		qWarning() << pFileName << "can't load";

		return( false );
	}

	if( CFG.format() != QSettings::IniFormat )
	{
		qWarning() << pFileName << "bad format";

		return( false );
	}

	mActive = false;

	CFG.beginGroup( "fugio" );

	VER = CFG.value( "version", VER ).toInt();

	CFG.endGroup();

	for( NodeHash::const_iterator it = mNodeHash.begin() ; it != mNodeHash.end() ; it++ )
	{
		QSharedPointer<fugio::NodeInterface>	NI = it.value();

		if( NI->hasControl() )
		{
			CFG.beginGroup( fugio::utils::uuid2string( NI->uuid() ) );

			NI->control()->loadSettings( CFG );

			CFG.endGroup();

			nodeUpdated( NI->uuid() );
		}
	}

	for( PinHash::const_iterator it = mPinHash.begin() ; it != mPinHash.end() ; it++ )
	{
		QSharedPointer<fugio::PinInterface>		PI = it.value();

		CFG.beginGroup( fugio::utils::uuid2string( PI->globalId() ) );

		if( PI->hasControl() )
		{
			CFG.beginGroup( "control" );

			PI->control()->loadSettings( CFG );

			CFG.endGroup();
		}

		if( PI->direction() == PIN_INPUT )
		{
			QVariant	V = CFG.value( "value" );

			if( V.isValid() && V != PI->value() )
			{
				PI->setValue( V );

				pinUpdated( PI );
			}
		}

		CFG.endGroup();
	}

	mActive = true;

	return( true );
}

bool ContextWorker::saveData( QString pFileName ) const
{
	Q_ASSERT( isContextThread() );

	QFileInfo				 FileInfo( pFileName );
	QString					 TmpFileName = FileInfo.absoluteFilePath().append( ".out" );

	if( true )
	{
		QSettings				 CFG( TmpFileName, QSettings::IniFormat );

		if( !CFG.isWritable() )
		{
			return( false );
		}

		CFG.clear();

		emit saveStart( CFG );

		CFG.beginGroup( "fugio" );

		CFG.setValue( "version", int( 2 ) );

		CFG.endGroup();

		for( NodeHash::const_iterator it = mNodeHash.begin() ; it != mNodeHash.end() ; it++ )
		{
			QSharedPointer<fugio::NodeInterface>	NI = it.value();

			if( NI->hasControl() )
			{
				CFG.beginGroup( fugio::utils::uuid2string( NI->uuid() ) );

				NI->control()->saveSettings( CFG );

				CFG.endGroup();
			}
		}

		for( PinHash::const_iterator it = mPinHash.begin() ; it != mPinHash.end() ; it++ )
		{
			QSharedPointer<fugio::PinInterface>		PI = it.value();

			CFG.beginGroup( fugio::utils::uuid2string( PI->globalId() ) );

			if( PI->hasControl() )
			{
				CFG.beginGroup( "control" );

				PI->control()->saveSettings( CFG );

				CFG.endGroup();
			}

			if( PI->direction() == PIN_INPUT && PI->value().isValid() )
			{
				CFG.setValue( "value", PI->value() );
			}

			CFG.endGroup();
		}
	}

	QString		TmpOld;

	if( FileInfo.exists() )
	{
		TmpOld = FileInfo.dir().absoluteFilePath( FileInfo.completeBaseName() ).append( ".old" );

		if( !QFile::rename( pFileName, TmpOld ) )
		{
			qWarning() << "Couldn't rename output file";

			return( false );
		}
	}

	if( !QFile::copy( TmpFileName, pFileName ) )
	{
		qWarning() << "Couldn't copy temporary file";
	}

	if( !QFile::remove( TmpFileName ) )
	{
		qWarning() << "Couldn't remove temporary file";
	}

	if( !TmpOld.isEmpty() && !QFile::remove( TmpOld ) )
	{
		qWarning() << "Couldn't remove old file";
	}

	return( true );
}

void ContextWorker::clear( void )
{
	Q_ASSERT( isContextThread() );

	for( QUuid ID : mNodeHash.keys() )
	{
		unregisterNode( ID );
	}

	// disconnect all pins

	for( QUuid PID : mConnectIO.keys() )
	{
		disconnectPin( PID );
	}

	for( QSharedPointer<fugio::NodeInterface> N : mNodeHash.values() )
	{
		if( NodePrivate *NP = qobject_cast<NodePrivate *>( N->qobject() ) )
		{
			NP->clear();
		}
	}

	emit clearContext();

	QMutexLocker	MutLck( &mUpdatePinMapMutex );

	mUpdatedNodeList.clear();
	mFinishedNodeList.clear();
	mInitDeferNodeList.clear();

	mUpdatePinMap.clear();

	mNodeHash.clear();
	mPinHash.clear();

	mConnectIO.clear();
	mConnectOI.clear();

	mMetaInfoMap.clear();
}

fugio::ContextSignals *ContextWorker::qobject()
{
	return( this );
}

fugio::GlobalInterface *ContextWorker::global( void )
{
	return( mContextPrivate->global() );
}

//void ContextPrivate::setActive( bool pActive )
//{
//	QMetaObject::invokeMethod( mContextWorker, "setActive", Q_ARG( bool, pActive ) );
//}

//bool ContextPrivate::active( void ) const
//{
//	return( mContextWorker->active() );
//}

QString ContextWorker::metaInfo( fugio::ContextInterface::MetaInfo pType) const
{
	return( mMetaInfoMap.value( pType ) );
}

void ContextWorker::setMetaInfo(fugio::ContextInterface::MetaInfo pType, const QString &pMetaData)
{
	if( mMetaInfoMap.value( pType ) == pMetaData )
	{
		return;
	}

	mMetaInfoMap.insert( pType, pMetaData );

	emit metaInfoChanged( pType, pMetaData );
}

void ContextWorker::registerPlayhead( fugio::PlayheadInterface *pPlayHead )
{
	mPlayheadList.removeAll( pPlayHead );

	mPlayheadList.append( pPlayHead );
}

void ContextWorker::unregisterPlayhead( fugio::PlayheadInterface *pPlayHead )
{
	mPlayheadList.removeAll( pPlayHead );
}

bool ContextWorker::isPlaying( void ) const
{
	return( mState == fugio::ContextInterface::Playing );
}

void ContextWorker::registerNode( QSharedPointer<fugio::NodeInterface> pNode )
{
	Q_ASSERT( isContextThread() );

	if( mNodeHash.contains( pNode->uuid() ) )
	{
		if( NodePrivate *NP = qobject_cast<NodePrivate *>( pNode->qobject() ) )
		{
			NP->setUuid( QUuid::createUuid() );
		}
	}

	pNode->setContext( this );

	mNodeHash.insert( pNode->uuid(), pNode );

	fugio::NodeSignals		*N = pNode->qobject();

	connect( N, SIGNAL(controlChanged(QSharedPointer<fugio::NodeInterface>)), this, SLOT(nodeControUpdated(QSharedPointer<fugio::NodeInterface>)) );
	connect( N, SIGNAL(nameChanged(QSharedPointer<fugio::NodeInterface>)), this, SLOT(nodeNameUpdated(QSharedPointer<fugio::NodeInterface>)) );
	connect( N, SIGNAL(activationChanged(QSharedPointer<fugio::NodeInterface>)), this, SLOT(nodeActivationUpdated(QSharedPointer<fugio::NodeInterface>)) );

	connect( N, SIGNAL(pinAdded(QSharedPointer<fugio::NodeInterface>,QSharedPointer<fugio::PinInterface>)), this, SLOT(onPinAdded(QSharedPointer<fugio::NodeInterface>,QSharedPointer<fugio::PinInterface>)) );
	connect( N, SIGNAL(pinRemoved(QSharedPointer<fugio::NodeInterface>,QSharedPointer<fugio::PinInterface>)), this, SLOT(onPinRemoved(QSharedPointer<fugio::NodeInterface>,QSharedPointer<fugio::PinInterface>)) );

	addDeferredNode( pNode );

	mNodeDeferProcess = true;

	emit nodeAdded( pNode->uuid() );
}

void ContextWorker::unregisterNode( const QUuid &pUUID )
{
	Q_ASSERT( isContextThread() );

	for( int i = 0 ; i < mInitDeferNodeList.size() ; )
	{
		if( mInitDeferNodeList.at( i )->uuid() == pUUID )
		{
			mInitDeferNodeList.removeAt( i );
		}
		else
		{
			i++;
		}
	}

	QSharedPointer<fugio::NodeInterface>	Node = mNodeHash.value( pUUID );

	if( Node )
	{
		mNodeHash.remove( pUUID );

		Node->qobject()->disconnect( this );

		if( Node->hasControl() )
		{
			Node->control()->deinitialise();
		}

		Node->setStatus( fugio::NodeInterface::Initialising );

		NodePrivate		*NP = qobject_cast<NodePrivate *>( Node->qobject() );

		NP->clear();

		Node->setContext( nullptr );

		emit nodeRemoved( pUUID );
	}
}

void ContextWorker::renameNode( const QUuid &pUUID1, const QUuid &pUUID2 )
{
	Q_ASSERT( isContextThread() );

	if( pUUID1 == pUUID2 )
	{
		return;
	}

	if( mNodeHash.contains( pUUID2 ) )
	{
		qWarning() << "ContextPrivate::renameNode - mNodeHash.contains( pUUID2 )";

		return;
	}

	NodeHash::iterator it = mNodeHash.find( pUUID1 );

	if( it == mNodeHash.end() )
	{
		qWarning() << "ContextPrivate::renameNode - !mNodeHash.contains( pUUID1 )";

		return;
	}

	QSharedPointer<fugio::NodeInterface>	Node = it.value();

	mNodeHash.remove( pUUID1 );

	mNodeHash.insert( pUUID2, Node );

	if( mConnectIO.contains( pUUID1 ) )
	{
		QUuid		D = mConnectIO.take( pUUID1 );

		mConnectIO.insert( pUUID2, D );
	}

	if( mConnectOI.contains( pUUID1 ) )
	{
		QList<QUuid>	L = mConnectOI.values( pUUID1 );

		for( QList<QUuid>::const_iterator it = L.begin() ; it != L.end() ; it++ )
		{
			mConnectOI.remove( pUUID1, *it );

			mConnectOI.insert( pUUID2, *it );
		}
	}

#if defined( QT_DEBUG ) && defined( DEBUG_CONNECTIONS )
	qDebug() << "ContextPrivate::renameNode" << pUUID1 << pUUID2;
#endif

	emit nodeRenamed( pUUID1, pUUID2 );
}

QSharedPointer<fugio::NodeInterface> ContextWorker::findNode( const QUuid &pUUID ) const
{
	NodeHash::const_iterator it = mNodeHash.find( pUUID );

	if( it == mNodeHash.end() )
	{
		return( QSharedPointer<fugio::NodeInterface>() );
	}

	return( it.value() );
}

QSharedPointer<fugio::NodeInterface> ContextWorker::findNode( const QString &pName ) const
{
	for( NodeHash::const_iterator it = mNodeHash.begin() ; it != mNodeHash.end() ; it++ )
	{
		if( it.value()->name() != pName )
		{
			continue;
		}

		return( it.value() );
	}

	return( QSharedPointer<fugio::NodeInterface>() );
}

void ContextWorker::registerPin( QSharedPointer<fugio::PinInterface> pPin )
{
	Q_ASSERT( isContextThread() );

	if( !mPinHash.contains( pPin->globalId() ) )
	{
		mPinHash.insert( pPin->globalId(), pPin );
	}
}

void ContextWorker::unregisterPin( const QUuid &pUUID )
{
	Q_ASSERT( isContextThread() );

	QSharedPointer<fugio::PinInterface>		PinInt = findPin( pUUID );

	if( PinInt )
	{
		mUpdatePinMapMutex.lock();

		for( int i = 0 ; i < mUpdatePinMap.size() ; )
		{
			QSharedPointer<fugio::PinInterface>		DstPin = mUpdatePinMap.at( i ).first;

			if( DstPin && DstPin->globalId() == pUUID )
			{
				mUpdatePinMap.removeAt( i );
			}
			else
			{
				i++;
			}
		}

		mUpdatePinMapMutex.unlock();
	}

	if( mPinHash.contains( pUUID ) )
	{
		mPinHash.remove( pUUID );
	}

	disconnectPin( pUUID );
}

void ContextWorker::renamePin( const QUuid &pUUID1, const QUuid &pUUID2 )
{
	Q_ASSERT( isContextThread() );

	if( pUUID1 == pUUID2 )
	{
		return;
	}

	if( mPinHash.contains( pUUID2 ) )
	{
		return;
	}

	PinHash::iterator it = mPinHash.find( pUUID1 );

	if( it == mPinHash.end() )
	{
		return;
	}

	QSharedPointer<fugio::PinInterface>	PIN = it.value();

	mPinHash.remove( pUUID1 );

	mPinHash.insert( pUUID2, PIN );

	if( mConnectIO.contains( pUUID1 ) )
	{
		QUuid		D = mConnectIO.take( pUUID1 );

		mConnectIO.insert( pUUID2, D );
	}

	if( mConnectOI.contains( pUUID1 ) )
	{
		QList<QUuid>	L = mConnectOI.values( pUUID1 );

		for( QList<QUuid>::const_iterator it = L.begin() ; it != L.end() ; it++ )
		{
			mConnectOI.remove( pUUID1, *it );

			mConnectOI.insert( pUUID2, *it );
		}
	}

#if defined( QT_DEBUG ) && defined( DEBUG_CONNECTIONS )
	qDebug() << "ContextPrivate::renamePin" << pUUID1 << pUUID2;
#endif

	emit pinRenamed( PIN->node()->uuid(), pUUID1, pUUID2 );
}

QSharedPointer<fugio::PinInterface> ContextWorker::findPin( const QUuid &pUUID )
{
	PinHash::iterator it = mPinHash.find( pUUID );

	if( it == mPinHash.end() )
	{
		return( QSharedPointer<fugio::PinInterface>() );
	}

	return( it.value() );
}

QList< QSharedPointer<fugio::PinInterface> > ContextWorker::connections( const QUuid &pUUID )
{
	QList< QSharedPointer<fugio::PinInterface> >	PinLst;

	if( mConnectIO.contains( pUUID ) )
	{
		PinLst.append( mPinHash.value( mConnectIO.value( pUUID ) ) );
	}

	if( mConnectOI.contains( pUUID ) )
	{
		QList<QUuid>	ValLst = mConnectOI.values( pUUID );

		for( const QUuid &UUID : ValLst )
		{
			PinLst.append( mPinHash.value( UUID ) );
		}
	}

	return( PinLst );
}

bool ContextWorker::isConnected( const QUuid &pUUID )
{
	return( mConnectIO.contains( pUUID ) || mConnectOI.contains( pUUID ) );
}

bool ContextWorker::isConnectedTo( const QUuid &pUUID1, const QUuid &pUUID2 )
{
	return( mConnectIO.value( pUUID1 ) == pUUID2 || mConnectIO.value( pUUID2 ) == pUUID1 || mConnectOI.contains( pUUID1, pUUID2 ) || mConnectOI.contains( pUUID2, pUUID1 ) );
}

void ContextWorker::connectPins( const QUuid &pUUID1, const QUuid &pUUID2 )
{
	Q_ASSERT( isContextThread() );

#if defined( QT_DEBUG ) && defined( DEBUG_CONNECTIONS )
	qDebug() << "connect" << pUUID1 << "->" << pUUID2;
#endif

	QSharedPointer<fugio::PinInterface>	P1 = mPinHash.value( pUUID1 );
	QSharedPointer<fugio::PinInterface>	P2 = mPinHash.value( pUUID2 );

	if( !P1 || !P2 )
	{
		return;
	}

	if( P1->direction() == PIN_INPUT )
	{
		mConnectIO.insert( pUUID1, pUUID2 );
		mConnectOI.insert( pUUID2, pUUID1 );

		emit linkAdded( pUUID1, pUUID2 );

		pinUpdated( P2 );
	}
	else
	{
		mConnectIO.insert( pUUID2, pUUID1 );
		mConnectOI.insert( pUUID1, pUUID2 );

		emit linkAdded( pUUID2, pUUID1 );

		pinUpdated( P1 );
	}

	emit P1->node()->qobject()->pinLinked( P1, P2 );
	emit P2->node()->qobject()->pinLinked( P2, P1 );

	emit P1->qobject()->linked( P2 );
	emit P2->qobject()->linked( P1 );
}

void ContextWorker::disconnectPins( const QUuid &pUUID1, const QUuid &pUUID2 )
{
	Q_ASSERT( isContextThread() );

#if defined( QT_DEBUG ) && defined( DEBUG_CONNECTIONS )
	qDebug() << "disconnect" << pUUID1 << "->" << pUUID2;
#endif

	QSharedPointer<fugio::PinInterface>	P1 = mPinHash.value( pUUID1 );
	QSharedPointer<fugio::PinInterface>	P2 = mPinHash.value( pUUID2 );

	if( P1->direction() == PIN_INPUT )
	{
		mConnectIO.remove( pUUID1 );
		mConnectOI.remove( pUUID2, pUUID1 );

		emit linkRemoved( pUUID1, pUUID2 );

		pinUpdated( P1 );
	}
	else
	{
		mConnectIO.remove( pUUID2 );
		mConnectOI.remove( pUUID1, pUUID2 );

		emit linkRemoved( pUUID2, pUUID1 );

		pinUpdated( P2 );
	}

	emit P1->node()->qobject()->pinUnlinked( P1, P2 );
	emit P2->node()->qobject()->pinUnlinked( P2, P1 );

	emit P1->qobject()->unlinked( P2 );
	emit P2->qobject()->unlinked( P1 );
}

void ContextWorker::disconnectPin( const QUuid &pUUID )
{
	Q_ASSERT( isContextThread() );

	QSharedPointer<fugio::PinInterface>	P1 = mPinHash.value( pUUID );

	for( QHash<QUuid,QUuid>::iterator it = mConnectIO.begin() ; it != mConnectIO.end() ; it++ )
	{
		QSharedPointer<fugio::PinInterface>	P2;

		if( it.key() == pUUID )
		{
			emit linkRemoved( it.key(), it.value() );

			P2 = mPinHash.value( it.value() );
		}
		else if( it.value() == pUUID )
		{
			emit linkRemoved( it.value(), it.key() );

			P2 = mPinHash.value( it.key() );
		}

		if( P1 && P2 )
		{
			if( P1->direction() == PIN_INPUT )
			{
				pinUpdated( P1 );
			}
			else
			{
				pinUpdated( P2 );
			}

			emit P1->node()->qobject()->pinUnlinked( P1, P2 );
			emit P2->node()->qobject()->pinUnlinked( P2, P1 );

			emit P1->qobject()->unlinked( P2 );
			emit P2->qobject()->unlinked( P1 );
		}
	}

	mConnectIO.remove( pUUID );

	mConnectOI.remove( pUUID );
}

QList<QUuid> ContextWorker::nodeList() const
{
	return( mNodeHash.keys() );
}

void ContextWorker::onPinAdded( QSharedPointer<fugio::NodeInterface> pNode, QSharedPointer<fugio::PinInterface> pPin )
{
	Q_ASSERT( isContextThread() );

	registerPin( pPin );

	ConnectionPair				MapKey( pNode->name(), pPin->name() );

//	DefaultMap::iterator		DefaultIterator = mDefaultMap.find( MapKey );

//	if( DefaultIterator != mDefaultMap.end() )
//	{
//		pPin->setValue( DefaultIterator.value() );
//	}

	for( ConnectionMap::iterator it = mConnectionMap.begin() ; it != mConnectionMap.end() ; it++ )
	{
		if( pPin->direction() == PIN_OUTPUT && it.key() == MapKey )
		{
			for( NodeHash::iterator NodeIterator = mNodeHash.begin() ; NodeIterator != mNodeHash.end() ; NodeIterator++ )
			{
				QSharedPointer<fugio::NodeInterface>	Node = NodeIterator.value();

				if( Node->name() != it.value().first )
				{
					continue;
				}

				QSharedPointer<fugio::PinInterface>	Pin = Node->findPinByName( it.value().second );

				if( !Pin || Pin->direction() != PIN_INPUT )
				{
					continue;
				}

				connectPins( pPin->globalId(), Pin->globalId() );
			}

			break;
		}
		else if( pPin->direction() == PIN_INPUT && it.value() == MapKey )
		{
			for( NodeHash::iterator NodeIterator = mNodeHash.begin() ; NodeIterator != mNodeHash.end() ; NodeIterator++ )
			{
				QSharedPointer<fugio::NodeInterface>	Node = *NodeIterator;

				if( Node->name() != it.key().first )
				{
					continue;
				}

				QSharedPointer<fugio::PinInterface>	Pin = Node->findPinByName( it.key().second );

				if( !Pin || Pin->direction() != PIN_OUTPUT )
				{
					continue;
				}

				connectPins( pPin->globalId(), Pin->globalId() );
			}

			break;
		}
	}

	emit pinAdded( pNode->uuid(), pPin->globalId() );
}

void ContextWorker::onPinRemoved( QSharedPointer<fugio::NodeInterface> pNode, QSharedPointer<fugio::PinInterface> pPin )
{
	Q_ASSERT( isContextThread() );

	unregisterPin( pPin->globalId() );

	emit pinRemoved( pNode->uuid(), pPin->globalId() );
}

void ContextWorker::nodeControUpdated( QSharedPointer<fugio::NodeInterface> pNode )
{
	emit nodeUpdated( pNode->uuid() );
}

void ContextWorker::nodeNameUpdated( QSharedPointer<fugio::NodeInterface> pNode )
{
	emit nodeUpdated( pNode->uuid() );
}

void ContextWorker::nodeActivationUpdated( QSharedPointer<fugio::NodeInterface> pNode )
{
	emit nodeActivated( pNode->uuid() );
}

void ContextWorker::processPlayhead( qint64 pTimeStamp )
{
	if( !isPlaying() )
	{
		return;
	}

	/*
	mDataFrameLength = 20;
	mGuiFrameLength  = 40;

	mDataLastTime = mGuiLastTime = QDateTime::currentMSecsSinceEpoch();

	while( mRunning )
	{
		qint64		CurrTime = QDateTime::currentMSecsSinceEpoch();

		if( mDataLastTime + mDataFrameLength <= CurrTime )
		{
			emit updateData( qreal( mDataLastTime + mDataFrameLength ) / 1000.0 );

			do
			{
				mDataLastTime += mDataFrameLength;
			}
			while( mDataLastTime + mDataFrameLength <= CurrTime );
		}
*/

	qreal	PlayheadTimeStamp = qreal( pTimeStamp ) / 1000.0;

	if( mPlayheadGlobalOffset == -1 )
	{
		mPlayheadGlobalOffset = PlayheadTimeStamp;
	}

	qreal		CurrTime = ( PlayheadTimeStamp - mPlayheadGlobalOffset ) - mPlayheadLatencyOffset;

	if( CurrTime < 0 )
	{
		return;
	}

	//CurrTime = qBound( 0.0, CurrTime + mPlayheadLocalOffset, mDuration );

	CurrTime += mPlayheadLocalOffset;

	if( mPlayheadLocalLast == -1 )
	{
		mPlayheadLocalLast = mPlayheadLocalOffset;
	}

	//audio().playheadPlay( mPlayheadLocalLast, CurrTime );

	playheadPlay( mPlayheadLocalLast, CurrTime );

	//io().processUpdatedLinks( CurrTime );

	mPlayheadLocalLast = CurrTime;
}

void ContextWorker::notifyAboutToPlay()
{
	Q_ASSERT( isContextThread() );

	for( fugio::PlayheadInterface *PH : mPlayheadList )
	{
		PH->playStart( mPlayheadLocalLast );
	}

	emit aboutToPlay();
}

void ContextWorker::setDuration(qreal pDuration)
{
	if( pDuration == mDuration )
	{
		return;
	}

	mDuration = pDuration;

	emit durationChanged( mDuration );
}

void ContextWorker::play( void )
{
	Q_ASSERT( isContextThread() );

	if( mState == fugio::ContextInterface::Playing )
	{
		return;
	}

	mPlayheadLatencyOffset = 0;//audio().latency();

	for( fugio::PlayheadInterface *PH : mPlayheadList )
	{
		mPlayheadLatencyOffset = qMax( mPlayheadLatencyOffset, PH->latency() );
	}

	mPlayheadGlobalOffset = -1;
	mPlayheadLocalOffset   = mPlayheadLocalLast;
	mPlayheadLocalLast     = -1;

	for( fugio::PlayheadInterface *PH : mPlayheadList )
	{
		PH->setTimeOffset( mPlayheadLocalOffset );
	}

	notifyAboutToPlay();

	mState = fugio::ContextInterface::Playing;

	emit stateChanged( mState );
}

void ContextWorker::playFrom( qreal pTimeStamp )
{
	Q_ASSERT( isContextThread() );

	playheadMove( pTimeStamp );

	mPlayheadLatencyOffset = 0;//audio().latency();

	for( fugio::PlayheadInterface *PH : mPlayheadList )
	{
		mPlayheadLatencyOffset = qMax( mPlayheadLatencyOffset, PH->latency() );
	}

	mPlayheadGlobalOffset = -1;
	mPlayheadLocalOffset   = pTimeStamp;//mComposition->trackdata()->position();
	mPlayheadLocalLast     = -1;

	for( fugio::PlayheadInterface *PH : mPlayheadList )
	{
		PH->setTimeOffset( mPlayheadLocalOffset );
	}

	notifyAboutToPlay();

	if( mState == fugio::ContextInterface::Playing )
	{
		return;
	}

	mState = fugio::ContextInterface::Playing;

	emit stateChanged( mState );
}

void ContextWorker::stop()
{
	if( mState == fugio::ContextInterface::Stopped )
	{
		return;
	}

	mState = fugio::ContextInterface::Stopped;

	emit stateChanged( mState );
}

void ContextWorker::pause()
{
	if( mState == fugio::ContextInterface::Paused )
	{
		return;
	}

	mState = fugio::ContextInterface::Paused;

	emit stateChanged( mState );

	//mComposition->trackdata()->pause();
}

void ContextWorker::resume()
{
	if( mState != fugio::ContextInterface::Paused )
	{
		return;
	}

	mState = fugio::ContextInterface::Playing;

	emit stateChanged( mState );

	//mComposition->trackdata()->resume();
}

void ContextWorker::setPlayheadPosition( qreal pTimeStamp, bool pRefresh )
{
	Q_ASSERT( isContextThread() );

	if( isPlaying() )
	{
		return;
	}

	pTimeStamp = qBound( 0.0, pTimeStamp, duration() );

	if( pTimeStamp != mPlayheadLocalLast )
	{
		playheadMove( pTimeStamp );
	}
	else if( !pRefresh )
	{
		return;
	}

	emit playheadPosition( mPlayheadLocalLast, pTimeStamp );

	mPlayheadLocalLast = pTimeStamp;
}

void ContextWorker::playheadMove( qreal pTimeStamp )
{
	for( fugio::PlayheadInterface *PH : mPlayheadList )
	{
		PH->playheadMove( pTimeStamp );
	}
}

void ContextWorker::playheadPlay( qreal pTimePrev, qreal pTimeCurr )
{
	Q_ASSERT( isContextThread() );

	if( mPlayheadList.isEmpty() )
	{
		return;
	}

	int		RunCnt = 0;

	for( fugio::PlayheadInterface *PH : mPlayheadList )
	{
		if( PH->playheadPlay( pTimePrev, pTimeCurr ) )
		{
			RunCnt++;
		}
	}

	if( RunCnt == 0 )
	{
		stop();
	}
}

//#define LOG_NODE_UPDATES

void ContextWorker::nodeInitialised( void )
{
	mNodeDeferProcess = true;
}

void ContextWorker::pinUpdated( QSharedPointer<fugio::PinInterface> pPin, qint64 pGlobalTimestamp, bool pUpdatedConnectedNode )
{
	Q_ASSERT( isContextThread() );

	PinPrivate	*PP = qobject_cast<PinPrivate *>( pPin->qobject() );

	if( PP )
	{
		PP->setGlobalTimestamp( pGlobalTimestamp >= 0 ? pGlobalTimestamp : global()->timestamp() );
	}

	emit pPin->qobject()->updated();

//	if( !pPin->isConnectedToActiveNode() )
//	{
//		//pPin->mUpdated = qMax( pTimeStamp, pPin->mUpdated );

//		return;
//	}

	QMutexLocker	MutLck( &mUpdatePinMapMutex );

	for( UpdatePinEntry &PinEnt : mUpdatePinMap )
	{
		QSharedPointer<fugio::PinInterface>	DstPin = PinEnt.first;

		if( DstPin && DstPin->globalId() == pPin->globalId() )
		{
#if defined( LOG_NODE_UPDATES )
			qDebug() << pPin->name() << "is already updated";
#endif
			if( !PinEnt.second && pUpdatedConnectedNode )
			{
				PinEnt.second = true;
			}

			return;
		}
	}

	UpdatePinEntry		PE;

	PE.first  = pPin;
	PE.second = pUpdatedConnectedNode;

	mUpdatePinMap << PE;
}

void ContextWorker::performance( QSharedPointer<fugio::NodeInterface> pNode, const QString &pName, qint64 pTimeStart, qint64 pTimeEnd )
{
	PerfEntry		PE;

	PE.mUuid      = pNode ? pNode->uuid() : QUuid();
	PE.mNode      = pNode ? pNode->name() : QString();
	PE.mName      = pName;
	PE.mTimeStart = pTimeStart;
	PE.mTimeEnd   = pTimeEnd;

	mPerfList << PE;
}

QList<fugio::PerfData> ContextWorker::perfdata()
{
	QMap<QUuid,fugio::PerfData>		PDL;

	for( const PerfEntry &PE : mPerfList )
	{
		if( !PDL.contains( PE.mUuid ) )
		{
			fugio::PerfData	PD;

			PD.mName  = PE.mName;
			PD.mNode  = PE.mNode;
			PD.mUuid  = PE.mUuid;
			PD.mCount = 0;
			PD.mTime  = 0;

			PDL.insert( PE.mUuid, PD );
		}

		fugio::PerfData	&PD = PDL[ PE.mUuid ];

		PD.mTime  += PE.mTimeEnd - PE.mTimeStart;
		PD.mCount += 1;
	}

	return( PDL.values() );
}

void ContextWorker::processDeferredNodes( QList< QSharedPointer<fugio::NodeInterface> > WaitingNodes, QList< QSharedPointer<fugio::NodeInterface> > &InitialisedNodes )
{
	Q_ASSERT( isContextThread() );

	QStringList			DeferredNodeNames;

	for( QSharedPointer<fugio::NodeInterface> N : WaitingNodes )
	{
		if( !N->hasControl() )
		{
			N->setStatus( fugio::NodeInterface::Error );

			N->setStatusMessage( tr( "Node has no control class: missing plugin or error loading plugin?" ) );

			continue;
		}

		N->setStatus( fugio::NodeInterface::Deferred );

		if( N->control()->initialise() )
		{
			N->setStatus( fugio::NodeInterface::Initialised );

#if defined( LOG_NODE_UPDATES )
			qDebug() << "Node Initialised:" << N->name();
#endif

			N->control()->inputsUpdated( 0 );

			InitialisedNodes.append( N );
		}
		else if( N->status() == fugio::NodeInterface::Deferred )
		{
#if defined( LOG_NODE_UPDATES )
			qDebug() << "Node Deferred:" << N->name();
#endif
			mInitDeferNodeList.append( N );

			DeferredNodeNames << N->name();
		}
	}

	if( DeferredNodeNames != mLastDeferredNodeNames )
	{
		if( !DeferredNodeNames.isEmpty() )
		{
			qDebug() << "Deferred:" << DeferredNodeNames;
		}

		mLastDeferredNodeNames = DeferredNodeNames;
	}
}

bool ContextWorker::isContextThread() const
{
	return( mContextPrivate->isContextThread() );
}

void ContextWorker::processDeferredNodes()
{
	Q_ASSERT( isContextThread() );

	mUpdatedNodeMutex.lock();

#if defined( LOG_NODE_UPDATES )
	qDebug() << "Processing Deferred Nodes:" << mInitDeferNodeList.size();
#endif

	QList< QSharedPointer<fugio::NodeInterface> >		WaitingNodes = mInitDeferNodeList;
	QList< QSharedPointer<fugio::NodeInterface> >		InitialisedNodes;

	mInitDeferNodeList.clear();

	mUpdatedNodeMutex.unlock();

	processDeferredNodes( WaitingNodes, InitialisedNodes );

	//-------------------------------------------------------------------------

	for( QSharedPointer<fugio::NodeInterface> N : InitialisedNodes )
	{
		emit nodeComplete( N->uuid() );
	}

#if defined( LOG_NODE_UPDATES )
	qDebug() << "Nodes Remaining:" << mInitDeferNodeList.size();
#endif
}

void ContextWorker::updateNode( QSharedPointer<fugio::NodeInterface> pNode )
{
	Q_ASSERT( isContextThread() );

	if( !pNode )
	{
		return;
	}

	QMutexLocker		Lock( &mUpdatedNodeMutex );

	if( mFinishedNodeList.contains( pNode ) )
	{
		if( !mFutureNodeList.contains( pNode ) )
		{
#if defined( LOG_NODE_UPDATES )
			qDebug() << "Node:" << pNode->name() << "(Future)";
#endif

			mFutureNodeList.append( pNode );
		}
		else
		{
#if defined( LOG_NODE_UPDATES )
			qDebug() << "Node:" << pNode->name() << "(Already Finished)";
#endif
			mFutureNodeList.removeAll( pNode );

			mFutureNodeList.append( pNode );
		}
	}
	else if( mInitDeferNodeList.contains( pNode ) )
	{
#if defined( LOG_NODE_UPDATES )
		qDebug() << "Node:" << pNode->name() << "(Already Deferred)";
#endif

		mNodeDeferProcess = true;
	}
	else if( !mUpdatedNodeList.contains( pNode ) )
	{
#if defined( LOG_NODE_UPDATES )
		qDebug() << "Node:" << pNode->name() << "(Updated)";
#endif
		mUpdatedNodeList.append( pNode );
	}
	else
	{
#if defined( LOG_NODE_UPDATES )
		qDebug() << "Node:" << pNode->name() << "(Already Updated)";
#endif
		mUpdatedNodeList.removeAll( pNode );

		mUpdatedNodeList.append( pNode );
	}
}

void ContextWorker::processNode( QSharedPointer<fugio::NodeInterface> pNode, qint64 pTimeStamp )
{
	QSharedPointer<NodePrivate>	NP = qSharedPointerCast<NodePrivate>( pNode );

	if( NP )
	{
		// If the node's input pins are marked as updated, set the timestamp to this frame

		for( QSharedPointer<fugio::PinInterface> &P : pNode->enumInputPins() )
		{
			PinPrivate	*PP = dynamic_cast<PinPrivate *>( P->qobject() );

			if( P->updated() > pTimeStamp )
			{
				if( PP )
				{
					PP->setUpdated( pTimeStamp );
				}
			}
		}

		NP->inputsUpdated( pTimeStamp );
	}
}

//void ContextPrivate::processContext( fugio::ContextInterface *pContext )
//{
//	Q_UNUSED( pContext )
//}

void ContextWorker::processUpdatedNodes( qint64 pTimeStamp )
{
	Q_ASSERT( isContextThread() );

	QList< QSharedPointer<fugio::NodeInterface> >		Nodes;

	mUpdatedNodeMutex.lock();

	Nodes = mUpdatedNodeList;

	mUpdatedNodeList.clear();

	//mFinishedNodeList.append( Nodes );

	mUpdatedNodeMutex.unlock();

	for( QList< QSharedPointer<fugio::NodeInterface> >::iterator it = Nodes.begin() ; it != Nodes.end() ; it++ )
	{
		QSharedPointer<fugio::NodeInterface>		NODE = *it;

		if( !mFinishedNodeList.contains( NODE ) )
		{
#if defined( LOG_NODE_UPDATES )
			qDebug() << "Node Processing:" << NODE->name();
#endif

			if( NODE->hasControl() )
			{
				processNode( NODE, pTimeStamp );
			}

			mUpdatedNodeMutex.lock();

			mFinishedNodeList.append( NODE );

			mUpdatedNodeMutex.unlock();
		}
#if defined( LOG_NODE_UPDATES )
		else
		{
			qDebug() << "Node Already Processed:" << NODE->name();
		}
#endif

	}
}

void ContextWorker::processUpdatedPins( qint64 pTimeStamp )
{
	Q_ASSERT( isContextThread() );

	mUpdatePinMapMutex.lock();

	QList<UpdatePinEntry>		PinMap = mUpdatePinMap;

	mUpdatePinMap.clear();

	mUpdatePinMapMutex.unlock();

	for( UpdatePinEntry &PE : PinMap )
	{
		QSharedPointer<PinPrivate>		P = qSharedPointerCast<PinPrivate>( PE.first );

		P->update( pTimeStamp, PE.second );
	}
}

void ContextWorker::doFrameInitialise( qint64 pTimeStamp )
{
	emit frameInitialise();
	emit frameInitialise( pTimeStamp );
}

void ContextWorker::doFrameStart( qint64 pTimeStamp )
{
	mUpdatedNodeMutex.lock();

	mUpdatedNodeList << mFutureNodeList;

	mFutureNodeList.clear();

	mUpdatedNodeMutex.unlock();

	//-------------------------------------------------------------------------

	emit frameStart();
	emit frameStart( pTimeStamp );

	if( !mInitDeferNodeList.isEmpty() )
	{
		if( mNodeDeferProcess )
		{
			mNodeDeferProcess = false;

			processDeferredNodes();
		}

		if( mInitDeferNodeList.isEmpty() )
		{
			emit contextStart();
		}
	}

	mFutureSync.waitForFinished();
}

void ContextWorker::doFrameProcess( qint64 pTimeStamp )
{
	if( mPlayheadTimerEnabled )
	{
		processPlayhead( pTimeStamp );
	}

#if defined( LOG_NODE_UPDATES )
	if( !mUpdatedNodeList.empty() )
	{
		qDebug() << "----------------------------------------------------------------------------------------";
	}
#endif

	processUpdatedPins( pTimeStamp );

	emit frameProcess();
	emit frameProcess( pTimeStamp );

	bool	DoneFinalise = false;

	while( !mUpdatedNodeList.empty() || !DoneFinalise )
	{
		processUpdatedNodes( pTimeStamp );

		processUpdatedPins( pTimeStamp );

		if( mUpdatedNodeList.isEmpty() )
		{
			mFutureSync.waitForFinished();

			if( mUpdatedNodeList.isEmpty() && !DoneFinalise )
			{
				emit frameFinalise();
				emit frameFinalise( pTimeStamp );

				DoneFinalise = true;
			}
		}
	}
}

void ContextWorker::doFrameEnd( qint64 pTimeStamp )
{
	mFinishedNodeList.clear();

	mUpdatedNodeMutex.lock();

	mUpdatedNodeList.clear();

	mUpdatedNodeMutex.unlock();

	emit frameEnd();
	emit frameEnd( pTimeStamp );

	mFutureSync.waitForFinished();

	mLastTimeStamp = pTimeStamp;

#if 1
	while( !mPerfList.isEmpty() && pTimeStamp - mPerfList.first().mTimeEnd > 1000 )
	{
		mPerfList.removeFirst();
	}
#endif
}

void ContextWorker::registerInterface( const QUuid &pUuid, QObject *pInterface )
{
	mInterfaceMap.remove( pUuid );

	mInterfaceMap.insert( pUuid, pInterface );
}

void ContextWorker::unregisterInterface( const QUuid &pUuid )
{
	mInterfaceMap.remove( pUuid );
}

QObject *ContextWorker::findInterface( const QUuid &pUuid )
{
	return( mInterfaceMap.value( pUuid, nullptr ) );
}

//-------------------------------------------------------------------------
// External assets

QUuid ContextWorker::registerExternalAsset( const QString &pFileName )
{
	QUuid		AssIdx = mAssetMap.key( pFileName );

	if( !AssIdx.isNull() )
	{
		return( AssIdx );
	}

	AssIdx = QUuid::createUuid();

	mAssetMap.insert( AssIdx, pFileName );

	return( AssIdx );
}

void ContextWorker::unregisterExternalAsset(const QString &pFileName)
{
	QUuid		AssIdx = mAssetMap.key( pFileName );

	if( !AssIdx.isNull() )
	{
		mAssetMap.remove( AssIdx );
	}
}

void ContextWorker::unregisterExternalAsset(const QUuid &pUuid)
{
	mAssetMap.remove( pUuid );
}

QString ContextWorker::externalAsset(const QUuid &pUuid) const
{
	return( mAssetMap.value( pUuid ) );
}

QSharedPointer<fugio::NodeInterface> ContextWorker::createNode( const QString &pName, const QUuid &pGlobalId, const QUuid &pControlId, const QVariantHash &pSettings )
{
	Q_ASSERT( isContextThread() );

	NodePrivate	*NODE = new NodePrivate();

	if( !NODE )
	{
		return( QSharedPointer<fugio::NodeInterface>() );
	}

	NODE->setName( pName );
	NODE->setUuid( pGlobalId );
	NODE->setControlUuid( pControlId );
	NODE->setSettings( pSettings );
	NODE->setContext( this );

	QSharedPointer<fugio::NodeInterface>		NODE_PTR = QSharedPointer<fugio::NodeInterface>( NODE );

	fugio::ClassEntry	CE = global()->findNodeClassEntry( pControlId );

	if( CE.mMetaObject )
	{
		QObject		*ClassInstance = CE.mMetaObject->newInstance( Q_ARG( QSharedPointer<fugio::NodeInterface>, NODE_PTR ) );

		if( ClassInstance )
		{
			fugio::NodeControlInterface	*NodeControl = qobject_cast<fugio::NodeControlInterface *>( ClassInstance );

			if( NodeControl )
			{
				NODE->setControl( QSharedPointer<fugio::NodeControlInterface>( NodeControl ) );
			}
		}
	}
	else
	{
		qWarning() << "Unknown NodeControlInterface" << pControlId;
	}

	return( NODE_PTR );
}

QSharedPointer<fugio::PinInterface> ContextWorker::createPin( const QString &pName, const QUuid &pGlobalId, const QUuid &pLocalId, PinDirection pDirection, const QUuid &pControlUUID, const QVariantHash &pSettings )
{
	Q_ASSERT( isContextThread() );

	Q_ASSERT( !pLocalId.isNull() );

	PinPrivate						*P = new PinPrivate();

	if( !P )
	{
		return( QSharedPointer<fugio::PinInterface>() );
	}

	QSharedPointer<fugio::PinInterface>	PinPtr = QSharedPointer<fugio::PinInterface>( P );

	P->setGlobalId( pGlobalId );
	P->setLocalId( pLocalId );
	P->setName( pName );
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

QSharedPointer<fugio::PinControlInterface> ContextWorker::createPinControl( const QUuid &pUUID, QSharedPointer<fugio::PinInterface> pPin )
{
	Q_ASSERT( isContextThread() );

	const QMetaObject	*SMO = global()->findPinMetaObject( pUUID );

	if( !SMO )
	{
		qWarning() << "Unknown fugio::PinControlInterface" << pUUID << pPin->name();

		return( QSharedPointer<fugio::PinControlInterface>() );
	}

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
		qWarning() << "Can't create Pin ClassInstance of" << global()->findPinMetaObject( pUUID )->className();
	}

	return( QSharedPointer<fugio::PinControlInterface>() );
}

bool ContextWorker::updatePinControl( QSharedPointer<fugio::PinInterface> pPin, const QUuid &pPinControlUuid )
{
	Q_ASSERT( isContextThread() );

	QSharedPointer<fugio::PinControlInterface>		PinControl = createPinControl( pPinControlUuid, pPin );

	if( PinControl )
	{
		pPin->setControl( PinControl );

		return( true );
	}

	return( false );
}

ContextWorker::ContextWorker(ContextPrivate *pContextPrivate)
	: mContextPrivate( pContextPrivate ),
	  mActive( true ), mNodeDeferProcess( false ), mState( fugio::ContextInterface::Stopped ),
	  mPlayheadTimerEnabled( true ), mPlayheadLocalLast( 0 ), mDuration( 30.0 ), mLastTimeStamp( 0 ),
	mFramePending( false ), mMutex( QMutex::Recursive )
{
	if( mMetaNameMap.isEmpty() )
	{
		mMetaNameMap.insert( fugio::ContextInterface::Author, "author" );
		mMetaNameMap.insert( fugio::ContextInterface::Description, "description" );
		mMetaNameMap.insert( fugio::ContextInterface::Name, "name" );
		mMetaNameMap.insert( fugio::ContextInterface::Url, "url" );
		mMetaNameMap.insert( fugio::ContextInterface::Version, "version" );
		mMetaNameMap.insert( fugio::ContextInterface::Created, "created" );
	}

	mMetaInfoMap.insert( fugio::ContextInterface::Name, tr( "Untitled" ) );

	mMetaInfoMap.insert( fugio::ContextInterface::Created, QDateTime::currentDateTime().toString( Qt::RFC2822Date ) );

}

ContextWorker::~ContextWorker()
{
	clear();
}

bool ContextWorker::load( QString pFileName, bool pPartial )
{
	Q_ASSERT( isContextThread() );

	if( !QFile( pFileName ).exists() )
	{
		qWarning() << pFileName << "doesn't exist";

		return( false );
	}

	QSettings										 CFG( pFileName, QSettings::IniFormat );
	int												 VER = 1;
	bool											 RET = false;

	if( CFG.status() != QSettings::NoError )
	{
		qWarning() << pFileName << "can't load";

		return( false );
	}

	if( CFG.format() != QSettings::IniFormat )
	{
		qWarning() << pFileName << "bad format";

		return( false );
	}

	QMutexLocker	L( &mMutex );

	setActive( false );

	emit loadStart( CFG, pPartial );

	//	if( !CFG.childGroups().contains( "fugio" ) )
	//	{
	//		qWarning() << pFileName << "invalid file";

	//		return( false );
	//	}

	CFG.beginGroup( "fugio" );

	VER = CFG.value( "version", VER ).toInt();

	if( !pPartial )
	{
		qreal	NewDur = CFG.value( "duration", double( mDuration ) ).toDouble();

		setDuration( NewDur );
	}

	CFG.endGroup();

	if( !pPartial )
	{
		CFG.beginGroup( "meta" );

		for( auto it = mMetaNameMap.begin() ; it != mMetaNameMap.end() ; it++ )
		{
			const QString		V = CFG.value( it.value() ).toString();

			if( !V.isEmpty() )
			{
				mMetaInfoMap.insert( it.key(), V );
			}
		}

		CFG.endGroup();
	}

	if( VER == 2 )
	{
		RET = loadSettings( CFG, pPartial );
	}

	//-------------------------------------------------------------------------

	processDeferredNodes();

	//-------------------------------------------------------------------------

	if( !pPartial && mInitDeferNodeList.isEmpty() )
	{
		emit contextStart();
	}

	//-------------------------------------------------------------------------

	emit loading( CFG, pPartial );

	emit loadEnd( CFG, pPartial );

	setActive( true );

	return( RET );
}

void ContextWorker::processFrame( qint64 pTimeStamp )
{
	Q_ASSERT( isContextThread() );

	if( !active() )
	{
		return;
	}

	QMutexLocker	L( &mMutex );

	if( mFramePending )
	{
		return;
	}

	mFramePending = true;

	doFrameInitialise( pTimeStamp );
	doFrameStart( pTimeStamp );
	doFrameProcess( pTimeStamp );
	doFrameEnd( pTimeStamp );

	mFramePending = false;
}

void ContextPrivate::processFrame( qint64 pTimeStamp )
{
	QMetaObject::invokeMethod( mContextWorker, "processFrame", Qt::QueuedConnection, Q_ARG( qint64, pTimeStamp ) );
}
