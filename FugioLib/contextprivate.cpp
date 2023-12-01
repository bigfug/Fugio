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

#include "nodeprivate.h"
#include "pinprivate.h"

#include <fugio/utils.h>

//using namespace fugio;

//#define LOG_NODE_UPDATES
//#define DEBUG_CONNECTIONS

QMap<fugio::ContextInterface::MetaInfo,QString>	 ContextPrivate::mMetaNameMap;

ContextPrivate::ContextPrivate( fugio::GlobalInterface *pApp, QObject *pParent ) :
	fugio::ContextSignals( pParent ), mActive( true ), mNodeDeferProcess( false ), mApp( pApp ), mState( fugio::ContextInterface::Stopped ),
	mPlayheadTimerEnabled( true ), mPlayheadLocalLast( 0 ), mDuration( 30.0 ), mLastTimeStamp( 0 )
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

ContextPrivate::~ContextPrivate( void )
{
	clear();

	//qDebug() << "~ContextPrivate";
}

bool ContextPrivate::load( const QString &pFileName, bool pPartial )
{
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

	mActive = false;

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

	mActive = true;

	return( RET );
}

bool ContextPrivate::unload( const QString &pFileName )
{
	if( !QFile( pFileName ).exists() )
	{
		qWarning() << pFileName << "doesn't exist";

		return( false );
	}

	QSettings										 CFG( pFileName, QSettings::IniFormat );
//	int												 VER = 1;
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

	mActive = true;

	return( true );
}

void ContextPrivate::loadNodeSettings( QSettings &pSettings, QVariantHash &pVarHsh, QStringList &pVarBse ) const
{
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

bool ContextPrivate::loadSettings( QSettings &pSettings, bool pPartial )
{
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

		if( SrcP == 0 )
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

		if( DstP == 0 )
		{
			continue;
		}

		connectPins( SrcP->globalId(), DstP->globalId() );
	}

	pSettings.endGroup();

	return( true );
}

bool ContextPrivate::save( const QString &pFileName, const QList<QUuid> *pNodeList ) const
{
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
			if( !pNodeList || pNodeList->contains( N->uuid() ) )
			{
				CFG.setValue( fugio::utils::uuid2string( N->uuid() ), fugio::utils::uuid2string( N->controlUuid() ) );
			}
		}

		CFG.endGroup();

		//-------------------------------------------------------------------------

		CFG.beginGroup( "connections" );

		for( QSharedPointer<fugio::NodeInterface> N : mNodeHash.values() )
		{
			if( !pNodeList || pNodeList->contains( N->uuid() ) )
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

					if( !pNodeList || pNodeList->contains( ConPin->node()->uuid() ) )
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
			if( !pNodeList || pNodeList->contains( N->uuid() ) )
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

bool ContextPrivate::loadData( const QString &pFileName )
{
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

bool ContextPrivate::saveData( const QString &pFileName ) const
{
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

void ContextPrivate::clear( void )
{
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

void ContextPrivate::setActive(bool pActive)
{
	if( mActive == pActive )
	{
		return;
	}

	mActive = pActive;

	emit activeStateChanged( mActive );
}

QString ContextPrivate::metaInfo( fugio::ContextInterface::MetaInfo pType) const
{
	return( mMetaInfoMap.value( pType ) );
}

void ContextPrivate::setMetaInfo(fugio::ContextInterface::MetaInfo pType, const QString &pMetaData)
{
	if( mMetaInfoMap.value( pType ) == pMetaData )
	{
		return;
	}

	mMetaInfoMap.insert( pType, pMetaData );

	emit metaInfoChanged( pType, pMetaData );
}

void ContextPrivate::registerPlayhead( fugio::PlayheadInterface *pPlayHead )
{
	mPlayheadList.removeAll( pPlayHead );

	mPlayheadList.append( pPlayHead );
}

void ContextPrivate::unregisterPlayhead( fugio::PlayheadInterface *pPlayHead )
{
	mPlayheadList.removeAll( pPlayHead );
}

bool ContextPrivate::isPlaying( void ) const
{
	return( mState == fugio::ContextInterface::Playing );
}

void ContextPrivate::registerNode( QSharedPointer<fugio::NodeInterface> pNode )
{
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

	connect( N, SIGNAL(controlChanged(QSharedPointer<fugio::NodeInterface>)), this, SLOT(nodeControlChanged(QSharedPointer<fugio::NodeInterface>)) );
	connect( N, SIGNAL(nameChanged(QSharedPointer<fugio::NodeInterface>)), this, SLOT(nodeNameChanged(QSharedPointer<fugio::NodeInterface>)) );
	connect( N, SIGNAL(activationChanged(QSharedPointer<fugio::NodeInterface>)), this, SLOT(onNodeActivationChanged(QSharedPointer<fugio::NodeInterface>)) );

	connect( N, SIGNAL(pinAdded(QSharedPointer<fugio::NodeInterface>,QSharedPointer<fugio::PinInterface>)), this, SLOT(onPinAdded(QSharedPointer<fugio::NodeInterface>,QSharedPointer<fugio::PinInterface>)) );
	connect( N, SIGNAL(pinRemoved(QSharedPointer<fugio::NodeInterface>,QSharedPointer<fugio::PinInterface>)), this, SLOT(onPinRemoved(QSharedPointer<fugio::NodeInterface>,QSharedPointer<fugio::PinInterface>)) );

	addDeferredNode( pNode );

	mNodeDeferProcess = true;

	emit nodeAdded( pNode->uuid() );
}

void ContextPrivate::unregisterNode( const QUuid &pUUID )
{
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

void ContextPrivate::renameNode( const QUuid &pUUID1, const QUuid &pUUID2 )
{
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

QSharedPointer<fugio::NodeInterface> ContextPrivate::findNode( const QUuid &pUUID ) const
{
	NodeHash::const_iterator it = mNodeHash.find( pUUID );

	if( it == mNodeHash.end() )
	{
		return( QSharedPointer<fugio::NodeInterface>() );
	}

	return( it.value() );
}

QSharedPointer<fugio::NodeInterface> ContextPrivate::findNode( const QString &pName ) const
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

void ContextPrivate::registerPin( QSharedPointer<fugio::PinInterface> pPin )
{
	if( !mPinHash.contains( pPin->globalId() ) )
	{
		mPinHash.insert( pPin->globalId(), pPin );
	}
}

void ContextPrivate::unregisterPin( const QUuid &pUUID )
{
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

void ContextPrivate::renamePin( const QUuid &pUUID1, const QUuid &pUUID2 )
{
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

QSharedPointer<fugio::PinInterface> ContextPrivate::findPin( const QUuid &pUUID )
{
	PinHash::iterator it = mPinHash.find( pUUID );

	if( it == mPinHash.end() )
	{
		return( QSharedPointer<fugio::PinInterface>() );
	}

	return( it.value() );
}

QList< QSharedPointer<fugio::PinInterface> > ContextPrivate::connections( const QUuid &pUUID )
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

bool ContextPrivate::isConnected( const QUuid &pUUID )
{
	return( mConnectIO.contains( pUUID ) || mConnectOI.contains( pUUID ) );
}

bool ContextPrivate::isConnectedTo( const QUuid &pUUID1, const QUuid &pUUID2 )
{
	return( mConnectIO.value( pUUID1 ) == pUUID2 || mConnectIO.value( pUUID2 ) == pUUID1 || mConnectOI.contains( pUUID1, pUUID2 ) || mConnectOI.contains( pUUID2, pUUID1 ) );
}

void ContextPrivate::connectPins( const QUuid &pUUID1, const QUuid &pUUID2 )
{
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

void ContextPrivate::disconnectPins( const QUuid &pUUID1, const QUuid &pUUID2 )
{
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

void ContextPrivate::disconnectPin( const QUuid &pUUID )
{
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

QList<QUuid> ContextPrivate::nodeList() const
{
	return( mNodeHash.keys() );
}

void ContextPrivate::onPinAdded( QSharedPointer<fugio::NodeInterface> pNode, QSharedPointer<fugio::PinInterface> pPin )
{
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

				if( Pin == 0 || Pin->direction() != PIN_INPUT )
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

				if( Pin == 0 || Pin->direction() != PIN_OUTPUT )
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

void ContextPrivate::onPinRemoved( QSharedPointer<fugio::NodeInterface> pNode, QSharedPointer<fugio::PinInterface> pPin )
{
	Q_UNUSED( pNode )
	Q_UNUSED( pPin )

	unregisterPin( pPin->globalId() );

	emit pinRemoved( pNode->uuid(), pPin->globalId() );
}

void ContextPrivate::nodeControlChanged( QSharedPointer<fugio::NodeInterface> pNode )
{
	emit nodeUpdated( pNode->uuid() );
}

void ContextPrivate::nodeNameChanged( QSharedPointer<fugio::NodeInterface> pNode )
{
	emit nodeUpdated( pNode->uuid() );
}

void ContextPrivate::onNodeActivationChanged( QSharedPointer<fugio::NodeInterface> pNode )
{
	emit nodeActivated( pNode->uuid() );
}

void ContextPrivate::processPlayhead( qint64 pTimeStamp )
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

void ContextPrivate::notifyAboutToPlay()
{
	for( fugio::PlayheadInterface *PH : mPlayheadList )
	{
		PH->playStart( mPlayheadLocalLast );
	}

	emit aboutToPlay();
}

void ContextPrivate::setDuration(qreal pDuration)
{
	if( pDuration == mDuration )
	{
		return;
	}

	mDuration = pDuration;

	emit durationChanged( mDuration );
}

void ContextPrivate::play( void )
{
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

void ContextPrivate::playFrom( qreal pTimeStamp )
{
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

void ContextPrivate::stop()
{
	if( mState == fugio::ContextInterface::Stopped )
	{
		return;
	}

	mState = fugio::ContextInterface::Stopped;

	emit stateChanged( mState );
}

void ContextPrivate::pause()
{
	if( mState == fugio::ContextInterface::Paused )
	{
		return;
	}

	mState = fugio::ContextInterface::Paused;

	emit stateChanged( mState );

	//mComposition->trackdata()->pause();
}

void ContextPrivate::resume()
{
	if( mState != fugio::ContextInterface::Paused )
	{
		return;
	}

	mState = fugio::ContextInterface::Playing;

	emit stateChanged( mState );

	//mComposition->trackdata()->resume();
}

void ContextPrivate::setPlayheadPosition( qreal pTimeStamp, bool pRefresh )
{
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

void ContextPrivate::playheadMove( qreal pTimeStamp )
{
	for( fugio::PlayheadInterface *PH : mPlayheadList )
	{
		PH->playheadMove( pTimeStamp );
	}
}

void ContextPrivate::playheadPlay( qreal pTimePrev, qreal pTimeCurr )
{
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

void ContextPrivate::nodeInitialised( void )
{
	mNodeDeferProcess = true;
}

void ContextPrivate::pinUpdated( QSharedPointer<fugio::PinInterface> pPin, qint64 pGlobalTimestamp, bool pUpdatedConnectedNode )
{
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

void ContextPrivate::performance( QSharedPointer<fugio::NodeInterface> pNode, const QString &pName, qint64 pTimeStart, qint64 pTimeEnd )
{
	PerfEntry		PE;

	PE.mUuid      = pNode ? pNode->uuid() : QUuid();
	PE.mNode      = pNode ? pNode->name() : QString();
	PE.mName      = pName;
	PE.mTimeStart = pTimeStart;
	PE.mTimeEnd   = pTimeEnd;

	mPerfList << PE;
}

QList<fugio::PerfData> ContextPrivate::perfdata()
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

void ContextPrivate::processDeferredNodes( QList< QSharedPointer<fugio::NodeInterface> > WaitingNodes, QList< QSharedPointer<fugio::NodeInterface> > &InitialisedNodes )
{
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

void ContextPrivate::processDeferredNodes()
{
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

void ContextPrivate::updateNode( QSharedPointer<fugio::NodeInterface> pNode )
{
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

void ContextPrivate::processNode( QSharedPointer<fugio::NodeInterface> pNode, qint64 pTimeStamp )
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

//void ContextPrivate::processContext( QSharedPointer<fugio::ContextInterface> pContext )
//{
//	Q_UNUSED( pContext )
//}

void ContextPrivate::processUpdatedNodes( qint64 pTimeStamp )
{
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

void ContextPrivate::processUpdatedPins( qint64 pTimeStamp )
{
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

void ContextPrivate::doFrameInitialise( qint64 pTimeStamp )
{
	emit frameInitialise();
	emit frameInitialise( pTimeStamp );
}

void ContextPrivate::doFrameStart( qint64 pTimeStamp )
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

void ContextPrivate::doFrameProcess( qint64 pTimeStamp )
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

void ContextPrivate::doFrameEnd( qint64 pTimeStamp )
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

void ContextPrivate::registerInterface( const QUuid &pUuid, QObject *pInterface )
{
	mInterfaceMap.remove( pUuid );

	mInterfaceMap.insert( pUuid, pInterface );
}

void ContextPrivate::unregisterInterface( const QUuid &pUuid )
{
	mInterfaceMap.remove( pUuid );
}

QObject *ContextPrivate::findInterface( const QUuid &pUuid )
{
	return( mInterfaceMap.value( pUuid, nullptr ) );
}

//-------------------------------------------------------------------------
// External assets

QUuid ContextPrivate::registerExternalAsset( const QString &pFileName )
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

void ContextPrivate::unregisterExternalAsset(const QString &pFileName)
{
	QUuid		AssIdx = mAssetMap.key( pFileName );

	if( !AssIdx.isNull() )
	{
		mAssetMap.remove( AssIdx );
	}
}

void ContextPrivate::unregisterExternalAsset(const QUuid &pUuid)
{
	mAssetMap.remove( pUuid );
}

QString ContextPrivate::externalAsset(const QUuid &pUuid) const
{
	return( mAssetMap.value( pUuid ) );
}

QSharedPointer<fugio::NodeInterface> ContextPrivate::createNode( const QString &pName, const QUuid &pGlobalId, const QUuid &pControlId, const QVariantHash &pSettings )
{
	NodePrivate	*NODE = new NodePrivate();

	if( !NODE )
	{
		return( QSharedPointer<fugio::NodeInterface>() );
	}

	NODE->moveToThread( thread() );

	NODE->setName( pName );
	NODE->setUuid( pGlobalId );
	NODE->setControlUuid( pControlId );
	NODE->setSettings( pSettings );
	NODE->setContext( this );

	QSharedPointer<fugio::NodeInterface>		NODE_PTR = QSharedPointer<fugio::NodeInterface>( NODE );

	fugio::ClassEntry	CE = mApp->findNodeClassEntry( pControlId );

	if( CE.mMetaObject )
	{
		QObject		*ClassInstance = CE.mMetaObject->newInstance( Q_ARG( QSharedPointer<fugio::NodeInterface>, NODE_PTR ) );

		if( ClassInstance )
		{
			ClassInstance->moveToThread( thread() );

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

QSharedPointer<fugio::PinInterface> ContextPrivate::createPin( const QString &pName, const QUuid &pGlobalId, const QUuid &pLocalId, PinDirection pDirection, const QUuid &pControlUUID, const QVariantHash &pSettings )
{
	Q_ASSERT( !pLocalId.isNull() );

	PinPrivate						*P = new PinPrivate();

	if( !P )
	{
		return( QSharedPointer<fugio::PinInterface>() );
	}

	QSharedPointer<fugio::PinInterface>	PinPtr = QSharedPointer<fugio::PinInterface>( P );

	P->moveToThread( thread() );

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

QSharedPointer<fugio::PinControlInterface> ContextPrivate::createPinControl( const QUuid &pUUID, QSharedPointer<fugio::PinInterface> pPin )
{
	const QMetaObject	*SMO = mApp->findPinMetaObject( pUUID );

	if( !SMO )
	{
		qWarning() << "Unknown fugio::PinControlInterface" << pUUID << pPin->name();

		return( QSharedPointer<fugio::PinControlInterface>() );
	}

	QObject		*ClassInstance = SMO->newInstance( Q_ARG( QSharedPointer<fugio::PinInterface>, pPin ) );

	if( ClassInstance )
	{
		ClassInstance->moveToThread( thread() );

		fugio::PinControlInterface		*PinControl = qobject_cast<fugio::PinControlInterface *>( ClassInstance );

		if( PinControl )
		{
			return( QSharedPointer<fugio::PinControlInterface>( PinControl ) );
		}

		delete ClassInstance;
	}
	else
	{
		qWarning() << "Can't create Pin ClassInstance of" << mApp->findPinMetaObject( pUUID )->className();
	}

	return( QSharedPointer<fugio::PinControlInterface>() );
}

bool ContextPrivate::updatePinControl( QSharedPointer<fugio::PinInterface> pPin, const QUuid &pPinControlUuid )
{
	QSharedPointer<fugio::PinControlInterface>		PinControl = createPinControl( pPinControlUuid, pPin );

	if( PinControl )
	{
		pPin->setControl( PinControl );

		return( true );
	}

	return( false );
}

