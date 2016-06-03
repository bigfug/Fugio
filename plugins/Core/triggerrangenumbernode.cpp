#include "triggerrangenumbernode.h"

#include <QDebug>

#include <fugio/core/uuid.h>

#include <fugio/context_interface.h>
#include <fugio/core/variant_interface.h>

TriggerRangeNumberNode::TriggerRangeNumberNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mPinIdx( -1 )
{
	mValOutput = pinOutput<fugio::VariantInterface *>( "Number", mPinOutput, PID_FLOAT );
}

void TriggerRangeNumberNode::inputsUpdated( qint64 pTimeStamp )
{
	if( !pTimeStamp )
	{
		return;
	}

	int		PinCnt = mNode->enumInputPins().size();
	int		PinVal = mPinIdx;

	for( QSharedPointer<fugio::PinInterface> P : mNode->enumInputPins() )
	{
		if( P->isUpdated( pTimeStamp ) )
		{
			//qDebug() << P->order();

			PinVal = P->order();

			break;
		}
	}

	if( PinVal == mPinIdx )
	{
		return;
	}

	mValOutput->setVariant( double( PinVal ) / double( PinCnt - 1 ) );

	pinUpdated( mPinOutput );

	mPinIdx = PinVal;
}

QList<QUuid> TriggerRangeNumberNode::pinAddTypesInput() const
{
	return( mNode->context()->global()->pinIds().keys() );
}

bool TriggerRangeNumberNode::canAcceptPin( fugio::PinInterface *pPin ) const
{
	return( pPin->direction() == PIN_OUTPUT );
}
