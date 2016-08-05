#include "framedelaynode.h"

#include <fugio/core/uuid.h>
#include <fugio/pin_control_interface.h>

#include <fugio/context_interface.h>
#include <fugio/context_signals.h>

FrameDelayNode::FrameDelayNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mSignalOutput( false )
{
	FUGID( PID_INPUT,	"fff9cb4f-48c5-48fa-818d-ca7b0af19bb8" );
	FUGID( PID_OUTPUT,	"83f4fd43-5c0a-4889-88dd-5cbad137a3cd" );

	mPinInput  = pinInput( "Input", PID_INPUT );

	mPinOutput = pinOutput( "Output", PID_OUTPUT );

	mPinInput->setAutoRename( true );

	mNode->pairPins( mPinInput, mPinOutput );
}

void FrameDelayNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	if( mPinOutput->isConnected() && mPinOutput->connectedPin()->hasControl() )
	{
		fugio::PinControlInterface	*PinCtlInf = output<fugio::PinControlInterface *>( mPinOutput );

		if( mPinInput->hasControl() && mPinInput->control()->qobject() != PinCtlInf->qobject() )
		{
			mPinInput->setControl( QSharedPointer<fugio::PinControlInterface>() );
		}

		if( !mPinInput->hasControl() )
		{
			mPinInput->setControl( mPinOutput->connectedPin()->control() );
		}
	}
	else if( mPinInput->hasControl() )
	{
		mPinInput->setControl( QSharedPointer<fugio::PinControlInterface>() );
	}

	if( mPinInput->isConnectedToActiveNode() && mPinInput->connectedPin()->hasControl() )
	{
		fugio::PinControlInterface	*PinCtlInf = input<fugio::PinControlInterface *>( mPinInput );

		if( mPinOutput->hasControl() && mPinOutput->control()->qobject() != PinCtlInf->qobject() )
		{
			mPinOutput->setControl( QSharedPointer<fugio::PinControlInterface>() );
		}

		if( !mPinOutput->hasControl() )
		{
			mPinOutput->setControl( mPinInput->connectedPin()->control() );

			mSignalOutput = true;
		}
		else if( mPinInput->isUpdated( pTimeStamp ) )
		{
			mSignalOutput = true;
		}
	}
	else if( mPinOutput->hasControl() )
	{
		mPinOutput->setControl( QSharedPointer<fugio::PinControlInterface>() );
	}
	else if( mPinInput->isUpdated( pTimeStamp ) )
	{
		mSignalOutput = true;
	}
}

void FrameDelayNode::contextFrameStart()
{
	if( mSignalOutput )
	{
		pinUpdated( mPinOutput );

		mSignalOutput = false;
	}
}

bool FrameDelayNode::initialise()
{
	if( !NodeControlBase::initialise() )
	{
		return( false );
	}

	connect( mNode->context()->qobject(), SIGNAL(frameStart()), this, SLOT(contextFrameStart()) );

	return( true );
}

bool FrameDelayNode::deinitialise()
{
	disconnect( mNode->context()->qobject(), SIGNAL(frameStart()), this, SLOT(contextFrameStart()) );

	return( NodeControlBase::deinitialise() );
}

bool FrameDelayNode::pinShouldAutoRename( fugio::PinInterface *pPin ) const
{
	return( pPin->direction() == PIN_INPUT );
}
