#ifndef INTERFACE_PIN_H
#define INTERFACE_PIN_H

#include <QObject>
#include <QUuid>
#include <QSharedPointer>

#include "fugio.h"
#include "global.h"
#include "pin_control_interface.h"
//#include "pin.h"

#define PID_FUGIO_NODE_TRIGGER				(QUuid("{94FBD706-66D8-462B-BBDE-B46CB15B059D}"))

FUGIO_NAMESPACE_BEGIN
class NodeInterface;
class NodeControlInterface;
class PinSignals;
FUGIO_NAMESPACE_END

class QSettings;

FUGIO_NAMESPACE_BEGIN

class PinInterface
{
public:
	virtual ~PinInterface( void ) {}

	virtual fugio::PinSignals *qobject( void ) = 0;

	virtual QUuid globalId( void ) const = 0;

	virtual QUuid localId( void ) const = 0;

	virtual QString name( void ) const = 0;

	virtual PinDirection direction( void ) const = 0;

	virtual int order( void ) const = 0;

	virtual void setOrder( int pOrder ) = 0;

	virtual bool removable( void ) const = 0;

	virtual void setRemovable( bool pRemovable ) = 0;

	virtual bool hidden( void ) const = 0;

	virtual void setHidden( bool pHidden ) = 0;

	virtual bool updatable( void ) const = 0;

	virtual void setUpdatable( bool pUpdatable ) = 0;

	virtual bool autoRename( void ) const = 0;

	virtual void setAutoRename( bool pAutoRename ) = 0;

	virtual fugio::NodeInterface *node( void ) = 0;

	virtual qint64 updated( void ) const = 0;

	virtual qint64 updatedGlobal( void ) const = 0;		// when the update actually took place in global time

	virtual bool isConnected( void ) const = 0;

	virtual bool isConnectedToActiveNode( void ) const = 0;

	virtual bool isUpdated( qint64 pTimeStamp ) const = 0;

	virtual QSharedPointer<PinControlInterface> control( void ) = 0;

	virtual QSharedPointer<PinControlInterface> control( void ) const = 0;

	virtual void setControl( QSharedPointer<PinControlInterface> pInterface ) = 0;

	virtual bool hasControl( void ) const = 0;

	//virtual void setUpdated( qint64 pTimeStamp, bool pUpdatedConnectedNode = true ) = 0;

	virtual void setValue( const QVariant &pVariant ) = 0;

	virtual QVariant value( void ) const = 0;

	virtual void loadSettings( QSettings &pSettings, bool pPartial ) = 0;

	virtual void saveSettings( QSettings &pSettings ) const = 0;

	virtual fugio::NodeInterface *connectedNode( void ) = 0;

	virtual QSharedPointer<fugio::PinInterface> connectedPin( void ) const = 0;

	virtual QList< QSharedPointer<fugio::PinInterface> > connectedPins( void ) const = 0;

	virtual void setSetting( const QString &pKey, const QVariant &pValue ) = 0;

	virtual void clearSetting( const QString &pKey ) = 0;

	virtual QVariant setting( const QString &pKey, const QVariant &pDefault ) const = 0;

	virtual void registerInterface( const QUuid &pUuid, QObject *pObject ) = 0;
	virtual void unregisterInterface( const QUuid &pUuid ) = 0;

	virtual QObject *findInterface( const QUuid &pUuid ) const = 0;

	virtual QObjectList interfaces( void ) const = 0;

	virtual QUuid controlUuid( void ) const = 0;

	virtual void setName( const QString &pName ) = 0;

	virtual void registerPinInputTypes( const QList<QUuid> &pTypeList ) = 0;
	virtual void registerPinInputType( const QUuid &pTypeList ) = 0;

	virtual QList<QUuid> inputTypes( void ) const = 0;

	virtual void setDescription( const QString &pDescription ) = 0;

	virtual QString description( void ) const = 0;

	virtual void setAlwaysUpdate( bool pAlwaysUpdate = true ) = 0;

	virtual bool alwaysUpdate( void ) const = 0;

	virtual void setDisplayLabel( QString pDisplayLabel ) = 0;

	virtual QString displayLabel( void ) const = 0;

	//-------------------------------------------------------------------------
	// Paired pins - id's are PinInterface::localId()

	virtual QUuid pairedUuid( void ) const = 0;
};

FUGIO_NAMESPACE_END

Q_DECLARE_INTERFACE( fugio::PinInterface, "com.bigfug.fugio.pin/1.0" )

#endif // INTERFACE_PIN_H
