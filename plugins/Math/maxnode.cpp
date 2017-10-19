#include "maxnode.h"

#include <fugio/core/uuid.h>

#include <fugio/context_interface.h>

MaxNode::MaxNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_VALUE, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_OUTPUT_VALUE, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );

	mPinInput = pinInput( "Number", PIN_INPUT_VALUE );

	mValOutput = pinOutput<fugio::VariantInterface *>( "Number", mPinOutput, PID_FLOAT, PIN_OUTPUT_VALUE );

	mPinInput->setAutoRename( true );
}

void MaxNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	float			OutVal = 0;
	bool			OutHas = false;

	for( QSharedPointer<fugio::PinInterface> P : mNode->enumInputPins() )
	{
		float		NewVal = qAbs( variant( P ).toFloat() );

		if( !OutHas )
		{
			OutVal = NewVal;
			OutHas = true;
		}
		else
		{
			OutVal = qMax( OutVal, NewVal );
		}
	}

	if( OutVal != mValOutput->variant().toFloat() )
	{
		mValOutput->setVariant( OutVal );

		pinUpdated( mPinOutput );
	}
}

QList<QUuid> MaxNode::pinAddTypesInput() const
{
	static QList<QUuid>		PinLst =
	{
		PID_FLOAT,
		PID_INTEGER,
		PID_STRING
	};

	return( PinLst );
}

bool MaxNode::canAcceptPin(fugio::PinInterface *pPin) const
{
	if( pPin->direction() != PIN_OUTPUT )
	{
		return( false );
	}

	return( true );
}
