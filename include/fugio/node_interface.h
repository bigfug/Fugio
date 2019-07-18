#ifndef INTERFACE_NODE_H
#define INTERFACE_NODE_H

#include <QObject>
#include <QList>
#include <QUuid>
#include <QSharedPointer>

#include "fugio.h"
#include "global.h"

class QSettings;

FUGIO_NAMESPACE_BEGIN
class ContextInterface;
class NodeSignals;
class NodeControlInterface;
class PinInterface;
class PinControlInterface;
FUGIO_NAMESPACE_END

FUGIO_NAMESPACE_BEGIN

class NodeInterface
{
public:
	virtual ~NodeInterface( void ) {}

	virtual QUuid uuid( void ) = 0;

	virtual fugio::NodeSignals *qobject( void ) = 0;

	virtual fugio::ContextInterface *context( void ) = 0;

	virtual void setContext( fugio::ContextInterface *pContext ) = 0;

	//-------------------------------------------------------------------------
	// Node Control

	virtual QSharedPointer<fugio::NodeControlInterface> control( void ) const = 0;

	virtual bool hasControl( void ) const = 0;

	virtual QUuid controlUuid( void ) const = 0;

	//-------------------------------------------------------------------------
	// Initialised/active flags

	virtual bool isInitialised( void ) const = 0;

	virtual bool isActive( void ) const = 0;

	virtual void setActive( bool pActive ) = 0;

	//-------------------------------------------------------------------------
	// Human readable name

	virtual QString name( void ) const = 0;

	virtual void setName( const QString &pName ) = 0;

	//-------------------------------------------------------------------------
	// Status

	typedef enum Status
	{
		Initialising, Deferred, Initialised, Warning, Error
	} Status;

	virtual Status status( void ) const = 0;

	virtual void setStatus( Status pStatus ) = 0;

	virtual QString statusMessage( void ) const = 0;

	virtual void setStatusMessage( const QString &pMessage ) = 0;

	//-------------------------------------------------------------------------
	// Pin lookup/enums

	virtual QSharedPointer<fugio::PinInterface> findPinByGlobalId( const QUuid &pUuid ) const = 0;

	virtual QSharedPointer<fugio::PinInterface> findPinByLocalId( const QUuid &pUuid ) const = 0;

	virtual QSharedPointer<fugio::PinInterface> findPinByName( const QString &pName ) const = 0;

	virtual QSharedPointer<fugio::PinInterface> findInputPinByName( const QString &pName ) const = 0;

	virtual QSharedPointer<fugio::PinInterface> findOutputPinByName( const QString &pName ) const = 0;

	virtual QList< QSharedPointer<fugio::PinInterface> > enumPins( void ) const = 0;

	virtual QList< QSharedPointer<fugio::PinInterface> > enumInputPins( void ) const = 0;

	virtual QList< QSharedPointer<fugio::PinInterface> > enumOutputPins( void ) const = 0;

	virtual bool hasPin( QSharedPointer<fugio::PinInterface> pPin ) const = 0;

	//-------------------------------------------------------------------------
	// Specialised/lower-level pin creation - generally use NodeControl helpers instead

	virtual QSharedPointer<fugio::PinInterface> createPin( const QString &pName, PinDirection pDirection, const QUuid &pGlobalId, const QUuid &pLocalId ) = 0;

	virtual QObject *createPin( const QString &pName, PinDirection pDirection, const QUuid &pGlobalId, const QUuid &pLocalId, QSharedPointer<fugio::PinInterface> &mPinInterface, const QUuid &pControlUUID ) = 0;

	virtual QSharedPointer<fugio::PinInterface> createProperty( const QString &pName, PinDirection pDirection, const QUuid &pGlobalId, const QUuid &pLocalId ) = 0;

	virtual QObject *createProperty( const QString &pName, PinDirection pDirection, const QUuid &pGlobalId, const QUuid &pLocalId, QSharedPointer<fugio::PinInterface> &mPinInterface, const QUuid &pControlUUID ) = 0;

	virtual void addPin( QSharedPointer<fugio::PinInterface> pPin ) = 0;

	virtual void removePin( QSharedPointer<fugio::PinInterface> pPin ) = 0;

	//-------------------------------------------------------------------------
	// Load and Save

	virtual void loadSettings( QSettings &pSettings, QMap<QUuid,QUuid> &pPinMap, bool pPartial ) = 0;

	virtual void saveSettings( QSettings &pSettings, bool pPartial ) const = 0;

	//-------------------------------------------------------------------------
	// Settings

	virtual void setSetting( const QString &pKey, const QVariant &pValue ) = 0;

	virtual QVariant setting( const QString &pKey, const QVariant &pDefault ) const = 0;

	virtual QVariantHash settings( void ) const = 0;

	//-------------------------------------------------------------------------
	// Paired Pins - id's are PinInterface::localId()

	typedef QPair<QUuid,QUuid>	UuidPair;

	virtual void pairPins( QSharedPointer<fugio::PinInterface> pPin1, QSharedPointer<fugio::PinInterface> pPin2 ) = 0;

	virtual void pairPins( const QUuid &pLocalId1, const QUuid &pLocalId2 ) = 0;

	virtual void unpairPins( QSharedPointer<fugio::PinInterface> pPin1, QSharedPointer<fugio::PinInterface> pPin2 ) = 0;

	virtual void unpairPins( const QUuid &pLocalId1, const QUuid &pLocalId2 ) = 0;

	virtual QList<UuidPair> pairedPins( void ) = 0;
};

FUGIO_NAMESPACE_END

Q_DECLARE_INTERFACE( fugio::NodeInterface, "com.bigfug.fugio.node/1.0" )

#endif // INTERFACE_NODE_H
