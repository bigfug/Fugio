#include "floornode.h"

#include <fugio/core/uuid.h>

#include <fugio/context_interface.h>

FloorNode::FloorNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_VALUE, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_OUTPUT_VALUE, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );

	mPinInput = pinInput( "Number", PIN_INPUT_VALUE );

	mValOutput = pinOutput<fugio::VariantInterface *>( "Number", mPinOutput, PID_FLOAT, PIN_OUTPUT_VALUE );

	mPinInput->setAutoRename( true );
}

void FloorNode::inputsUpdated( qint64 pTimeStamp )
{
	Q_UNUSED( pTimeStamp )

	float		NewVal = std::floor( variant( mPinInput ).toFloat() );

	if( NewVal != mValOutput->variant().toFloat() )
	{
		mValOutput->setVariant( NewVal );

		pinUpdated( mPinOutput );
	}
}
