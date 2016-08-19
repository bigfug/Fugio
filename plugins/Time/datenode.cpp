#include "datenode.h"

#include <fugio/global_interface.h>
#include <fugio/context_interface.h>
#include <fugio/context_signals.h>

#include <QSettings>
#include <QDate>

DateNode::DateNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_OUTPUT_DAY, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_OUTPUT_DAY_OF_WEEK, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );
	FUGID( PIN_OUTPUT_DAY_OF_YEAR, "261cc653-d7fa-4c34-a08b-3603e8ae71d5" );
	FUGID( PIN_OUTPUT_MONTH, "249f2932-f483-422f-b811-ab679f006381" );
	FUGID( PIN_OUTPUT_YEAR, "ce8d578e-c5a4-422f-b3c4-a1bdf40facdb" );
	FUGID( PIN_OUTPUT_WEEK_NUMBER, "e6bf944e-5f46-4994-bd51-13c2aa6415b7" );

	mValOutputDay = pinOutput<fugio::VariantInterface *>( "Day", mPinOutputDay, PID_INTEGER, PIN_OUTPUT_DAY );
	mValOutputDayOfWeek = pinOutput<fugio::VariantInterface *>( "Day of Week", mPinOutputDayOfWeek, PID_INTEGER, PIN_OUTPUT_DAY_OF_WEEK );
	mValOutputDayOfYear = pinOutput<fugio::VariantInterface *>( "Day of Year", mPinOutputDayOfYear, PID_INTEGER, PIN_OUTPUT_DAY_OF_YEAR );
	mValOutputMonth = pinOutput<fugio::VariantInterface *>( "Month", mPinOutputMonth, PID_INTEGER, PIN_OUTPUT_MONTH );
	mValOutputYear = pinOutput<fugio::VariantInterface *>( "Year", mPinOutputYear, PID_INTEGER, PIN_OUTPUT_YEAR );
	mValOutputWeekNumber = pinOutput<fugio::VariantInterface *>( "Week Number", mPinOutputWeekNumber, PID_INTEGER, PIN_OUTPUT_WEEK_NUMBER );
}

bool DateNode::initialise( void )
{
	if( !NodeControlBase::initialise() )
	{
		return( false );
	}

	connect( mNode->context()->qobject(), SIGNAL(frameStart(qint64)), this, SLOT(contextFrameStart(qint64)) );

	return( true );
}

bool DateNode::deinitialise( void )
{
	disconnect( mNode->context()->qobject(), SIGNAL(frameStart(qint64)), this, SLOT(contextFrameStart(qint64)) );

	return( NodeControlBase::deinitialise() );
}

void DateNode::contextFrameStart( qint64 pTimeStamp )
{
	Q_UNUSED( pTimeStamp )

	QDate		CurrDate = QDate::currentDate();

	if( mValOutputDay->variant().toInt() != CurrDate.day() )
	{
		mValOutputDay->setVariant( CurrDate.day() );

		pinUpdated( mPinOutputDay );
	}

	if( mValOutputDayOfWeek->variant().toInt() != CurrDate.dayOfWeek() )
	{
		mValOutputDayOfWeek->setVariant( CurrDate.dayOfWeek() );

		pinUpdated( mPinOutputDayOfWeek );
	}

	if( mValOutputDayOfYear->variant().toInt() != CurrDate.dayOfYear() )
	{
		mValOutputDayOfYear->setVariant( CurrDate.dayOfYear() );

		pinUpdated( mPinOutputDayOfYear );
	}

	if( mValOutputMonth->variant().toInt() != CurrDate.month() )
	{
		mValOutputMonth->setVariant( CurrDate.month() );

		pinUpdated( mPinOutputMonth );
	}

	if( mValOutputYear->variant().toInt() != CurrDate.year() )
	{
		mValOutputYear->setVariant( CurrDate.year() );

		pinUpdated( mPinOutputYear );
	}

	if( mValOutputWeekNumber->variant().toInt() != CurrDate.weekNumber() )
	{
		mValOutputWeekNumber->setVariant( CurrDate.weekNumber() );

		pinUpdated( mPinOutputWeekNumber );
	}
}
