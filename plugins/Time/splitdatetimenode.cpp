#include "splitdatetimenode.h"

#include <fugio/time/uuid.h>

#include <QDateTime>
#include <QDate>
#include <QTime>

SplitDateTimeNode::SplitDateTimeNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_DATETIME, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_OUTPUT_DATE, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );
	FUGID( PIN_OUTPUT_TIME, "261cc653-d7fa-4c34-a08b-3603e8ae71d5" );

	mPinInputDateTime = pinInput( "DateTime", PIN_INPUT_DATETIME );

	mValOutputDate = pinOutput<fugio::VariantInterface *>( "Date", mPinOutputDate, PID_DATE, PIN_OUTPUT_DATE );

	mValOutputTime = pinOutput<fugio::VariantInterface *>( "Time", mPinOutputTime, PID_TIME, PIN_OUTPUT_TIME );

	mPinInputDateTime->registerPinInputType( PID_DATETIME );
}

void SplitDateTimeNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	QDateTime		DT = variant( mPinInputDateTime ).toDateTime();
	QDate			D  = DT.date();
	QTime			T  = DT.time();

	if( mValOutputDate->variant().toDate() != D )
	{
		mValOutputDate->setVariant( D );

		pinUpdated( mPinOutputDate );
	}

	if( mValOutputTime->variant().toTime() != T )
	{
		mValOutputTime->setVariant( T );

		pinUpdated( mPinOutputTime );
	}
}
