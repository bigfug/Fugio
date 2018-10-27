#ifndef INTERFACE_CONTEXT_H
#define INTERFACE_CONTEXT_H

#include <QUuid>
#include <QSharedPointer>
#include <QFuture>

//#include "context.h"

#include "global_interface.h"
#include "node_interface.h"
#include "pin_interface.h"

#include "global.h"

FUGIO_NAMESPACE_BEGIN

class ContextSignals;
class PlayheadInterface;

typedef struct PerfData
{
	QUuid							mUuid;
	QString							mNode;
	QString							mName;
	int								mCount;
	qint64							mTime;
} PerfData;

class ContextInterface
{
public:
	virtual ~ContextInterface( void ) {}

	virtual fugio::ContextSignals *qobject( void ) = 0;

	virtual fugio::GlobalInterface *global( void ) = 0;

	virtual bool load( const QString &pFileName, bool pPartial = false ) = 0;

	virtual bool unload( const QString &pFileName ) = 0;

	virtual bool save( const QString &pFileName, const QList<QUuid> *pNodeList = Q_NULLPTR ) const = 0;

	virtual bool loadData( const QString &pFileName ) = 0;

	virtual bool saveData( const QString &pFileName ) const = 0;

	virtual void setActive( bool pActive ) = 0;

	virtual bool active( void ) const = 0;

	virtual void futureSync( const QFuture<void> &pFuture ) = 0;

	//-------------------------------------------------------------------------
	// Meta Information

	typedef enum MetaInfo
	{
		Name, Author, Url, Description, Version, Filename, Created,
		User = 1000
	} MetaInfo;

	virtual QString metaInfo( MetaInfo pType ) const = 0;

	virtual void setMetaInfo( MetaInfo pType, const QString &pMetaData ) = 0;

	//-------------------------------------------------------------------------
	// Interfaces

	virtual void registerInterface( const QUuid &pUuid, QObject *pInterface ) = 0;

	virtual void unregisterInterface( const QUuid &pUuid ) = 0;

	virtual QObject *findInterface( const QUuid &pUuid ) = 0;

	//-------------------------------------------------------------------------

	virtual void doFrameStart( qint64 pTimeStamp ) = 0;
	virtual void doFrameProcess( qint64 pTimeStamp ) = 0;
	virtual void doFrameEnd( qint64 pTimeStamp ) = 0;

	//-------------------------------------------------------------------------
	// Nodes

	virtual QSharedPointer<fugio::NodeInterface> createNode( const QString &pName, const QUuid &pGlobalId, const QUuid &pControlId, const QVariantHash &pSettings = QVariantHash() ) = 0;

	virtual QSharedPointer<fugio::PinInterface> createPin( const QString &pName, const QUuid &pGlobalId, const QUuid &pLocalId, PinDirection pDirection, const QUuid &pControlId = QUuid(), const QVariantHash &pSettings = QVariantHash() ) = 0;

	virtual QSharedPointer<fugio::PinControlInterface> createPinControl( const QUuid &pUUID, QSharedPointer<fugio::PinInterface> pPin ) = 0;

	virtual bool updatePinControl( QSharedPointer<fugio::PinInterface> pPin, const QUuid &pPinControlUuid ) = 0;


	virtual void registerNode( QSharedPointer<fugio::NodeInterface> pNode ) = 0;

	virtual void unregisterNode( const QUuid &pUUID ) = 0;

	virtual void renameNode( const QUuid &pUUID1, const QUuid &pUUID2 ) = 0;

	virtual QSharedPointer<fugio::NodeInterface> findNode( const QUuid &pUUID ) const  = 0;
	virtual QSharedPointer<fugio::NodeInterface> findNode( const QString &pName ) const  = 0;

	virtual QList<QUuid> nodeList( void ) const = 0;

	virtual void nodeInitialised( void ) = 0;

	virtual void updateNode( QSharedPointer<fugio::NodeInterface> pNode ) = 0;

	//-------------------------------------------------------------------------
	// Pins - id's are globalId() not localId()

	virtual void registerPin( QSharedPointer<fugio::PinInterface> pPin ) = 0;
	virtual void unregisterPin( const QUuid &pUUID ) = 0;
	virtual void renamePin( const QUuid &pUUID1, const QUuid &pUUID2 ) = 0;
	virtual QSharedPointer<fugio::PinInterface> findPin( const QUuid &pUUID ) = 0;

	virtual void pinUpdated( QSharedPointer<fugio::PinInterface> pPin, qint64 pGlobalTimestamp = -1, bool pUpdatedConnectedNode = true ) = 0;

	//-------------------------------------------------------------------------
	// Connections

	virtual QList< QSharedPointer<fugio::PinInterface> > connections( const QUuid &pUUID ) = 0;

	virtual bool isConnected( const QUuid &pUUID ) = 0;
	virtual bool isConnectedTo( const QUuid &pUUID1, const QUuid &pUUID2 ) = 0;

	virtual void connectPins( const QUuid &pUUID1, const QUuid &pUUID2 ) = 0;
	virtual void disconnectPins( const QUuid &pUUID1, const QUuid &pUUID2 ) = 0;
	virtual void disconnectPin( const QUuid &pUUID ) = 0;

	//-------------------------------------------------------------------------
	// Node Performance

	virtual void performance( QSharedPointer<fugio::NodeInterface> pNode, const QString &pName, qint64 pTimeStart, qint64 pTimeEnd ) = 0;

	virtual QList<PerfData> perfdata( void ) = 0;

	//-------------------------------------------------------------------------
	// Context Playhead

	typedef enum TimeState
	{
		Stopped, Paused, Playing
	} TimeState;

	virtual void registerPlayhead( fugio::PlayheadInterface *pPlayHead ) = 0;
	virtual void unregisterPlayhead( fugio::PlayheadInterface *pPlayHead ) = 0;

	virtual bool isPlaying( void ) const = 0;

	virtual void setPlayheadTimerEnabled( bool pEnabled ) = 0;

	virtual void playheadMove( qreal pTimeStamp ) = 0;
	virtual void playheadPlay( qreal pTimePrev, qreal pTimeCurr ) = 0;

	virtual void play( void ) = 0;
	virtual void stop( void ) = 0;
	virtual void pause( void ) = 0;
	virtual void resume( void ) = 0;

	virtual void setPlayheadPosition( qreal pTimeStamp, bool pRefresh = true ) = 0;

	virtual TimeState state( void ) const = 0;

	virtual qreal position( void ) const = 0;
	virtual qreal duration( void ) const = 0;

	virtual void setDuration( qreal pDuration ) = 0;

	virtual void notifyAboutToPlay( void ) = 0;

	//-------------------------------------------------------------------------
	// External assets

	virtual QUuid registerExternalAsset( const QString &pFileName ) = 0;

	virtual void unregisterExternalAsset( const QString &pFileName ) = 0;
	virtual void unregisterExternalAsset( const QUuid &pUuid ) = 0;

	virtual QString externalAsset( const QUuid &pUuid ) const = 0;
};

FUGIO_NAMESPACE_END

Q_DECLARE_INTERFACE( fugio::ContextInterface, "com.bigfug.fugio.Context/1.0" )

#endif // INTERFACE_CONTEXT_H
