#include "outputrangenode.h"

#include <fugio/core/uuid.h>

#include <fugio/context_interface.h>
#include <fugio/core/variant_interface.h>

OutputRangeNode::OutputRangeNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mPinCnt( 0 ), mPinIdx( -1 )
{
	FUGID( PIN_INPUT_SOURCE, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_INPUT_RANGE, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );
	FUGID( PIN_INPUT_INDEX, "261cc653-d7fa-4c34-a08b-3603e8ae71d5" );
	FUGID( PIN_OUTPUT_OUTPUT, "c997473a-2016-466b-9128-beacb99870a2" );

	mPinInputSource  = pinInput( "Input", PIN_INPUT_SOURCE );

	mPinInputRange = pinInput( "Number", PIN_INPUT_RANGE );

	mPinInputIndex = pinInput( "Index", PIN_INPUT_INDEX );

	QSharedPointer<fugio::PinInterface> P = pinOutput( "Output", PIN_OUTPUT_OUTPUT );

	P->setAutoRename( true );
}

void OutputRangeNode::inputsUpdated( qint64 pTimeStamp )
{
	QList< QSharedPointer<fugio::PinInterface> >	PinLst = mNode->enumOutputPins();

	int						 PinCnt = PinLst.count();

	if( PinCnt != mPinCnt )
	{
		mPidIdx = -1;

		mPinCnt = PinCnt;
	}

	if( !mPinCnt || !mPinInputSource->isConnectedToActiveNode() )
	{
		return;
	}

	int						 PinIdx = mPinIdx;

	if( mPinInputRange->isUpdated( pTimeStamp ) )
	{
		PinIdx = qBound( 0.0, variant( mPinInputRange ).toDouble(), 1.0 ) * double( PinCnt );
	}

	if( mPinInputIndex->isUpdated( pTimeStamp ) )
	{
		PinIdx = variant( mPinInputIndex ).toInt();
	}

	PinIdx = qBound( 0, PinIdx, PinCnt - 1 );

	if( PinIdx == mPidIdx && !mPinInputSource->isUpdated( pTimeStamp ) )
	{
		return;
	}

	for( int i = 0 ; i < PinLst.count() ; i++ )
	{
		QSharedPointer<fugio::PinInterface>	P = PinLst[ i ];

		if( P->order() == PinIdx )
		{
			P->setControl( mPinInputSource->connectedPin()->control() );

			pinUpdated( P );
		}
		else
		{
			P->setControl( QSharedPointer<fugio::PinControlInterface>() );
		}
	}

	mPidIdx = PinIdx;
}

bool OutputRangeNode::canAcceptPin( fugio::PinInterface *pPin ) const
{
	return( pPin->direction() == PIN_INPUT );
}

bool OutputRangeNode::pinShouldAutoRename( fugio::PinInterface *pPin ) const
{
	return( pPin->direction() == PIN_OUTPUT );
}
