#include "timenode.h"

#include <fugio/global_interface.h>
#include <fugio/context_interface.h>
#include <fugio/context_signals.h>

#include <QSettings>
#include <QDateTime>

TimeNode::TimeNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mLastUpdate( 0 )
{
	FUGID( PIN_INPUT_INTERVAL, "51297977-7b4b-4e08-9dea-89a8add4abe0" );
	FUGID( PIN_OUTPUT_CONTEXT_MS, "261cc653-d7fa-4c34-a08b-3603e8ae71d5" );
	FUGID( PIN_OUTPUT_CONTEXT_S, "249f2932-f483-422f-b811-ab679f006381" );

	mPinInputInterval = pinInput( "Interval (Ms)", PIN_INPUT_INTERVAL );

	mPinInputInterval->setValue( 40 );

	mValOutputContextTimeMs = pinOutput<fugio::VariantInterface *>( "Milliseconds", mPinOutputContextTimeMs, PID_INTEGER, PIN_OUTPUT_CONTEXT_MS );
	mValOutputContextTimeS  = pinOutput<fugio::VariantInterface *>( "Seconds", mPinOutputContextTimeS, PID_FLOAT, PIN_OUTPUT_CONTEXT_S );

	mPinOutputContextTimeMs->setDescription( tr( "The number of milliseconds since this context started" ) );
	mPinOutputContextTimeS->setDescription( tr( "The number of seconds since this context started" ) );
}

bool TimeNode::initialise( void )
{
	if( !NodeControlBase::initialise() )
	{
		return( false );
	}

	connect( mNode->context()->qobject(), SIGNAL(frameStart(qint64)), this, SLOT(contextFrameStart(qint64)) );

	return( true );
}

bool TimeNode::deinitialise( void )
{
	disconnect( mNode->context()->qobject(), SIGNAL(frameStart(qint64)), this, SLOT(contextFrameStart(qint64)) );

	return( NodeControlBase::deinitialise() );
}

void TimeNode::contextFrameStart( qint64 pTimeStamp )
{
	qint64			Interval  = qMax( 1, variant( mPinInputInterval ).toInt() );

	if( !mLastUpdate )
	{
		mLastUpdate = pTimeStamp - Interval;
	}

	if( mLastUpdate + Interval > pTimeStamp )
	{
		return;
	}

	mValOutputContextTimeMs->setVariant( pTimeStamp );
	mValOutputContextTimeS->setVariant( double( pTimeStamp ) / 1000.0 );

	pinUpdated( mPinOutputContextTimeMs );
	pinUpdated( mPinOutputContextTimeS );

	mLastUpdate += Interval;
}
