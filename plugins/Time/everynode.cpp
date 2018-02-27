#include "everynode.h"

#include <QDateTime>
#include <QDebug>

#include <fugio/node_interface.h>
#include <fugio/context_interface.h>
#include <fugio/context_signals.h>
#include <fugio/core/uuid.h>
#include <fugio/core/variant_interface.h>

EveryNode::EveryNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mTimer( nullptr )
{
	FUGID( PIN_INPUT_NUMBER, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_OUTPUT_TRIGGER, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );

	mPinNumber = pinInput( "Milliseconds", PIN_INPUT_NUMBER );

	mPinNumber->setValue( 1000 );

	pinOutput<fugio::PinControlInterface *>( "Trigger", mPinTrigger, PID_TRIGGER, PIN_OUTPUT_TRIGGER );
}

bool EveryNode::initialise()
{
	if( !NodeControlBase::initialise() )
	{
		return( false );
	}

	mTimer = new QTimer( this );

	if( mTimer )
	{
		connect( mTimer, &QTimer::timeout, this, &EveryNode::timeout );
	}

	return( true );
}

bool EveryNode::deinitialise()
{
	if( mTimer )
	{
		delete mTimer;

		mTimer = nullptr;
	}

	return( NodeControlBase::deinitialise() );
}

void EveryNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	if( mTimer )
	{
		if( mTimer->isActive() )
		{
			pinUpdated( mPinTrigger );
		}

		int			Interval = variant<int>( mPinNumber );

		if( Interval != mTimer->interval() )
		{
			mTimer->stop();

			if( Interval > 0 )
			{
				mTimer->start( Interval );
			}
		}
	}
}

void EveryNode::timeout()
{
	mNode->context()->updateNode( mNode );

	mNode->context()->global()->scheduleFrame();
}

