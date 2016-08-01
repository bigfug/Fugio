#include "minmaxnode.h"

#include <fugio/core/uuid.h>

MinMaxNode::MinMaxNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mMin( -1 ), mMax( -1 )
{
	FUGID( PIN_INPUT_VALUE, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_INPUT_RESET, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );
	FUGID( PIN_OUTPUT_OUTPUT, "261cc653-d7fa-4c34-a08b-3603e8ae71d5" );
	FUGID( PIN_OUTPUT_MIN, "249f2932-f483-422f-b811-ab679f006381" );
	FUGID( PIN_OUTPUT_MAX, "ce8d578e-c5a4-422f-b3c4-a1bdf40facdb" );

	mPinInputValue = pinInput( "Input", PIN_INPUT_VALUE );

	mPinInputReset = pinInput( "Reset", PIN_INPUT_RESET );

	mValOutputOutput = pinOutput<fugio::VariantInterface *>( "Output", mPinOutputOutput, PID_FLOAT, PIN_OUTPUT_OUTPUT );

	mValOutputMin = pinOutput<fugio::VariantInterface *>( "Min", mPinOutputMin, PID_FLOAT, PIN_OUTPUT_MIN );

	mValOutputMax = pinOutput<fugio::VariantInterface *>( "Max", mPinOutputMax, PID_FLOAT, PIN_OUTPUT_MAX );
}

void MinMaxNode::inputsUpdated( qint64 pTimeStamp )
{
	bool		OK;
	float		Value = variant( mPinInputValue ).toFloat( &OK );

	if( !pTimeStamp || mPinInputReset->isUpdated( pTimeStamp ) )
	{
		mMin = mMax = -1;
	}

	if( !OK )
	{
		return;
	}

	if( mMin == -1 || Value < mMin )
	{
		mMin = Value;
	}

	if( mMax == -1 || Value > mMax )
	{
		mMax = Value;
	}

	if( mMin != mValOutputMin->variant().toFloat() )
	{
		mValOutputMin->setVariant( mMin );

		pinUpdated( mPinOutputMin );
	}

	if( mMax != mValOutputMax->variant().toFloat() )
	{
		mValOutputMax->setVariant( mMax );

		pinUpdated( mPinOutputMax );
	}

	float	OutVal = qBound( mMin, Value, mMax );
	float	OutRng = mMax - mMin;

	if( OutRng )
	{
		OutVal = ( OutVal - mMin ) / OutRng;
	}
	else
	{
		OutVal = 0;
	}

	//qDebug() << Value << mMin << mMax << OutRng << OutVal;

	if( OutVal != mValOutputOutput->variant().toFloat() )
	{
		mValOutputOutput->setVariant( OutVal );

		pinUpdated( mPinOutputOutput );
	}
}
