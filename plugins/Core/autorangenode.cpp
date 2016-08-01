#include "autorangenode.h"
#include <fugio/context_interface.h>
#include <fugio/context_signals.h>

AutoRangeNode::AutoRangeNode(QSharedPointer<fugio::NodeInterface> pNode)
	: NodeControlBase( pNode ), mValueAdded( false ), mCurVal( 0 ), mCurMax( 1 ), mCurMin( 0 ), mLastTime( -1 )
{
	mPinInput = pinInput( "Input" );

	mValOutput = pinOutput<fugio::VariantInterface *>( "Output", mPinOutput, PID_FLOAT );

	mValMin = pinOutput<fugio::VariantInterface *>( "Min", mPinMin, PID_FLOAT );

	mValMax = pinOutput<fugio::VariantInterface *>( "Max", mPinMax, PID_FLOAT );
}

bool AutoRangeNode::initialise()
{
	if( !fugio::NodeControlBase::initialise() )
	{
		return( false );
	}

	connect( mNode->context()->qobject(), SIGNAL(frameStart()), this, SLOT(contextFrameStart()) );
	connect( mNode->context()->qobject(), SIGNAL(frameProcess(qint64)), this, SLOT(contextFrameProcess(qint64)) );

	return( true );
}

bool AutoRangeNode::deinitialise()
{
	disconnect( mNode->context()->qobject(), SIGNAL(frameStart()), this, SLOT(contextFrameStart()) );
	disconnect( mNode->context()->qobject(), SIGNAL(frameProcess(qint64)), this, SLOT(contextFrameProcess(qint64)) );

	return( NodeControlBase::deinitialise() );
}

void AutoRangeNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	bool	OK;

	mCurVal = variant( mPinInput ).toDouble( &OK );

	if( OK )
	{
		mValueAdded = true;
	}

	if( mValueAdded )
	{
		updateRange( pTimeStamp );
	}
}

void AutoRangeNode::contextFrameStart()
{
	mValueAdded = false;
}

void AutoRangeNode::contextFrameProcess(qint64 pTimeStamp)
{
	if( !mValueAdded )
	{
		updateRange( pTimeStamp );
	}
}

void AutoRangeNode::updateRange( qint64 pCurrTime )
{
	static double	MIN_INC = 0.75;
	static double	MAX_INC = 0.75;
	static double	MIN_DEC = 0.01;
	static double	MAX_DEC = 0.01;

	if( mLastTime < 0 )
	{
		if( mValueAdded )
		{
			mLastTime = pCurrTime;

			mCurMin = mCurMax = mCurVal;

			mValMin->setVariant( mCurMin );
			mValMax->setVariant( mCurMax );

			double	OutVal = qBound( mCurMin, mCurVal, mCurMax );
			double	OutRng = mCurMax - mCurMin;

			if( OutRng != 0 )
			{
				OutVal = ( OutVal - mCurMin ) / OutRng;
			}

			mValOutput->setVariant( OutVal );

			pinUpdated( mPinMin );
			pinUpdated( mPinMax );
			pinUpdated( mPinOutput );
		}

		return;
	}

	const qint64	TimeDiff = pCurrTime - mLastTime;
	const double	TmeScl = double( TimeDiff ) / 1000;

	if( mCurVal > mCurMax )
	{
		mCurMax = qMin( mCurVal, mCurMax + ( ( mCurVal - mCurMax ) * MAX_INC * TmeScl ) );
	}

	if( mCurVal < mCurMin )
	{
		mCurMin = qMax( mCurVal, mCurMin - ( ( mCurMin - mCurVal ) * MIN_INC * TmeScl ) );
	}

	if( mCurMax > mCurVal )
	{
		mCurMax = qMax( mCurVal, mCurMax - ( ( mCurMax - mCurVal ) * MAX_DEC * TmeScl ) );
	}

	if( mCurMin < mCurVal )
	{
		mCurMin = qMin( mCurVal, mCurMin + ( ( mCurVal - mCurMin ) * MIN_DEC * TmeScl ) );
	}

	double	OutVal = qBound( mCurMin, mCurVal, mCurMax );
	double	OutRng = mCurMax - mCurMin;

	if( OutRng != 0 )
	{
		OutVal = ( OutVal - mCurMin ) / OutRng;
	}

	if( mValMin->variant().toDouble() != mCurMin )
	{
		mValMin->setVariant( mCurMin );

		pinUpdated( mPinMin );
	}

	if( mValMax->variant().toDouble() != mCurMax )
	{
		mValMax->setVariant( mCurMax );

		pinUpdated( mPinMax );
	}

	if( mValOutput->variant().toDouble() != OutVal )
	{
		mValOutput->setVariant( OutVal );

		pinUpdated( mPinOutput );
	}

	mLastTime = pCurrTime;
}

