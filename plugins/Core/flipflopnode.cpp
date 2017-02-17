#include "flipflopnode.h"

#include <fugio/core/uuid.h>

#include <fugio/context_interface.h>

FlipFlopNode::FlipFlopNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mState( false )
{
	FUGID( PIN_INPUT_VALUE,	"9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_INPUT_SWITCH, "5064e449-8b0b-4447-9009-c81997f754ef" );
	FUGID( PIN_OUTPUT_ONE,	"1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );
	FUGID( PIN_OUTPUT_TWO, "b13398e2-4e71-460f-a3b1-499ce9af224d" );

	mPinInputValue = pinInput( "Input", PIN_INPUT_VALUE );

	mPinInputSwitch = pinInput( "Switch", PIN_INPUT_SWITCH );

	mPinOutput1 = pinOutput( "Output 1", PIN_OUTPUT_ONE );
	mPinOutput2 = pinOutput( "Output 2", PIN_OUTPUT_TWO );
}

void FlipFlopNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	if( !pTimeStamp || mPinInputSwitch->isUpdated( pTimeStamp ) )
	{
		fugio::VariantInterface		*V = input<fugio::VariantInterface *>( mPinInputSwitch );

		if( V && V->variant().type() == QVariant::Bool )
		{
			mState = V->variant().toBool();
		}
		else if( pTimeStamp )
		{
			mState = !mState;
		}
	}

	if( !pTimeStamp || mPinInputValue->isUpdated( pTimeStamp ) )
	{
		QSharedPointer<fugio::PinControlInterface>	I;

		if( mPinInputValue->isConnected() )
		{
			I = mPinInputValue->connectedPin()->control();
		}

		if( I != mPinOutput1->control() )
		{
			mPinOutput1->setControl( I );
		}

		if( I != mPinOutput2->control() )
		{
			mPinOutput2->setControl( I );
		}
	}

	if( mPinInputValue->isUpdated( pTimeStamp ) )
	{
		if( !mState )
		{
			pinUpdated( mPinOutput1 );
		}
		else
		{
			pinUpdated( mPinOutput2 );
		}
	}
}
