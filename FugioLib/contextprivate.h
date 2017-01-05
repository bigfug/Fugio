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

	virtual fugio::ContextSignals *qobject( void ) Q_DECL_OVERRIDE
	{
		return( this );
	}

	virtual fugio::GlobalInterface *global( void ) Q_DECL_OVERRIDE
	{
		return( mApp );
	}

	virtual void setActive( bool pActive ) Q_DECL_OVERRIDE;

	virtual bool active( void ) const Q_DECL_OVERRIDE
	{
		return( mActive );
	}

	virtual void futureSync( const QFuture<void> &pFuture ) Q_DECL_OVERRIDE
	{
		mFutureSync.addFuture( pFuture );
	}

	virtual QString metaInfo( MetaInfo pType ) const Q_DECL_OVERRIDE;

	virtual void setMetaInfo( MetaInfo pType, const QString &pMetaData ) Q_DECL_OVERRIDE;

	virtual bool load( const QString &pFileName, bool pPartial = false ) Q_DECL_OVERRIDE;

	virtual bool unload( const QString &pFileName ) Q_DECL_OVERRIDE;

	virtual bool save( const QString &pFileName, const QList<QUuid> *pNodeList = 0 ) const Q_DECL_OVERRIDE;

	virtual void registerInterface( const QUuid &pUuid, QObject *pInterface ) Q_DECL_OVERRIDE;

	virtual void unregisterInterface( const QUuid &pUuid ) Q_DECL_OVERRIDE;

	virtual QObject *findInterface( const QUuid &pUuid ) Q_DECL_OVERRIDE;

	virtual void doFrameInitialise( qint64 pTimeStamp );
	virtual void doFrameStart( qint64 pTimeStamp ) Q_DECL_OVERRIDE;
	virtual void doFrameProcess( qint64 pTimeStamp ) Q_DECL_OVERRIDE;
	virtual void doFrameEnd( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

	virtual void updateNode( QSharedPointer<fugio::NodeInterface> pNode ) Q_DECL_OVERRIDE;

	virtual QSharedPointer<fugio::NodeInterface> createNode( const QString &pName, const QUuid &pGlobalId, const QUuid &pControlId, const QVariantHash &pSettings ) Q_DECL_OVERRIDE;

	virtual QSharedPointer<fugio::PinInterface> createPin( const QString &pName, const QUuid &pGlobalId, const QUuid &pLocalId, PinDirection pDirection, const QUuid &pControlUUID, const QVariantHash &pSettings ) Q_DECL_OVERRIDE;

	virtual QSharedPointer<fugio::PinControlInterface> createPinControl( const QUuid &pUUID, QSharedPointer<fugio::PinInterface> pPin ) Q_DECL_OVERRIDE;

	virtual bool updatePinControl( QSharedPointer<fugio::PinInterface> pPin, const QUuid &pPinControlUuid ) Q_DECL_OVERRIDE;

	virtual void registerNode( QSharedPointer<fugio::NodeInterface> pNode ) Q_DECL_OVERRIDE;
	virtual void unregisterNode( const QUuid &pUUID ) Q_DECL_OVERRIDE;
	virtual void renameNode( const QUuid &pUUID1, const QUuid &pUUID2 ) Q_DECL_OVERRIDE;
	virtual QSharedPointer<fugio::NodeInterface> findNode( const QUuid &pUUID ) const Q_DECL_OVERRIDE;
	virtual QSharedPointer<fugio::NodeInterface> findNode( const QString &pName ) const Q_DECL_OVERRIDE;

	virtual void registerPin( QSharedPointer<fugio::PinInterface> pPin ) Q_DECL_OVERRIDE;
	virtual void unregisterPin( const QUuid &pUUID ) Q_DECL_OVERRIDE;
	virtual void renamePin( const QUuid &pUUID1, const QUuid &pUUID2 ) Q_DECL_OVERRIDE;
	virtual QSharedPointer<fugio::PinInterface> findPin( const QUuid &pUUID ) Q_DECL_OVERRIDE;

	virtual QList< QSharedPointer<fugio::PinInterface> > connections( const QUuid &pUUID ) Q_DECL_OVERRIDE;

	virtual bool isConnected( const QUuid &pUUID ) Q_DECL_OVERRIDE;
	virtual bool isConnectedTo( const QUuid &pUUID1, const QUuid &pUUID2 ) Q_DECL_OVERRIDE;

	virtual void connectPins( const QUuid &pUUID1, const QUuid &pUUID2 ) Q_DECL_OVERRIDE;
	virtual void disconnectPins( const QUuid &pUUID1, const QUuid &pUUID2 ) Q_DECL_OVERRIDE;
	virtual void disconnectPin( const QUuid &pUUID ) Q_DECL_OVERRIDE;

	virtual QList<QUuid> nodeList( void ) const Q_DECL_OVERRIDE;

	virtual void nodeInitialised( void ) Q_DECL_OVERRIDE;

	virtual void pinUpdated( QSharedPointer<fugio::PinInterface> pPin, bool pUpdatedConnectedNode = true ) Q_DECL_OVERRIDE;

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

	void onNodeActivationChanged( QSharedPointer<fugio::NodeInterface> pNode );
	void nodeControlChanged( QSharedPointer<fugio::NodeInterface> pNode );
	void nodeNameChanged( QSharedPointer<fugio::NodeInterface> pNode );

	void onPinAdded( QSharedPointer<fugio::NodeInterface> pNode, QSharedPointer<fugio::PinInterface> pPin );
	void onPinRemoved( QSharedPointer<fugio::NodeInterface> pNode, QSharedPointer<fugio::PinInterface> pPin );

	virtual void play() Q_DECL_OVERRIDE;
	virtual void stop() Q_DECL_OVERRIDE;
	virtual void pause() Q_DECL_OVERRIDE;
	virtual void resume() Q_DECL_OVERRIDE;

	void playFrom( qreal pTimeStamp );

protected:
	void processDeferredNodes( QList< QSharedPointer<fugio::NodeInterface> > WaitingNodes, QList< QSharedPointer<fugio::NodeInterface> > &InitialisedNodes );

private:
	void processUpdatedNodes( qint64 pTimeStamp );

	void processUpdatedPins( qint64 pTimeStamp );

	void processPlayhead( qint64 pTimeStamp );

	static void processNode( QSharedPointer<fugio::NodeInterface> pNode, qint64 pTimeStamp );

	bool loadSettings( QSettings &pSettings, bool pPartial );

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
};

#endif // CONTEXTPRIVATE_H
