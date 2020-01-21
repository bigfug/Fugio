#ifndef GAMEPADNODE_H
#define GAMEPADNODE_H

#include <QObject>

#include <QGamepad>
#include <QMutex>

#include <fugio/nodecontrolbase.h>

#include <fugio/core/variant_interface.h>

class GamepadNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Gamepod" )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/Gamepad" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE GamepadNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~GamepadNode( void ) Q_DECL_OVERRIDE {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

protected:
	QGamepad									 mGamepad;

	QSharedPointer<fugio::PinInterface>			 mPinInputDeviceId;

	QSharedPointer<fugio::PinInterface>			 mPinOutputConnected;
	fugio::VariantInterface						*mValOutputConnected;

	QSharedPointer<fugio::PinInterface>			 mPinOutputName;
	fugio::VariantInterface						*mValOutputName;

	QSharedPointer<fugio::PinInterface>			 mPinOutputLeftAxisX;
	fugio::VariantInterface						*mValOutputLeftAxisX;
	QSharedPointer<fugio::PinInterface>			 mPinOutputLeftAxisY;
	fugio::VariantInterface						*mValOutputLeftAxisY;

	QSharedPointer<fugio::PinInterface>			 mPinOutputRightAxisX;
	fugio::VariantInterface						*mValOutputRightAxisX;
	QSharedPointer<fugio::PinInterface>			 mPinOutputRightAxisY;
	fugio::VariantInterface						*mValOutputRightAxisY;

	typedef enum GamepadControl
	{
		axisLeftX,
		axisLeftY,
		axisRightX,
		axisRightY
	} GamepadControl;

	QMap<GamepadControl, QVariant>				 mUpdatedControls;
	QMutex										 mUpdatedMutex;
};

#endif // GAMEPADNODE_H
