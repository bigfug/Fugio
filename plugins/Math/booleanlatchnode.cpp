#include "booleanlatchnode.h"

#include <fugio/core/uuid.h>

#include <fugio/pin_variant_iterator.h>

BooleanLatchNode::BooleanLatchNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_RESET, "c6b773cb-791e-431e-a265-7ec3cdd6cc46" );
	FUGID( PIN_INPUT_VALUE, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_OUTPUT_VALUE, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );

	mPinInputReset = pinInput( tr( "Reset" ), PIN_INPUT_RESET );
	mPinInput = pinInput( tr( "Trigger" ), PIN_INPUT_VALUE );

	mValOutput = pinOutput<fugio::VariantInterface *>( "Boolean", mPinOutput, PID_BOOL, PIN_OUTPUT_VALUE );

	mPinInput->setAutoRename( true );

	mNode->pairPins( mPinInput, mPinOutput );
}

void BooleanLatchNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	const bool	ResetPins = mPinInputReset->isUpdated( pTimeStamp );

	for( fugio::NodeInterface::UuidPair UP : mNode->pairedPins() )
	{
		QSharedPointer<fugio::PinInterface>  SrcPin = mNode->findPinByLocalId( UP.first );
		QSharedPointer<fugio::PinInterface>  DstPin = mNode->findPinByLocalId( UP.second );
		fugio::VariantInterface				*DstVar = qobject_cast<fugio::VariantInterface *>( DstPin && DstPin->hasControl() ? DstPin->control()->qobject() : nullptr );

		if( !DstVar )
		{
			continue;
		}

		bool		CurVal = DstVar->variant().toBool();
		bool		NewVal = ( ResetPins ? false : ( SrcPin->isUpdated( pTimeStamp ) | CurVal  ));

		if( CurVal != NewVal )
		{
			DstVar->setVariant( NewVal );

			pinUpdated( DstPin );
		}
	}
}

QUuid BooleanLatchNode::pairedPinControlUuid( QSharedPointer<fugio::PinInterface> pPin ) const
{
	return( pPin->direction() == PIN_INPUT ? PID_BOOL : PID_TRIGGER );
}

QList<QUuid> BooleanLatchNode::pinAddTypesInput() const
{
	return( QList<QUuid>() );
}

bool BooleanLatchNode::canAcceptPin( fugio::PinInterface *pPin ) const
{
	return( pPin->direction() == PIN_OUTPUT );
}
