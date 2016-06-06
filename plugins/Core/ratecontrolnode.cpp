#include "ratecontrolnode.h"

#include <fugio/core/uuid.h>

#include <fugio/context_interface.h>

RateControlNode::RateControlNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mHaveInput( false ), mHaveTrigger( false ), mTimeInput( 0 ), mTimeTrigger( 0 )
{
	mPinInput   = pinInput( "Input" );
	mPinTrigger = pinInput( "Trigger" );

	mPinOutput = mNode->createPin( "Output", PIN_OUTPUT, next_uuid() );
}

void RateControlNode::inputsUpdated( qint64 pTimeStamp )
{
	Q_UNUSED( pTimeStamp )

	if( mPinInput->isConnected() && mPinOutput->control().isNull() )
	{
		mPinOutput->setControl( mPinInput->connectedPin()->control() );

		mNode->context()->pinUpdated( mPinOutput );
	}
	else if( !mPinInput->isConnected() && !mPinOutput->control().isNull() )
	{
		mPinOutput->setControl( QSharedPointer<fugio::PinControlInterface>() );

		mNode->context()->pinUpdated( mPinOutput );
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

		mNode->context()->pinUpdated( mPinOutput );

		mHaveInput = mHaveTrigger = false;
	}
}
