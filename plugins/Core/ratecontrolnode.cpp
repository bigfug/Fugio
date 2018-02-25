#include "ratecontrolnode.h"

#include <fugio/core/uuid.h>

#include <fugio/context_interface.h>

RateControlNode::RateControlNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mHaveInput( false ), mHaveTrigger( false ), mTimeInput( 0 ), mTimeTrigger( 0 )
{
	FUGID( PIN_INPUT_INPUT, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_OUTPUT_OUTPUT, "261cc653-d7fa-4c34-a08b-3603e8ae71d5" );

	mPinInput   = pinInput( "Input", PIN_INPUT_INPUT );
	mPinTrigger = pinInput( "Trigger", PID_FUGIO_NODE_TRIGGER );

	mPinOutput = pinOutput( "Output", PIN_OUTPUT_OUTPUT );
}

void RateControlNode::inputsUpdated( qint64 pTimeStamp )
{
	if( !pTimeStamp )
	{
		return;
	}

	if( mPinInput->isConnected() && mPinOutput->control().isNull() )
	{
		mPinOutput->setControl( mPinInput->connectedPin()->control() );

		pinUpdated( mPinOutput );
	}
	else if( !mPinInput->isConnected() && !mPinOutput->control().isNull() )
	{
		mPinOutput->setControl( QSharedPointer<fugio::PinControlInterface>() );

		pinUpdated( mPinOutput );
	}

	if( mPinInput->isConnectedToActiveNode() && mPinInput->updated() > mTimeInput )
	{
		mHaveInput = true;
	}

	if( mPinTrigger->updated() > mTimeTrigger )
	{
		mHaveTrigger = true;
	}

	if( mHaveInput && mHaveTrigger )
	{
		mTimeInput   = mPinInput->updated();
		mTimeTrigger = mPinTrigger->updated();

		pinUpdated( mPinOutput );

		mHaveInput = mHaveTrigger = false;
	}
}
