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

	//-------------------------------------------------------------------------
	// Node Control

	virtual QSharedPointer<fugio::NodeControlInterface> control( void ) = 0;

	virtual bool hasControl( void ) const = 0;

	virtual QUuid controlUuid( void ) const = 0;

	//-------------------------------------------------------------------------
	// Initialised/active flags

	virtual bool isInitialised( void ) const = 0;

	virtual bool isActive( void ) const = 0;

	virtual void setActive( bool pActive ) = 0;

	//-------------------------------------------------------------------------
	// Human readable name

	virtual const QString &name( void ) = 0;

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

	virtual QSharedPointer<fugio::PinInterface> findPinByGlobalId( const QUuid &pUuid ) = 0;

	virtual QSharedPointer<fugio::PinInterface> findPinByLocalId( const QUuid &pUuid ) = 0;

	virtual QSharedPointer<fugio::PinInterface> findPinByName( const QString &pName ) const = 0;

	virtual QSharedPointer<fugio::PinInterface> findInputPinByName( const QString &pName ) const = 0;

	virtual QSharedPointer<fugio::PinInterface> findOutputPinByName( const QString &pName ) const = 0;

	virtual QList< QSharedPointer<fugio::PinInterface> > enumPins( void ) = 0;

	virtual QList< QSharedPointer<fugio::PinInterface> > enumInputPins( void ) = 0;

	virtual QList< QSharedPointer<fugio::PinInterface> > enumOutputPins( void ) = 0;

	virtual bool hasPin( QSharedPointer<fugio::PinInterface> pPin ) const = 0;

	//-------------------------------------------------------------------------
	// Specialised/lower-level pin creation - generally use NodeControl helpers instead

	virtual QSharedPointer<fugio::PinInterface> createPin( const QString &pName, PinDirection pDirection, const QUuid &pLocalId ) = 0;

	virtual QObject *createPin( const QString &pName, PinDirection pDirection, const QUuid &pLocalId, QSharedPointer<fugio::PinInterface> &mPinInterface, const QUuid &pControlUUID ) = 0;

	virtual void addPin( QSharedPointer<fugio::PinInterface> pPin ) = 0;

	virtual void removePin( QSharedPointer<fugio::PinInterface> pPin ) = 0;

	//-------------------------------------------------------------------------
	// Load and Save

	virtual void loadSettings1( QSettings &pSettings, bool pPartial ) = 0;
	virtual void loadSettings2( QSettings &pSettings, QMap<QUuid,QUuid> &pPinMap, bool pPartial ) = 0;

	virtual void saveSettings1( QSettings &pSettings, bool pPartial ) = 0;
	virtual void saveSettings2( QSettings &pSettings, bool pPartial ) = 0;

	//-------------------------------------------------------------------------
	// Settings

	virtual void setSetting( const QString &pKey, const QVariant &pValue ) = 0;

	virtual QVariant setting( const QString &pKey, const QVariant &pDefault ) = 0;

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
