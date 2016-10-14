#include "xinputnode.h"

#include <fugio/node_interface.h>
#include <fugio/context_interface.h>
#include <fugio/context_signals.h>
#include <fugio/core/uuid.h>

XInputNode::XInputNode( QSharedPointer<fugio::NodeInterface> pNode ) :
	NodeControlBase( pNode )
{
#if defined( XINPUT_SUPPORTED )
	memset( &mLastState, 0, sizeof( mLastState ) );
#endif

	FUGID( PIN_INPUT_UNIT, "da55f50c-1dee-47c9-bee5-6a2e48672a13" );

	FUGID( PIN_OUTPUT_STICK_LEFT_X, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_OUTPUT_STICK_LEFT_Y, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );
	FUGID( PIN_OUTPUT_STICK_LEFT_M, "261cc653-d7fa-4c34-a08b-3603e8ae71d5" );
	FUGID( PIN_OUTPUT_STICK_RIGHT_X, "249f2932-f483-422f-b811-ab679f006381" );
	FUGID( PIN_OUTPUT_STICK_RIGHT_Y, "ce8d578e-c5a4-422f-b3c4-a1bdf40facdb" );
	FUGID( PIN_OUTPUT_STICK_RIGHT_M, "e6bf944e-5f46-4994-bd51-13c2aa6415b7" );
	FUGID( PIN_OUTPUT_BUTTON_A, "a2bbf374-0dc8-42cb-b85a-6a43b58a348f" );
	FUGID( PIN_OUTPUT_BUTTON_B, "51297977-7b4b-4e08-9dea-89a8add4abe0" );
	FUGID( PIN_OUTPUT_BUTTON_X, "c997473a-2016-466b-9128-beacb99870a2" );
	FUGID( PIN_OUTPUT_BUTTON_Y, "e27ce75e-fb9f-4a57-97b8-1c2d966f053b" );
	FUGID( PIN_OUTPUT_TRIGGER_L, "3d995a98-0f56-4371-a1fa-67ecaefeba1c" );
	FUGID( PIN_OUTPUT_TRIGGER_R, "d77f1053-0f88-4b4a-bfc6-57be3bb1eddd" );
	FUGID( PIN_OUTPUT_DPAD_UP, "1ceaeb2d-0463-44bc-9b80-10129cd8f4eb" );
	FUGID( PIN_OUTPUT_DPAD_RIGHT, "5064e449-8b0b-4447-9009-c81997f754ef" );
	FUGID( PIN_OUTPUT_DPAD_DOWN, "5c8f8f4e-58ce-4e47-9e1e-4168d17e1863" );
	FUGID( PIN_OUTPUT_DPAD_LEFT, "a9b8d8d8-e4aa-4a99-b4eb-ea22f1945c63" );

	FUGID( PIN_OUTPUT_START, "87bffde4-66e0-40bf-9f48-c963f6619af9" );
	FUGID( PIN_OUTPUT_BACK, "6b956399-4368-421c-b030-37d9c6eae66c" );

	FUGID( PIN_OUTPUT_THUMB_L, "02cfe97a-fd14-47d6-b35f-f1d0fd691cf0" );
	FUGID( PIN_OUTPUT_THUMB_R, "8de5d105-f0d5-48e8-bef3-2b0f57592328" );

	FUGID( PIN_OUTPUT_SHOULDER_L, "77efa180-06fd-466d-a751-5e24be4ca9d9" );
	FUGID( PIN_OUTPUT_SHOULDER_R, "4042ce30-4d86-41f1-9f21-8a8e93ed99cf" );

	mPinInputIndex = pinInput( "Index", PIN_INPUT_UNIT );

	mStickLeftX = pinOutput<fugio::VariantInterface *>( "Left Stick X", mPinStickLeftX, PID_INTEGER, PIN_OUTPUT_STICK_LEFT_X );
	mStickLeftY = pinOutput<fugio::VariantInterface *>( "Left Stick Y", mPinStickLeftY, PID_INTEGER, PIN_OUTPUT_STICK_LEFT_Y );
	mStickLeftM = pinOutput<fugio::VariantInterface *>( "Left Stick Magnitutde", mPinStickLeftM, PID_FLOAT, PIN_OUTPUT_STICK_LEFT_M );

	mStickRightX = pinOutput<fugio::VariantInterface *>( "Right Stick X", mPinStickRightX, PID_FLOAT, PIN_OUTPUT_STICK_RIGHT_X );
	mStickRightY = pinOutput<fugio::VariantInterface *>( "Right Stick Y", mPinStickRightY, PID_FLOAT, PIN_OUTPUT_STICK_RIGHT_Y );
	mStickRightM = pinOutput<fugio::VariantInterface *>( "Right Stick Magnitutde", mPinStickRightM, PID_FLOAT, PIN_OUTPUT_STICK_RIGHT_M );

	mButtonA = pinOutput<fugio::VariantInterface *>( "Button A", mPinButtonA, PID_BOOL, PIN_OUTPUT_BUTTON_A );
	mButtonB = pinOutput<fugio::VariantInterface *>( "Button B", mPinButtonB, PID_BOOL, PIN_OUTPUT_BUTTON_B );
	mButtonX = pinOutput<fugio::VariantInterface *>( "Button X", mPinButtonX, PID_BOOL, PIN_OUTPUT_BUTTON_X );
	mButtonY = pinOutput<fugio::VariantInterface *>( "Button Y", mPinButtonY, PID_BOOL, PIN_OUTPUT_BUTTON_Y );

	mTriggerL = pinOutput<fugio::VariantInterface *>( "Left Trigger", mPinTriggerL, PID_FLOAT, PIN_OUTPUT_TRIGGER_L );
	mTriggerR = pinOutput<fugio::VariantInterface *>( "Right Trigger", mPinTriggerR, PID_FLOAT, PIN_OUTPUT_TRIGGER_R );

	mDPadUp    = pinOutput<fugio::VariantInterface *>( "DPad Up", mPinDPadUp, PID_BOOL, PIN_OUTPUT_DPAD_UP );
	mDPadRight = pinOutput<fugio::VariantInterface *>( "DPad Right", mPinDPadRight, PID_BOOL, PIN_OUTPUT_DPAD_RIGHT );
	mDPadDown  = pinOutput<fugio::VariantInterface *>( "DPad Down", mPinDPadDown, PID_BOOL, PIN_OUTPUT_DPAD_DOWN );
	mDPadLeft  = pinOutput<fugio::VariantInterface *>( "DPad Left", mPinDPadLeft, PID_BOOL, PIN_OUTPUT_DPAD_LEFT );

	mStart = pinOutput<fugio::VariantInterface *>( "Start", mPinStart, PID_BOOL, PIN_OUTPUT_START );
	mBack  = pinOutput<fugio::VariantInterface *>( "Back", mPinBack, PID_BOOL, PIN_OUTPUT_BACK );

	mThumbL = pinOutput<fugio::VariantInterface *>( "Left Thumb", mPinThumbL, PID_BOOL, PIN_OUTPUT_THUMB_L );
	mThumbR = pinOutput<fugio::VariantInterface *>( "Right Thumb", mPinThumbR, PID_BOOL, PIN_OUTPUT_THUMB_R );

	mShoulderL = pinOutput<fugio::VariantInterface *>( "Left Shoulder", mPinShoulderL, PID_BOOL, PIN_OUTPUT_SHOULDER_L );
	mShoulderR = pinOutput<fugio::VariantInterface *>( "Right Shoulder", mPinShoulderR, PID_BOOL, PIN_OUTPUT_SHOULDER_R );
}

XInputNode::~XInputNode( void )
{

}

bool XInputNode::initialise()
{
	if( !fugio::NodeControlBase::initialise() )
	{
		return( false );
	}

	connect( mNode->context()->qobject(), SIGNAL(frameStart()), this, SLOT(frameStart()) );

	return( true );
}

void XInputNode::frameStart( void )
{
	int		CtlIdx = variant( mPinInputIndex ).toInt();

#if defined( XINPUT_SUPPORTED )
	XINPUT_STATE		State;

	if( XInputGetState( CtlIdx, &State ) != ERROR_SUCCESS )
	{
		mNode->setStatus( fugio::NodeInterface::Error );

		return;
	}

	mNode->setStatus( fugio::NodeInterface::Initialised );

	if( ( State.Gamepad.wButtons & XINPUT_GAMEPAD_A ) != ( mLastState.Gamepad.wButtons & XINPUT_GAMEPAD_A ) )
	{
		mButtonA->setVariant( ( State.Gamepad.wButtons & XINPUT_GAMEPAD_A ) != 0 );

		mNode->context()->pinUpdated( mPinButtonA );
	}

	if( ( State.Gamepad.wButtons & XINPUT_GAMEPAD_B ) != ( mLastState.Gamepad.wButtons & XINPUT_GAMEPAD_B ) )
	{
		mButtonB->setVariant( ( State.Gamepad.wButtons & XINPUT_GAMEPAD_B ) != 0 );

		mNode->context()->pinUpdated( mPinButtonB );
	}

	if( ( State.Gamepad.wButtons & XINPUT_GAMEPAD_X ) != ( mLastState.Gamepad.wButtons & XINPUT_GAMEPAD_X ) )
	{
		mButtonX->setVariant( ( State.Gamepad.wButtons & XINPUT_GAMEPAD_X ) != 0 );

		mNode->context()->pinUpdated( mPinButtonX );
	}

	if( ( State.Gamepad.wButtons & XINPUT_GAMEPAD_Y ) != ( mLastState.Gamepad.wButtons & XINPUT_GAMEPAD_Y ) )
	{
		mButtonY->setVariant( ( State.Gamepad.wButtons & XINPUT_GAMEPAD_Y ) != 0 );

		mNode->context()->pinUpdated( mPinButtonY );
	}

	//-------------------------------------------------------------------------
	// START/BACK

	if( ( State.Gamepad.wButtons & XINPUT_GAMEPAD_START ) != ( mLastState.Gamepad.wButtons & XINPUT_GAMEPAD_START ) )
	{
		mStart->setVariant( ( State.Gamepad.wButtons & XINPUT_GAMEPAD_START ) != 0 );

		pinUpdated( mPinStart );
	}

	if( ( State.Gamepad.wButtons & XINPUT_GAMEPAD_BACK ) != ( mLastState.Gamepad.wButtons & XINPUT_GAMEPAD_BACK ) )
	{
		mBack->setVariant( ( State.Gamepad.wButtons & XINPUT_GAMEPAD_BACK ) != 0 );

		pinUpdated( mPinBack );
	}

	//-------------------------------------------------------------------------
	// THUMB

	if( ( State.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB ) != ( mLastState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB ) )
	{
		mThumbL->setVariant( ( State.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB ) != 0 );

		pinUpdated( mPinThumbL );
	}

	if( ( State.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB ) != ( mLastState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB ) )
	{
		mThumbR->setVariant( ( State.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB ) != 0 );

		pinUpdated( mPinThumbR );
	}

	//-------------------------------------------------------------------------
	// SHOULDER

	if( ( State.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER ) != ( mLastState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER ) )
	{
		mShoulderL->setVariant( ( State.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER ) != 0 );

		pinUpdated( mPinShoulderL );
	}

	if( ( State.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER ) != ( mLastState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER ) )
	{
		mShoulderR->setVariant( ( State.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER ) != 0 );

		pinUpdated( mPinShoulderR );
	}

	//-------------------------------------------------------------------------
	// DPAD

	if( ( State.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP ) != ( mLastState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP ) )
	{
		mDPadUp->setVariant( ( State.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP ) != 0 );

		pinUpdated( mPinDPadUp );
	}

	if( ( State.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT ) != ( mLastState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT ) )
	{
		mDPadRight->setVariant( ( State.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT ) != 0 );

		pinUpdated( mPinDPadRight );
	}

	if( ( State.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN ) != ( mLastState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN ) )
	{
		mDPadDown->setVariant( ( State.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN ) != 0 );

		pinUpdated( mPinDPadDown );
	}

	if( ( State.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT ) != ( mLastState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT ) )
	{
		mDPadLeft->setVariant( ( State.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT ) != 0 );

		pinUpdated( mPinDPadLeft );
	}

	//-------------------------------------------------------------------------
	// Triggers

	if( State.Gamepad.bLeftTrigger != mLastState.Gamepad.bLeftTrigger )
	{
		mTriggerR->setVariant( qreal( State.Gamepad.bRightTrigger ) / 255.0 );

		mNode->context()->pinUpdated( mPinTriggerL );
	}

	if( State.Gamepad.bRightTrigger != mLastState.Gamepad.bRightTrigger )
	{
		mTriggerR->setVariant( qreal( State.Gamepad.bRightTrigger ) / 255.0 );

		mNode->context()->pinUpdated( mPinTriggerR );
	}

	if( State.Gamepad.sThumbLX != mLastState.Gamepad.sThumbLX || State.Gamepad.sThumbLY != mLastState.Gamepad.sThumbLY )
	{
		float LX = State.Gamepad.sThumbLX;
		float LY = State.Gamepad.sThumbLY;

		//determine how far the controller is pushed
		float magnitude = sqrt(LX*LX + LY*LY);

		//determine the direction the controller is pushed
//		float normalizedLX = LX / magnitude;
//		float normalizedLY = LY / magnitude;

		float normalizedMagnitude = 0;

		//check if the controller is outside a circular dead zone
		if ( magnitude > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE )
		{
		  //clip the magnitude at its expected maximum value
		  if ( magnitude > 32767) magnitude = 32767;

		  //adjust magnitude relative to the end of the dead zone
		  magnitude -= XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;

		  //optionally normalize the magnitude with respect to its expected range
		  //giving a magnitude value of 0.0 to 1.0
		  normalizedMagnitude = magnitude / ( 32767 - XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE );
		}
		else //if the controller is in the deadzone zero out the magnitude
		{
			magnitude = 0.0;
			normalizedMagnitude = 0.0;
		}

		mStickLeftX->setVariant( State.Gamepad.sThumbLX );
		mStickLeftY->setVariant( State.Gamepad.sThumbLY );
		mStickLeftM->setVariant( normalizedMagnitude );

		mNode->context()->pinUpdated( mPinStickLeftX );
		mNode->context()->pinUpdated( mPinStickLeftY );
		mNode->context()->pinUpdated( mPinStickLeftM );
	}

	if( State.Gamepad.sThumbRX != mLastState.Gamepad.sThumbRX || State.Gamepad.sThumbRY != mLastState.Gamepad.sThumbRY )
	{
		float RX = State.Gamepad.sThumbRX;
		float RY = State.Gamepad.sThumbRY;

		//determine how far the controller is pushed
		float magnitude = sqrt(RX*RX + RY*RY);

		//determine the direction the controller is pushed
//		float normalizedLX = RX / magnitude;
//		float normalizedLY = RY / magnitude;

		float normalizedMagnitude = 0;

		//check if the controller is outside a circular dead zone
		if ( magnitude > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE )
		{
		  //clip the magnitude at its expected maximum value
		  if ( magnitude > 32767) magnitude = 32767;

		  //adjust magnitude relative to the end of the dead zone
		  magnitude -= XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;

		  //optionally normalize the magnitude with respect to its expected range
		  //giving a magnitude value of 0.0 to 1.0
		  normalizedMagnitude = magnitude / ( 32767 - XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE );
		}
		else //if the controller is in the deadzone zero out the magnitude
		{
			magnitude = 0.0;
			normalizedMagnitude = 0.0;
		}

		mStickRightX->setVariant( State.Gamepad.sThumbRX );
		mStickRightY->setVariant( State.Gamepad.sThumbRY );
		mStickRightM->setVariant( normalizedMagnitude );

		pinUpdated( mPinStickRightX );
		pinUpdated( mPinStickRightY );
		pinUpdated( mPinStickRightM );
	}

	mLastState = State;
#endif
}

