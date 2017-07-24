#include "modulusnode.h"

#include <cmath>

#include <fugio/core/uuid.h>

#include <fugio/context_interface.h>

ModulusNode::ModulusNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_MODULUS, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_INPUT_NUMBER,  "da55f50c-1dee-47c9-bee5-6a2e48672a13" );
	FUGID( PIN_OUTPUT_NUMBER, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );

	mPinInputModulus = pinInput( "Modulus", PIN_INPUT_MODULUS );

	mPinInputModulus->setValue( 2 );

	mPinInputNumber = pinInput( "Number", PIN_INPUT_NUMBER );

	mValOutputNumber = pinOutput<fugio::VariantInterface *>( "Number", mPinOutputNumber, PID_FLOAT, PIN_OUTPUT_NUMBER );

	mPinInputNumber->setAutoRename( true );

	mNode->pairPins( mPinInputNumber, mPinOutputNumber );
}

void ModulusNode::inputsUpdated( qint64 pTimeStamp )
{
	Q_UNUSED( pTimeStamp )

	float		NewMod = variant( mPinInputModulus ).toFloat();
	float		NewVal = variant( mPinInputNumber ).toFloat();

	NewVal = std::fmod( NewVal, NewMod );

	if( NewVal != mValOutputNumber->variant().toFloat() )
	{
		mValOutputNumber->setVariant( NewVal );

		pinUpdated( mPinOutputNumber );
	}
}
