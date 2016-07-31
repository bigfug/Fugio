#include "everynode.h"

#include <QDateTime>
#include <QDebug>

#include <fugio/node_interface.h>
#include <fugio/context_interface.h>
#include <fugio/context_signals.h>
#include <fugio/core/uuid.h>
#include <fugio/core/variant_interface.h>

EveryNode::EveryNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mLastTime( -1 )
{
	mPinNumber = pinInput( "Number" );

	mPinNumber->setValue( 1000 );

	pinOutput<fugio::PinControlInterface *>( "Trigger", mPinTrigger, PID_TRIGGER );
}

bool EveryNode::initialise( void )
{
	if( !NodeControlBase::initialise() )
	{
		return( false );
	}

	mLastTime = -1;

	connect( mNode->context()->qobject(), SIGNAL(frameStart(qint64)), this, SLOT(frameStart(qint64)) );

	return( true );
}

bool EveryNode::deinitialise()
{
	mNode->context()->qobject()->disconnect( this );

	return( NodeControlBase::deinitialise() );
}

void EveryNode::frameStart( qint64 pTimeStamp )
{
	qint64			ClockTime = pTimeStamp;

	if( mLastTime < 0 || mLastTime > ClockTime )
	{
		mLastTime = ClockTime;

		return;
	}

	qint64			TriggerTime = variant( mPinNumber ).toInt();

	if( TriggerTime <= 0 )
	{
		return;
	}

	if( ClockTime - mLastTime < TriggerTime )
	{
		return;
	}

	// here we have two options, either use the current time as the trigger time,
	// which will possibly result in missed triggers, or increment the last time,
	// which might cause a whole bunch of triggers if there's been a clock delay

	// instead. let's check the ClockTime isn't TriggerTime * 2 ahead

	if( ClockTime - mLastTime - TriggerTime >= TriggerTime )
	{
		mLastTime = ClockTime;
	}
	else
	{
		mLastTime += TriggerTime;
	}

	mNode->context()->pinUpdated( mPinTrigger );
}
