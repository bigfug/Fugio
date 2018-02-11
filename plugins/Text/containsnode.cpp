#include "containsnode.h"

#include <fugio/core/uuid.h>

#include <fugio/pin_variant_iterator.h>

ContainsNode::ContainsNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_SEARCH, "c6b773cb-791e-431e-a265-7ec3cdd6cc46" );
	FUGID( PIN_INPUT_VALUE, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_OUTPUT_VALUE, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );

	mPinInputSearch = pinInput( tr( "Search" ), PIN_INPUT_SEARCH );

	mPinInput = pinInput( tr( "String" ), PIN_INPUT_VALUE );

	mValOutput = pinOutput<fugio::VariantInterface *>( "Found", mPinOutput, PID_BOOL, PIN_OUTPUT_VALUE );

	mPinInput->setAutoRename( true );

	mNode->pairPins( mPinInput, mPinOutput );
}

void ContainsNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	QString	Search = variant<QString>( mPinInputSearch );

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
			bool		NewVar = SrcVar.index( i ).toString().contains( Search );

			if( DstVar->variant( i ).toBool() != NewVar )
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

QUuid ContainsNode::pairedPinControlUuid( QSharedPointer<fugio::PinInterface> pPin ) const
{
	return( pPin->direction() == PIN_INPUT ? PID_BOOL : PID_STRING );
}

QList<QUuid> ContainsNode::pinAddTypesInput() const
{
	static QList<QUuid>	PinLst =
	{
		PID_STRING
	};

	return( PinLst );
}

bool ContainsNode::canAcceptPin( fugio::PinInterface *pPin ) const
{
	return( pPin->direction() == PIN_OUTPUT );
}
