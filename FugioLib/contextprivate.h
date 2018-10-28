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
#include <QThread>

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
class ContextPrivate;

class FUGIOLIBSHARED_EXPORT ContextWorker : public fugio::ContextSignals, public fugio::ContextInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::ContextInterface )

	Q_PROPERTY( bool active READ active WRITE setActive NOTIFY activeChanged )

public:
	ContextWorker( ContextPrivate *pContextPrivate );

	virtual ~ContextWorker( void ) Q_DECL_OVERRIDE;

	void clear( void );

	ContextPrivate *context( void )
	{
		return( mContextPrivate );
	}

	//-------------------------------------------------------------------------
	// fugio::ContextInterface

	virtual fugio::ContextSignals *qobject( void ) Q_DECL_OVERRIDE;

	virtual fugio::GlobalInterface *global( void ) Q_DECL_OVERRIDE;

	virtual void futureSync( const QFuture<void> &pFuture ) Q_DECL_OVERRIDE
	{
		mFutureSync.addFuture( pFuture );
	}

	virtual QString metaInfo( MetaInfo pType ) const Q_DECL_OVERRIDE;

	virtual void setMetaInfo( MetaInfo pType, const QString &pMetaData ) Q_DECL_OVERRIDE;

	Q_INVOKABLE virtual bool unload( QString pFileName ) Q_DECL_OVERRIDE;

	Q_INVOKABLE virtual bool save( QString pFileName, QList<QUuid> pNodeList = QList<QUuid>() ) const Q_DECL_OVERRIDE;

	virtual bool loadData( QString pFileName ) Q_DECL_OVERRIDE;

	virtual bool saveData( QString pFileName ) const Q_DECL_OVERRIDE;

	virtual void registerInterface( const QUuid &pUuid, QObject *pInterface ) Q_DECL_OVERRIDE;

	virtual void unregisterInterface( const QUuid &pUuid ) Q_DECL_OVERRIDE;

	virtual QObject *findInterface( const QUuid &pUuid ) Q_DECL_OVERRIDE;

	Q_INVOKABLE virtual void updateNode( QSharedPointer<fugio::NodeInterface> pNode ) Q_DECL_OVERRIDE;

	Q_INVOKABLE virtual QSharedPointer<fugio::NodeInterface> createNode( const QString &pName, const QUuid &pGlobalId, const QUuid &pControlId, const QVariantHash &pSettings ) Q_DECL_OVERRIDE;

	Q_INVOKABLE virtual QSharedPointer<fugio::PinInterface> createPin( const QString &pName, const QUuid &pGlobalId, const QUuid &pLocalId, PinDirection pDirection, const QUuid &pControlUUID, const QVariantHash &pSettings ) Q_DECL_OVERRIDE;

	Q_INVOKABLE virtual QSharedPointer<fugio::PinControlInterface> createPinControl( const QUuid &pUUID, QSharedPointer<fugio::PinInterface> pPin ) Q_DECL_OVERRIDE;

	Q_INVOKABLE virtual bool updatePinControl( QSharedPointer<fugio::PinInterface> pPin, const QUuid &pPinControlUuid ) Q_DECL_OVERRIDE;

	Q_INVOKABLE virtual void registerNode( QSharedPointer<fugio::NodeInterface> pNode ) Q_DECL_OVERRIDE;
	Q_INVOKABLE virtual void unregisterNode( const QUuid &pUUID ) Q_DECL_OVERRIDE;
	Q_INVOKABLE virtual void renameNode( const QUuid &pUUID1, const QUuid &pUUID2 ) Q_DECL_OVERRIDE;
	virtual QSharedPointer<fugio::NodeInterface> findNode( const QUuid &pUUID ) const Q_DECL_OVERRIDE;
	virtual QSharedPointer<fugio::NodeInterface> findNode( const QString &pName ) const Q_DECL_OVERRIDE;

	Q_INVOKABLE virtual void registerPin( QSharedPointer<fugio::PinInterface> pPin ) Q_DECL_OVERRIDE;
	Q_INVOKABLE virtual void unregisterPin( const QUuid &pUUID ) Q_DECL_OVERRIDE;
	Q_INVOKABLE virtual void renamePin( const QUuid &pUUID1, const QUuid &pUUID2 ) Q_DECL_OVERRIDE;
	virtual QSharedPointer<fugio::PinInterface> findPin( const QUuid &pUUID ) Q_DECL_OVERRIDE;

	virtual QList< QSharedPointer<fugio::PinInterface> > connections( const QUuid &pUUID ) Q_DECL_OVERRIDE;

	virtual bool isConnected( const QUuid &pUUID ) Q_DECL_OVERRIDE;
	virtual bool isConnectedTo( const QUuid &pUUID1, const QUuid &pUUID2 ) Q_DECL_OVERRIDE;

	Q_INVOKABLE virtual void connectPins( const QUuid &pUUID1, const QUuid &pUUID2 ) Q_DECL_OVERRIDE;
	Q_INVOKABLE virtual void disconnectPins( const QUuid &pUUID1, const QUuid &pUUID2 ) Q_DECL_OVERRIDE;
	Q_INVOKABLE virtual void disconnectPin( const QUuid &pUUID ) Q_DECL_OVERRIDE;

	virtual QList<QUuid> nodeList( void ) const Q_DECL_OVERRIDE;

	virtual void nodeInitialised( void ) Q_DECL_OVERRIDE;

	Q_INVOKABLE virtual void pinUpdated( QSharedPointer<fugio::PinInterface> pPin, qint64 pGlobalTimestamp = -1, bool pUpdatedConnectedNode = true ) Q_DECL_OVERRIDE;

	virtual void performance( QSharedPointer<fugio::NodeInterface> pNode, const QString &pName, qint64 pTimeStart, qint64 pTimeEnd ) Q_DECL_OVERRIDE;

	virtual QList<fugio::PerfData> perfdata( void ) Q_DECL_OVERRIDE;

	virtual void addDeferredNode( QSharedPointer<fugio::NodeInterface> pNode )
	{
		mInitDeferNodeList.append( pNode );
	}

	//-------------------------------------------------------------------------
	// Context Playhead

	virtual void registerPlayhead( fugio::PlayheadInterface *pPlayHead ) Q_DECL_OVERRIDE;
	virtual void unregisterPlayhead( fugio::PlayheadInterface *pPlayHead ) Q_DECL_OVERRIDE;

	virtual bool isPlaying( void ) const Q_DECL_OVERRIDE;

	virtual void setPlayheadTimerEnabled( bool pEnabled ) Q_DECL_OVERRIDE
	{
		mPlayheadTimerEnabled = pEnabled;
	}

	virtual void playheadMove( qreal pTimeStamp ) Q_DECL_OVERRIDE;
	virtual void playheadPlay( qreal pTimePrev, qreal pTimeCurr ) Q_DECL_OVERRIDE;

	virtual void setPlayheadPosition( qreal pTimeStamp, bool pRefresh = true ) Q_DECL_OVERRIDE;

	virtual TimeState state( void ) const Q_DECL_OVERRIDE
	{
		return( mState );
	}

	virtual qreal duration( void ) const Q_DECL_OVERRIDE
	{
		return( mDuration );
	}

	virtual qreal position( void ) const Q_DECL_OVERRIDE
	{
		return( qMax( mPlayheadLocalLast, 0.0 ) );
	}

	virtual void notifyAboutToPlay( void ) Q_DECL_OVERRIDE;

	virtual void setDuration( qreal pDuration ) Q_DECL_OVERRIDE;

	//-------------------------------------------------------------------------
	// External assets

	virtual QUuid registerExternalAsset( const QString &pFileName ) Q_DECL_OVERRIDE;

	virtual void unregisterExternalAsset( const QString &pFileName ) Q_DECL_OVERRIDE;
	virtual void unregisterExternalAsset( const QUuid &pUuid ) Q_DECL_OVERRIDE;

	virtual QString externalAsset( const QUuid &pUuid ) const Q_DECL_OVERRIDE;

public slots:
	//void processContext( qint64 pTimeStamp );

	void nodeActivationUpdated( QSharedPointer<fugio::NodeInterface> pNode );
	void nodeControUpdated( QSharedPointer<fugio::NodeInterface> pNode );
	void nodeNameUpdated( QSharedPointer<fugio::NodeInterface> pNode );

	void onPinAdded( QSharedPointer<fugio::NodeInterface> pNode, QSharedPointer<fugio::PinInterface> pPin );
	void onPinRemoved( QSharedPointer<fugio::NodeInterface> pNode, QSharedPointer<fugio::PinInterface> pPin );

	virtual void play() Q_DECL_OVERRIDE;
	virtual void stop() Q_DECL_OVERRIDE;
	virtual void pause() Q_DECL_OVERRIDE;
	virtual void resume() Q_DECL_OVERRIDE;

	void playFrom( qreal pTimeStamp );

protected:
	void processDeferredNodes( QList< QSharedPointer<fugio::NodeInterface> > WaitingNodes, QList< QSharedPointer<fugio::NodeInterface> > &InitialisedNodes );

	bool isContextThread() const;

private:
	void processUpdatedNodes( qint64 pTimeStamp );

	void processUpdatedPins( qint64 pTimeStamp );

	void processPlayhead( qint64 pTimeStamp );

	static void processNode( QSharedPointer<fugio::NodeInterface> pNode, qint64 pTimeStamp );

//	bool loadSettings( QSettings &pSettings, bool pPartial );

//	void loadNodeSettings( QSettings &pSettings, QVariantHash &pVarHsh, QStringList &pVarBse ) const;

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

//	bool												 mActive;

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

	fugio::ContextInterface::TimeState					 mState;

	bool												 mPlayheadTimerEnabled;

	PlayheadList										 mPlayheadList;
	qreal												 mPlayheadGlobalOffset;
	qreal												 mPlayheadLocalOffset;
	qreal												 mPlayheadLocalLast;
	qreal												 mPlayheadLatencyOffset;

	qreal												 mDuration;

	QFutureSynchronizer<void>							 mFutureSync;

	typedef QPair< QWeakPointer<fugio::PinInterface>, bool >		UpdatePinEntry;

	QMutex												 mUpdatePinMapMutex;
	QList<UpdatePinEntry>								 mUpdatePinMap;

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

public:
	virtual bool active() const Q_DECL_OVERRIDE
	{
		return( mActive );
	}

public slots:
	void update( void )
	{
//		mGlobalPrivate->executeFrame();
	}

	Q_INVOKABLE virtual bool load( QString pFileName, bool pPartial ) Q_DECL_OVERRIDE;

	virtual void setActive( bool pActive ) Q_DECL_OVERRIDE
	{
		if( mActive == pActive )
		{
			return;
		}

		mActive = pActive;

		emit activeChanged();
	}

	Q_INVOKABLE void processFrame( qint64 pTimeStamp );

protected:
	void loadNodeSettings( QSettings &pSettings, QVariantHash &pVarHsh, QStringList &pVarBse ) const;

	bool loadSettings( QSettings &pSettings, bool pPartial );

	void doFrameInitialise( qint64 pTimeStamp );
	void doFrameStart( qint64 pTimeStamp );
	void doFrameProcess( qint64 pTimeStamp );
	void doFrameEnd( qint64 pTimeStamp );

private:
	ContextPrivate	*mContextPrivate;
	QMutex			 mMutex;
	bool			 mActive;
	bool			 mFramePending;
};

class FUGIOLIBSHARED_EXPORT ContextPrivate : public QThread, public fugio::ContextInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::ContextInterface )

public:
	explicit ContextPrivate( fugio::GlobalInterface *pApp, QObject *parent = Q_NULLPTR );

	virtual ~ContextPrivate( void ) Q_DECL_OVERRIDE;

	bool isContextThread( void ) const;

	//-------------------------------------------------------------------------
	// fugio::ContextInterface

	virtual fugio::GlobalInterface *global( void ) Q_DECL_OVERRIDE
	{
		return( mApp );
	}

	virtual fugio::ContextSignals *qobject( void ) Q_DECL_OVERRIDE
	{
		return( mContextWorker );
	}

	// Inherited via ContextInterface
	virtual bool unload( QString pFileName ) override;
	virtual bool loadData( QString pFileName ) override;
	virtual bool saveData( QString pFileName ) const override;
	virtual void setActive( bool pActive ) override;
	virtual bool active( void ) const override;
	virtual void futureSync( const QFuture<void>& pFuture ) override;
	virtual QString metaInfo( MetaInfo pType ) const override;
	virtual void setMetaInfo( MetaInfo pType, const QString & pMetaData ) override;
	virtual void registerInterface( const QUuid & pUuid, QObject * pInterface ) override;
	virtual void unregisterInterface( const QUuid & pUuid ) override;
	virtual QObject * findInterface( const QUuid & pUuid ) override;
	virtual QSharedPointer<fugio::NodeInterface> createNode( const QString & pName, const QUuid & pGlobalId, const QUuid & pControlId, const QVariantHash & pSettings = QVariantHash() ) override;
	virtual QSharedPointer<fugio::PinInterface> createPin( const QString & pName, const QUuid & pGlobalId, const QUuid & pLocalId, PinDirection pDirection, const QUuid & pControlId = QUuid(), const QVariantHash & pSettings = QVariantHash() ) override;
	virtual QSharedPointer<fugio::PinControlInterface> createPinControl( const QUuid & pUUID, QSharedPointer<fugio::PinInterface> pPin ) override;
	virtual bool updatePinControl( QSharedPointer<fugio::PinInterface> pPin, const QUuid & pPinControlUuid ) override;
	virtual void registerNode( QSharedPointer<fugio::NodeInterface> pNode ) override;
	virtual void unregisterNode( const QUuid & pUUID ) override;
	virtual void renameNode( const QUuid & pUUID1, const QUuid & pUUID2 ) override;
	virtual QSharedPointer<fugio::NodeInterface> findNode( const QUuid & pUUID ) const override;
	virtual QSharedPointer<fugio::NodeInterface> findNode( const QString & pName ) const override;
	virtual QList<QUuid> nodeList( void ) const override;
	virtual void nodeInitialised( void ) override;
	virtual void updateNode( QSharedPointer<fugio::NodeInterface> pNode ) override;
	virtual void registerPin( QSharedPointer<fugio::PinInterface> pPin ) override;
	virtual void unregisterPin( const QUuid & pUUID ) override;
	virtual void renamePin( const QUuid & pUUID1, const QUuid & pUUID2 ) override;
	virtual QSharedPointer<fugio::PinInterface> findPin( const QUuid & pUUID ) override;
	virtual void pinUpdated( QSharedPointer<fugio::PinInterface> pPin, qint64 pGlobalTimestamp = -1, bool pUpdatedConnectedNode = true ) override;
	virtual QList<QSharedPointer<fugio::PinInterface>> connections( const QUuid & pUUID ) override;
	virtual bool isConnected( const QUuid & pUUID ) override;
	virtual bool isConnectedTo( const QUuid & pUUID1, const QUuid & pUUID2 ) override;
	virtual void connectPins( const QUuid & pUUID1, const QUuid & pUUID2 ) override;
	virtual void disconnectPins( const QUuid & pUUID1, const QUuid & pUUID2 ) override;
	virtual void disconnectPin( const QUuid & pUUID ) override;
	virtual void performance( QSharedPointer<fugio::NodeInterface> pNode, const QString & pName, qint64 pTimeStart, qint64 pTimeEnd ) override;
	virtual QList<fugio::PerfData> perfdata( void ) override;
	virtual void registerPlayhead( fugio::PlayheadInterface * pPlayHead ) override;
	virtual void unregisterPlayhead( fugio::PlayheadInterface * pPlayHead ) override;
	virtual bool isPlaying( void ) const override;
	virtual void setPlayheadTimerEnabled( bool pEnabled ) override;
	virtual void playheadMove( qreal pTimeStamp ) override;
	virtual void playheadPlay( qreal pTimePrev, qreal pTimeCurr ) override;
	virtual void play( void ) override;
	virtual void stop( void ) override;
	virtual void pause( void ) override;
	virtual void resume( void ) override;
	virtual void setPlayheadPosition( qreal pTimeStamp, bool pRefresh = true ) override;
	virtual TimeState state( void ) const override;
	virtual qreal position( void ) const override;
	virtual qreal duration( void ) const override;
	virtual void setDuration( qreal pDuration ) override;
	virtual void notifyAboutToPlay( void ) override;
	virtual QUuid registerExternalAsset( const QString & pFileName ) override;
	virtual void unregisterExternalAsset( const QString & pFileName ) override;
	virtual void unregisterExternalAsset( const QUuid & pUuid ) override;
	virtual QString externalAsset( const QUuid & pUuid ) const override;

public slots:
	void clear( void );

	virtual bool load( QString pFileName, bool pPartial = false ) Q_DECL_OVERRIDE;

	virtual bool save( QString pFileName, QList<QUuid> pNodeList = QList<QUuid>() ) const Q_DECL_OVERRIDE;

	void processFrame( qint64 pTimeStamp );

private:
	void run( void ) Q_DECL_OVERRIDE
	{
		exec();
	}

private:
	fugio::GlobalInterface			*mApp;

	ContextWorker					*mContextWorker;
};

#endif // CONTEXTPRIVATE_H
