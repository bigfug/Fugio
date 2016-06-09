#include "clocknode.h"

#include "fugio/global_interface.h"
#include "fugio/context_interface.h"
#include <fugio/context_signals.h>

#include <QSettings>
#include <QDateTime>

ClockNode::ClockNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	mPinInputTime = pinInput( "Time" );

	mValOutputHour24         = pinOutput<fugio::VariantInterface *>( "Hour (24h)", mPinOutputHour24, PID_INTEGER );

	mValOutputMinute         = pinOutput<fugio::VariantInterface *>( "Minute", mPinOutputMinute, PID_INTEGER );

	mValOutputSecond         = pinOutput<fugio::VariantInterface *>( "Second", mPinOutputSecond, PID_INTEGER );

	mValOutputDayMillis      = pinOutput<fugio::VariantInterface *>( "Day Ms", mPinOutputDayMillis, PID_INTEGER );

	mValOutputMillis         = pinOutput<fugio::VariantInterface *>( "Milliseconds", mPinOutputMillis, PID_INTEGER );

	mValOutputDayPosition    = pinOutput<fugio::VariantInterface *>( "Day Position", mPinOutputDayPosition, PID_FLOAT );

	mValOutputHourPosition   = pinOutput<fugio::VariantInterface *>( "Hour Position", mPinOutputHourPosition, PID_FLOAT );

	mValOutputMinutePosition = pinOutput<fugio::VariantInterface *>( "Minute Position", mPinOutputMinutePosition, PID_FLOAT );

	mValOutputSecondPosition = pinOutput<fugio::VariantInterface *>( "Second Position", mPinOutputSecondPosition, PID_FLOAT );

	mPinInputTime->setDescription( tr( "An input time, otherwise the current local time is used" ) );

	mPinOutputHour24->setDescription( tr( "The current hour (0-23)" ) );
	mPinOutputMinute->setDescription( tr( "The current minute (0-59)" ) );
	mPinOutputSecond->setDescription( tr( "The current second (0-59)" ) );
	mPinOutputDayMillis->setDescription( tr( "The number of milliseconds since the start of the current day (0-86400000)" ) );
	mPinOutputMillis->setDescription( tr( "The current milliseconds (0-999)" ) );
	mPinOutputDayPosition->setDescription( tr( "How much of the current day has elapsed (0.0-1.0)" ) );
	mPinOutputHourPosition->setDescription( tr( "How much of the current hour has elapsed (0.0-1.0)" ) );
	mPinOutputMinutePosition->setDescription( tr( "How much of the current minute has elapsed (0.0-1.0)" ) );
	mPinOutputSecondPosition->setDescription( tr( "How much of the current second has elapsed (0.0-1.0)" ) );
}

ClockNode::~ClockNode()
{

}

bool ClockNode::initialise( void )
{
	connect( mNode->context()->qobject(), SIGNAL(frameStart(qint64)), this, SLOT(frameStart(qint64)) );

	return( true );
}

bool ClockNode::deinitialise( void )
{
	disconnect( mNode->context()->qobject(), SIGNAL(frameStart(qint64)), this, SLOT(frameStart(qint64)) );

	return( true );
}

void ClockNode::frameStart( qint64 pTimeStamp )
{
	if( !mPinInputTime->isConnected() )
	{
		inputsUpdated( pTimeStamp );
	}
}

void ClockNode::inputsUpdated( qint64 pTimeStamp )
{
	qint64			ClockTime = QDateTime::currentMSecsSinceEpoch();

	if( mPinInputTime->isConnected() )
	{
		if( !mPinInputTime->isUpdated( pTimeStamp ) )
		{
			return;
		}
	}

	static const qint64		HoursInDay = 24;

	static const qint64		MinutesInHour = 60;

	static const qint64		SecondsInMinute = 60;

	static const qint64		MillisecondsInSecond = 1000;
	static const qint64		MillisecondsInMinute = MillisecondsInSecond * SecondsInMinute;
	static const qint64		MillisecondsInHour   = MillisecondsInMinute * MinutesInHour;
	static const qint64		MillisecondsInDay    = MillisecondsInHour   * HoursInDay;

	QDateTime		DateTime = QDateTime::fromMSecsSinceEpoch( ClockTime );
	QTime			Time     = DateTime.time();

	if( Time.hour() != mValOutputHour24->variant().toInt() )
	{
		mValOutputHour24->setVariant( Time.hour() );

		mNode->context()->pinUpdated( mPinOutputHour24 );
	}

	if( Time.minute() != mValOutputMinute->variant().toInt() )
	{
		mValOutputMinute->setVariant( Time.minute() );

		mNode->context()->pinUpdated( mPinOutputMinute );
	}

	if( Time.second() != mValOutputSecond->variant().toInt() )
	{
		mValOutputSecond->setVariant( Time.second() );

		mNode->context()->pinUpdated( mPinOutputSecond );
	}

	if( Time.msec() != mValOutputMillis->variant().toInt() )
	{
		mValOutputMillis->setVariant( Time.msec() );

		mNode->context()->pinUpdated( mPinOutputMillis );
	}

	mValOutputDayMillis->setVariant( Time.msecsSinceStartOfDay() );

	mValOutputDayPosition->setVariant( qreal( ClockTime % MillisecondsInDay ) / qreal( MillisecondsInDay ) );

	mValOutputHourPosition->setVariant( qreal( ClockTime % MillisecondsInHour ) / qreal( MillisecondsInHour ) );

	mValOutputMinutePosition->setVariant( qreal( ClockTime % MillisecondsInMinute ) / qreal( MillisecondsInMinute ) );

	mValOutputSecondPosition->setVariant( qreal( ClockTime % MillisecondsInSecond ) / qreal( MillisecondsInSecond ) );

	mNode->context()->pinUpdated( mPinOutputDayMillis );
	mNode->context()->pinUpdated( mPinOutputDayPosition );
	mNode->context()->pinUpdated( mPinOutputHourPosition );
	mNode->context()->pinUpdated( mPinOutputMinutePosition );
	mNode->context()->pinUpdated( mPinOutputSecondPosition );
}
