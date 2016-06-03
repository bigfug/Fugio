#ifndef CONTEXTPRIVATE_H
#define CONTEXTPRIVATE_H

#include <QObject>
#include <QPointF>
#include <QList>
#include <QMap>
#include <QPair>
#include <QUuid>
#include <QSettings>
#include <QMutex>
#include <QHash>
#include <QMultiHash>
#include <QSharedPointer>
#include <QFutureSynchronizer>

#include <fugio/context_signals.h>
#include <fugio/global.h>
#include <fugio/context_interface.h>

FUGIO_NAMESPACE_BEGIN
class NodeInterface;
class PinControlInterface;
class PinInterface;
class PlayheadInterface;
FUGIO_NAMESPACE_END

class NodePrivate;

class FUGIOLIBSHARED_EXPORT ContextPrivate : public fugio::ContextSignals, public fugio::ContextInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::ContextInterface )

public:
	explicit ContextPrivate( fugio::GlobalInterface *pApp, QObject *parent = 0 );

	virtual ~ContextPrivate( void );

	void clear( void );

	//-------------------------------------------------------------------------
	// fugio::ContextInterface

	virtual fugio::ContextSignals *qobject( void )
	{
		return( this );
	}

	virtual fugio::GlobalInterface *global( void )
	{
		return( mApp );
	}

	virtual void setActive( bool pActive );

	virtual bool active( void ) const
	{
		return( mActive );
	}

	virtual void futureSync( const QFuture<void> &pFuture )
	{
		mFutureSync.addFuture( pFuture );
	}

	virtual QString metaInfo( MetaInfo pType ) const;

	virtual void setMetaInfo( MetaInfo pType, const QString &pMetaData );

	virtual bool load( const QString &pFileName, bool pPartial = false );

	virtual bool unload( const QString &pFileName );

	virtual bool save( const QString &pFileName, const QList<QUuid> *pNodeList = 0 ) const;

	virtual void registerInterface( const QUuid &pUuid, QObject *pInterface );

	virtual void unregisterInterface( const QUuid &pUuid );

	virtual QObject *findInterface( const QUuid &pUuid );

	virtual void doFrameInitialise( qint64 pTimeStamp );
	virtual void doFrameStart( qint64 pTimeStamp );
	virtual void doFrameProcess( qint64 pTimeStamp );
	virtual void doFrameEnd( qint64 pTimeStamp );

	virtual QSharedPointer<fugio::NodeInterface> createNode( const QString &pName, const QUuid &pUUID );

	virtual void updateNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual void registerNode( QSharedPointer<fugio::NodeInterface> pNode, const QUuid &pOrigId );
	virtual void unregisterNode( const QUuid &pUUID );
	virtual void renameNode( const QUuid &pUUID1, const QUuid &pUUID2 );
	virtual QSharedPointer<fugio::NodeInterface> findNode( const QUuid &pUUID );
	virtual QSharedPointer<fugio::NodeInterface> findNode( const QString &pName );

	virtual void registerPin( QSharedPointer<fugio::PinInterface> pPin );
	virtual void unregisterPin( const QUuid &pUUID );
	virtual void renamePin( const QUuid &pUUID1, const QUuid &pUUID2 );
	virtual QSharedPointer<fugio::PinInterface> findPin( const QUuid &pUUID );

	virtual QList< QSharedPointer<fugio::PinInterface> > connections( const QUuid &pUUID );

	virtual bool isConnected( const QUuid &pUUID );
	virtual bool isConnectedTo( const QUuid &pUUID1, const QUuid &pUUID2 );

	virtual void connectPins( const QUuid &pUUID1, const QUuid &pUUID2 );
	virtual void disconnectPins( const QUuid &pUUID1, const QUuid &pUUID2 );
	virtual void disconnectPin( const QUuid &pUUID );

	virtual QList<QUuid> nodeList( void );

	virtual void nodeInitialised( void );

	virtual void pinUpdated( QSharedPointer<fugio::PinInterface> pPin, bool pUpdatedConnectedNode = true );

	virtual void performance( QSharedPointer<fugio::NodeInterface> pNode, const QString &pName, qint64 pTimeStart, qint64 pTimeEnd );

	virtual QList<fugio::PerfData> perfdata( void );

	virtual void addDeferredNode( QSharedPointer<fugio::NodeInterface> pNode )
	{
		mInitDeferNodeList.append( pNode );
	}

	//-------------------------------------------------------------------------
	// Context Playhead

	virtual void registerPlayhead( fugio::PlayheadInterface *pPlayHead );
	virtual void unregisterPlayhead( fugio::PlayheadInterface *pPlayHead );

	virtual bool isPlaying( void ) const;

	virtual void setPlayheadTimerEnabled( bool pEnabled )
	{
		mPlayheadTimerEnabled = pEnabled;
	}

	virtual void playheadMove( qreal pTimeStamp );
	virtual void playheadPlay( qreal pTimePrev, qreal pTimeCurr );

	virtual void setPlayheadPosition( qreal pTimeStamp, bool pRefresh = true );

	virtual TimeState state( void ) const
	{
		return( mState );
	}

	virtual qreal duration( void ) const
	{
		return( mDuration );
	}

	virtual qreal position( void ) const
	{
		return( qMax( mPlayheadLocalLast, 0.0 ) );
	}

	virtual void notifyAboutToPlay( void );

	virtual void setDuration( qreal pDuration );

	//-------------------------------------------------------------------------
	// External assets

	virtual QUuid registerExternalAsset( const QString &pFileName );

	virtual void unregisterExternalAsset( const QString &pFileName );
	virtual void unregisterExternalAsset( const QUuid &pUuid );

	virtual QString externalAsset( const QUuid &pUuid ) const;

public slots:
	//void processContext( qint64 pTimeStamp );

	void onNodeActivationChanged( QSharedPointer<fugio::NodeInterface> pNode );
	void nodeControlChanged( QSharedPointer<fugio::NodeInterface> pNode );
	void nodeNameChanged( QSharedPointer<fugio::NodeInterface> pNode );

	void onPinAdded( QSharedPointer<fugio::NodeInterface> pNode, QSharedPointer<fugio::PinInterface> pPin );
	void onPinRemoved( QSharedPointer<fugio::NodeInterface> pNode, QSharedPointer<fugio::PinInterface> pPin );

	virtual void play();
	virtual void stop();
	virtual void pause();
	virtual void resume();

	void playFrom( qreal pTimeStamp );

protected:
	void processDeferredNodes( QList< QSharedPointer<fugio::NodeInterface> > WaitingNodes, QList< QSharedPointer<fugio::NodeInterface> > &InitialisedNodes );
        
private:
	void processUpdatedNodes( qint64 pTimeStamp );

	void processUpdatedPins( qint64 pTimeStamp );

	void processPlayhead( qint64 pTimeStamp );

	static void processNode( QSharedPointer<fugio::NodeInterface> pNode, qint64 pTimeStamp );

	//static void processContext( QSharedPointer<fugio::ContextInterface> pContext );

	bool loadSettings1( QSettings &pSettings, QList< QSharedPointer<fugio::NodeInterface> > &pNewNodeList, bool pPartial );
	bool loadSettings2( QSettings &pSettings, QList< QSharedPointer<fugio::NodeInterface> > &pNewNodeList, bool pPartial );

	void loadNodeSettings( QSettings &pSettings, QVariantHash &pVarHsh, QStringList &pVarBse ) const;

private slots:
	virtual void processDeferredNodes( void );

private:
	typedef QPair<QString, QString>							ConnectionPair;
	typedef	QMap<ConnectionPair, ConnectionPair>			ConnectionMap;
	typedef	QMap<ConnectionPair, QString>					DefaultMap;
	typedef QMap<QUuid, QObject *>							UuidObjectMap;

	typedef	QHash<QUuid,QSharedPointer<fugio::NodeInterface> >		NodeHash;
	typedef	QHash<QUuid,QSharedPointer<fugio::PinInterface> >		PinHash;

	typedef QList<fugio::PlayheadInterface *>						PlayheadList;

	bool												 mActive;

	ConnectionMap										 mConnectionMap;

	QMutex												 mUpdatedNodeMutex;
	QList< QSharedPointer<fugio::NodeInterface> >		 mUpdatedNodeList;
	QList< QSharedPointer<fugio::NodeInterface> >		 mFinishedNodeList;
	QList< QSharedPointer<fugio::NodeInterface> >		 mFutureNodeList;

	QList< QSharedPointer<fugio::NodeInterface> >		 mInitDeferNodeList;

	bool												 mNodeDeferProcess;

	NodeHash											 mNodeHash;
	PinHash												 mPinHash;

	QHash<QUuid,QUuid>									 mConnectIO;
	QMultiHash<QUuid,QUuid>								 mConnectOI;

	UuidObjectMap										 mInterfaceMap;

	fugio::GlobalInterface								*mApp;

	fugio::ContextInterface::TimeState					 mState;

	bool												 mPlayheadTimerEnabled;

	PlayheadList										 mPlayheadList;
	qreal												 mPlayheadGlobalOffset;
	qreal												 mPlayheadLocalOffset;
	qreal												 mPlayheadLocalLast;
	qreal												 mPlayheadLatencyOffset;

	qreal												 mDuration;

	QFutureSynchronizer<void>							 mFutureSync;

	typedef QMap< QSharedPointer<fugio::PinInterface>, bool >		UpdatePinMap;

	UpdatePinMap										 mUpdatePinMap;

	static QMap<fugio::ContextInterface::MetaInfo,QString>	 mMetaNameMap;

	QMap<fugio::ContextInterface::MetaInfo,QString>		 mMetaInfoMap;

	typedef struct PerfEntry
	{
		QUuid							mUuid;
		QString							mNode;
		QString							mName;
		qint64							mTimeStart;
		qint64							mTimeEnd;
	} PerfEntry;

	QList<PerfEntry>									 mPerfList;

	QMap<QUuid,QString>									 mAssetMap;

	QStringList											 mLastDeferredNodeNames;

	qint64												 mLastTimeStamp;
};

#endif // CONTEXTPRIVATE_H
