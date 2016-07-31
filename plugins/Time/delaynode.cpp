#include "delaynode.h"

#include <QDateTime>
#include <QDebug>

#include <fugio/node_interface.h>
#include <fugio/context_interface.h>
#include <fugio/context_signals.h>
#include <fugio/core/uuid.h>
#include <fugio/core/variant_interface.h>

DelayNode::DelayNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mTriggerTime( -1 )
{
	mPinNumber = pinInput( "Number" );

	pinOutput<fugio::PinControlInterface *>( "Trigger", mPinTrigger, PID_TRIGGER );
}

void DelayNode::inputsUpdated( qint64 pTimeStamp )
{
	mTriggerTime = pTimeStamp + variant( mPinNumber ).toInt();

	if( mTriggerTime > pTimeStamp )
	{
		connect( mNode->context()->qobject(), SIGNAL(frameStart(qint64)), this, SLOT(frameStart(qint64)) );
	}
}

void DelayNode::frameStart( qint64 pTimeStamp )
{
	if( mTriggerTime < 0 || pTimeStamp < mTriggerTime )
	{
		return;
	}

	mNode->context()->pinUpdated( mPinTrigger );

	disconnect( mNode->context()->qobject(), SIGNAL(frameStart(qint64)), this, SLOT(frameStart(qint64)) );
}
