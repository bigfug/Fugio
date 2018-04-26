#include "processclicknode.h"

#include <QApplication>

#include <fugio/core/uuid.h>

ProcessClickNode::ProcessClickNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mLastClick( -1 ), mLastValue( false )
{
	FUGID( PIN_INPUT_BOOLEAN, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_OUTPUT_DOWN, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );
	FUGID( PIN_OUTPUT_UP, "261cc653-d7fa-4c34-a08b-3603e8ae71d5" );
	FUGID( PIN_OUTPUT_DOUBLE_CLICK, "249f2932-f483-422f-b811-ab679f006381" );

	mPinInputBoolean = pinInput( tr( "Boolean" ), PIN_INPUT_BOOLEAN );

	mPinOutputDown = pinOutput( tr( "Down" ), PIN_OUTPUT_DOWN );

	mPinOutputUp = pinOutput( tr( "Up" ), PIN_OUTPUT_UP );

	mPinOutputDoubleClick = pinOutput( tr( "Double" ), PIN_OUTPUT_DOUBLE_CLICK );
}

void ProcessClickNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	bool	NextValue = variant<bool>( mPinInputBoolean );

	if( NextValue && !mLastValue )
	{
		pinUpdated( mPinOutputDown );

		if( mLastClick >= 0 && pTimeStamp - mLastClick < QApplication::doubleClickInterval() )
		{
			pinUpdated( mPinOutputDoubleClick );

			mLastClick = -1;
		}
		else
		{
			mLastClick = pTimeStamp;
		}
	}

	if( !NextValue && mLastValue )
	{
		pinUpdated( mPinOutputUp );
	}

	mLastValue = NextValue;
}
