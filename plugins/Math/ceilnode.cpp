#include "ceilnode.h"

#include <cmath>

#include <fugio/core/uuid.h>

#include <fugio/context_interface.h>

#include <fugio/pin_variant_iterator.h>

CeilNode::CeilNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_VALUE, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_OUTPUT_VALUE, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );

	mPinInput = pinInput( "Number", PIN_INPUT_VALUE );

	mValOutput = pinOutput<fugio::VariantInterface *>( "Number", mPinOutput, PID_FLOAT, PIN_OUTPUT_VALUE );

	mPinInput->setAutoRename( true );
}

void CeilNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	bool		UpdateOutput = mPinOutput->alwaysUpdate();

	fugio::PinVariantIterator	Input( mPinInput );

	variantSetCount( mValOutput, Input.count(), UpdateOutput );

	for( int i = 0 ; i < Input.count() ; i++ )
	{
		variantSetValue( mValOutput, i, std::ceil( Input.index( i ).toFloat() ), UpdateOutput );
	}

	if( UpdateOutput )
	{
		pinUpdated( mPinOutput );
	}
}
