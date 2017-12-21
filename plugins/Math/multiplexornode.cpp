#include "multiplexornode.h"

#include <fugio/core/uuid.h>

#include <fugio/context_interface.h>

MultiplexorNode::MultiplexorNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_A, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_INPUT_B, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );
	FUGID( PIN_INPUT_SEL, "e6bf944e-5f46-4994-bd51-13c2aa6415b7" );
	FUGID( PIN_OUTPUT_VALUE, "261cc653-d7fa-4c34-a08b-3603e8ae71d5" );

	mPinInputA   = pinInput( "A", PIN_INPUT_A);
	mPinInputB   = pinInput( "B", PIN_INPUT_B );
	mPinInputSel = pinInput( "Sel", PIN_INPUT_SEL );

	mValOutputValue = pinOutput<fugio::VariantInterface *>( "Output", mPinOutputValue, PID_BOOL, PIN_OUTPUT_VALUE );
}

void MultiplexorNode::inputsUpdated( qint64 pTimeStamp )
{
	Q_UNUSED( pTimeStamp )

	bool	A = variant( mPinInputA ).toBool();
	bool	B = variant( mPinInputB ).toBool();
	bool	S = variant( mPinInputSel ).toBool();
	bool	V = S ? B : A;

	if( V != mValOutputValue->variant().toBool() )
	{
		mValOutputValue->setVariant( V );

		pinUpdated( mPinOutputValue );
	}
}
