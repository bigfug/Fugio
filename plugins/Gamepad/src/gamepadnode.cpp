#include "gamepadnode.h"

#include <fugio/core/uuid.h>

GamepadNode::GamepadNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_DEVICE_ID,		"9e154e12-bcd8-4ead-95b1-5a59833bcf4e" )
	FUGID( PIN_OUTPUT_CONNECTED,	"1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" )
	FUGID( PIN_OUTPUT_NAME,			"51297977-7b4b-4e08-9dea-89a8add4abe0" )
	FUGID( PIN_OUTPUT_LEFT_AXIS_X,	"c997473a-2016-466b-9128-beacb99870a2" )
	FUGID( PIN_OUTPUT_LEFT_AXIS_Y,	"e27ce75e-fb9f-4a57-97b8-1c2d966f053b" )
	FUGID( PIN_OUTPUT_RIGHT_AXIS_X, "b13398e2-4e71-460f-a3b1-499ce9af224d" )
	FUGID( PIN_OUTPUT_RIGHT_AXIS_Y, "fe19f444-7a5f-47e6-ae83-ceb43b8ae915" )

	mPinInputDeviceId = pinInput( tr( "Device Id" ), PIN_INPUT_DEVICE_ID );

	mPinInputDeviceId->registerPinInputType( PID_INTEGER );

	mPinInputDeviceId->setValue( 0 );

	mValOutputConnected = pinOutput<fugio::VariantInterface *>( tr( "Connected" ), mPinOutputConnected, PID_BOOL, PIN_OUTPUT_CONNECTED );

	mValOutputName = pinOutput<fugio::VariantInterface *>( tr( "Name" ), mPinOutputName, PID_STRING, PIN_OUTPUT_NAME );

	//-------------------------------------------------------------------------
	// Pins for gamepad controls

	mValOutputLeftAxisX = pinOutput<fugio::VariantInterface *>( tr( "Left Axis X" ), mPinOutputLeftAxisX, PID_FLOAT, PIN_OUTPUT_LEFT_AXIS_X );
	mValOutputLeftAxisY = pinOutput<fugio::VariantInterface *>( tr( "Left Axis Y" ), mPinOutputLeftAxisY, PID_FLOAT, PIN_OUTPUT_LEFT_AXIS_Y );

	mValOutputRightAxisX = pinOutput<fugio::VariantInterface *>( tr( "Right Axis X" ), mPinOutputRightAxisX, PID_FLOAT, PIN_OUTPUT_RIGHT_AXIS_X );
	mValOutputRightAxisY = pinOutput<fugio::VariantInterface *>( tr( "Right Axis Y" ), mPinOutputRightAxisY, PID_FLOAT, PIN_OUTPUT_RIGHT_AXIS_Y );

	//-------------------------------------------------------------------------
	// Connect all the signals for the gamepad

	connect( &mGamepad, &QGamepad::deviceIdChanged, this, [=]( int )
	{
		nodeUpdate();
	} );

	connect( &mGamepad, &QGamepad::connectedChanged, this, [=]( bool )
	{
		nodeUpdate();
	} );

	connect( &mGamepad, &QGamepad::nameChanged, this, [=]( QString )
	{
		nodeUpdate();
	} );

	connect( &mGamepad, &QGamepad::axisLeftXChanged, this, [=]( double v )
	{
		QMutexLocker	L( &mUpdatedMutex );

		mUpdatedControls.insert( axisLeftX, v );

		nodeUpdate();
	} );

	connect( &mGamepad, &QGamepad::axisLeftYChanged, this, [=]( double v )
	{
		QMutexLocker	L( &mUpdatedMutex );

		mUpdatedControls.insert( axisLeftY, v );

		nodeUpdate();
	} );

	connect( &mGamepad, &QGamepad::axisRightXChanged, this, [=]( double v )
	{
		QMutexLocker	L( &mUpdatedMutex );

		mUpdatedControls.insert( axisRightX, v );

		nodeUpdate();
	} );

	connect( &mGamepad, &QGamepad::axisRightYChanged, this, [=]( double v )
	{
		QMutexLocker	L( &mUpdatedMutex );

		mUpdatedControls.insert( axisRightY, v );

		nodeUpdate();
	} );
}

void GamepadNode::inputsUpdated( qint64 pTimeStamp )
{
	fugio::NodeControlBase::inputsUpdated( pTimeStamp );

	const int		DeviceId = variant( mPinInputDeviceId ).toInt();

	if( DeviceId != mGamepad.deviceId() )
	{
		mGamepad.setDeviceId( DeviceId );
	}

	if( mGamepad.isConnected() != mValOutputConnected->variant().toBool() )
	{
		mValOutputConnected->setVariant( mGamepad.isConnected() );

		pinUpdated( mPinOutputConnected );
	}

	if( mGamepad.name() != mValOutputName->variant().toString() )
	{
		mValOutputName->setVariant( mGamepad.name() );

		pinUpdated( mPinOutputName );
	}

	if( !mGamepad.isConnected() )
	{
		return;
	}

	QMutexLocker	L( &mUpdatedMutex );

	for( QMap<GamepadControl, QVariant>::iterator it = mUpdatedControls.begin() ; it != mUpdatedControls.end() ; it++ )
	{
		QSharedPointer<fugio::PinInterface>		 P;
		fugio::VariantInterface					*V = Q_NULLPTR;

		switch( it.key() )
		{
			case axisLeftX:
				P = mPinOutputLeftAxisX;
				V = mValOutputLeftAxisX;
				break;

			case axisLeftY:
				P = mPinOutputLeftAxisY;
				V = mValOutputLeftAxisY;
				break;

			case axisRightX:
				P = mPinOutputRightAxisX;
				V = mValOutputRightAxisX;
				break;

			case axisRightY:
				P = mPinOutputRightAxisY;
				V = mValOutputRightAxisY;
				break;
		}

		if( V && V->variant() != it.value() )
		{
			V->setVariant( it.value() );

			pinUpdated( P );
		}
	}

	mUpdatedControls.clear();
}
