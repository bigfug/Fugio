#include "datetimetostringnode.h"

#include <QTime>
#include <QDate>
#include <QDateTime>

#include <fugio/core/uuid.h>
#include <fugio/time/uuid.h>

#include "timepin.h"

DateTimeToStringNode::DateTimeToStringNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_FORMAT, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_INPUT_TIME, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );
	FUGID( PIN_OUTPUT_STRING, "261cc653-d7fa-4c34-a08b-3603e8ae71d5" );

	pinInput( tr( "Trigger" ), PID_FUGIO_NODE_TRIGGER );

	mPinInputFormat = pinInput( tr( "Format" ), PIN_INPUT_FORMAT );

	mPinInputInput   = pinInput( tr( "Input" ), PIN_INPUT_TIME );

	mValOutputString = pinOutput<fugio::VariantInterface *>( tr( "String" ), mPinOutputString, PID_STRING, PIN_OUTPUT_STRING );
}

void DateTimeToStringNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	const QString	Format    = variant( mPinInputFormat ).toString();

	QVariant		Input = variant( mPinInputInput );

	QString			Output;

	switch( QMetaType::Type( Input.type() ) )
	{
		case QMetaType::QTime:
			{
				QTime	V = Input.toTime();

				if( Format.isEmpty() )
				{
					Output = V.toString();
				}
				else
				{
					Output = V.toString( Format );
				}
			}
			break;

		case QMetaType::QDate:
			{
				QDate	V = Input.toDate();

				if( Format.isEmpty() )
				{
					Output = V.toString();
				}
				else
				{
					Output = V.toString( Format );
				}
			}
			break;

		case QMetaType::QDateTime:
			{
				QDateTime	V = Input.toDateTime();

				if( Format.isEmpty() )
				{
					Output = V.toString();
				}
				else
				{
					Output = V.toString( Format );
				}
			}
			break;

		default:
			break;
	}

	if( mValOutputString->variant().toString() != Output )
	{
		mValOutputString->setVariant( Output );

		pinUpdated( mPinOutputString );
	}
}
