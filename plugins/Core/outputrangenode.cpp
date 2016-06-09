#include "outputrangenode.h"

#include <fugio/core/uuid.h>

#include <fugio/context_interface.h>
#include <fugio/core/variant_interface.h>

OutputRangeNode::OutputRangeNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mPinCnt( 0 ), mPinIdx( -1 )
{
	mPinInput  = pinInput( "Input" );

	mPinNumber = pinInput( "Number" );
}

void OutputRangeNode::inputsUpdated( qint64 pTimeStamp )
{
	if( !pTimeStamp )
	{
		return;
	}

	QList< QSharedPointer<fugio::PinInterface> >	PinLst = mNode->enumOutputPins();

	int						 PinIdx = -1;
	int						 PinCnt = PinLst.count();

	if( PinCnt != mPinCnt )
	{
		mPidIdx = -1;

		mPinCnt = PinCnt;
	}

	if( !mPinCnt || !mPinInput->isConnectedToActiveNode() )
	{
		return;
	}

	fugio::VariantInterface		*V;

	if( mPinNumber->isConnectedToActiveNode() && ( V = input<fugio::VariantInterface *>( mPinNumber ) ) != nullptr )
	{
		PinIdx = qBound( 0.0, V->variant().toDouble(), 1.0 ) * double( PinCnt - 1 );
	}
	else
	{
		PinIdx = qBound( 0.0, mPinNumber->value().toDouble(), 1.0 ) * double( PinCnt - 1 );
	}

	if( PinIdx == mPidIdx && !mPinInput->isUpdated( pTimeStamp ) )
	{
		return;
	}

	for( int i = 0 ; i < PinLst.count() ; i++ )
	{
		QSharedPointer<fugio::PinInterface>	P = PinLst[ i ];

		if( P->order() == PinIdx )
		{
			P->setControl( mPinInput->connectedPin()->control() );

			pinUpdated( P );
		}
		else
		{
			//P->setControl( QSharedPointer<fugio::PinControlInterface>() );

//			pinUpdated( P );
		}
	}

	mPidIdx = PinIdx;
}

bool OutputRangeNode::canAcceptPin( fugio::PinInterface *pPin ) const
{
	return( pPin->direction() == PIN_INPUT );
}
