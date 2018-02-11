#include "leftnode.h"

#include <fugio/core/uuid.h>

#include <fugio/pin_variant_iterator.h>

LeftNode::LeftNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_INDEX, "c6b773cb-791e-431e-a265-7ec3cdd6cc46" );
	FUGID( PIN_INPUT_VALUE, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_OUTPUT_VALUE, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );

	mPinInputIndex = pinInput( tr( "Index" ), PIN_INPUT_INDEX );

	mPinInput = pinInput( tr( "String" ), PIN_INPUT_VALUE );

	mValOutput = pinOutput<fugio::VariantInterface *>( "String", mPinOutput, PID_STRING, PIN_OUTPUT_VALUE );

	mPinInput->setAutoRename( true );

	mNode->pairPins( mPinInput, mPinOutput );
}

void LeftNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	int		Index = variant<int>( mPinInputIndex );

	if( Index <= 0 )
	{
		Index = 0;
	}

	for( fugio::NodeInterface::UuidPair UP : mNode->pairedPins() )
	{
		QSharedPointer<fugio::PinInterface>  SrcPin = mNode->findPinByLocalId( UP.first );
		QSharedPointer<fugio::PinInterface>  DstPin = mNode->findPinByLocalId( UP.second );
		fugio::VariantInterface				*DstVar = qobject_cast<fugio::VariantInterface *>( DstPin && DstPin->hasControl() ? DstPin->control()->qobject() : nullptr );

		if( !DstVar )
		{
			continue;
		}

		fugio::PinVariantIterator			 SrcVar( SrcPin );

		bool								 DstUpd = false;

		DstVar->setVariantCount( SrcVar.count() );

		for( int i = 0 ; i < SrcVar.count() ; i++ )
		{
			QString		NewVar = SrcVar.index( i ).toString().left( Index );

			if( DstVar->variant( i ).toString() != NewVar )
			{
				DstVar->setVariant( i, NewVar );

				DstUpd = true;
			}
		}

		if( DstUpd )
		{
			pinUpdated( DstPin );
		}
	}
}

QUuid LeftNode::pairedPinControlUuid( QSharedPointer<fugio::PinInterface> pPin ) const
{
	return( pPin->direction() == PIN_INPUT ? PID_STRING : PID_STRING );
}

QList<QUuid> LeftNode::pinAddTypesInput() const
{
	static QList<QUuid>	PinLst =
	{
		PID_STRING
	};

	return( PinLst );
}

bool LeftNode::canAcceptPin( fugio::PinInterface *pPin ) const
{
	return( pPin->direction() == PIN_OUTPUT );
}
