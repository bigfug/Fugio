#include "localtimenode.h"

#include <fugio/core/uuid.h>
#include <fugio/time/uuid.h>
#include <fugio/context_signals.h>

#include <QDateTime>

LocalTimeNode::LocalTimeNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_OUTPUT_TIME, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );

	mPinInputTrigger = pinInput( tr( "Trigger" ), PID_FUGIO_NODE_TRIGGER );

	mValOutputTime = pinOutput<fugio::VariantInterface*>( tr( "DateTime" ), mPinOutputTime, PID_DATETIME, PIN_OUTPUT_TIME );
}

bool LocalTimeNode::initialise()
{
	if( !NodeControlBase::initialise() )
	{
		return( false );
	}

	connect( mNode->context()->qobject(), SIGNAL(frameStart()), this, SLOT(contextFrameStart()) );

	return( true );
}

bool LocalTimeNode::deinitialise()
{
	disconnect( mNode->context()->qobject(), SIGNAL(frameStart()), this, SLOT(contextFrameStart()) );

	return( NodeControlBase::deinitialise() );
}

void LocalTimeNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	updateTime();
}

void LocalTimeNode::contextFrameStart()
{
	if( !mPinInputTrigger->isConnected() )
	{
		updateTime();
	}
}

void LocalTimeNode::updateTime()
{
	QDateTime		LocalTime = QDateTime::currentDateTime();

	if( mValOutputTime->variant().toDateTime() != LocalTime )
	{
		mValOutputTime->setVariant( LocalTime );

		pinUpdated( mPinOutputTime );
	}
}
