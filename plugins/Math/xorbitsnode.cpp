#include "xorbitsnode.h"

#include <QBitArray>

#include <fugio/core/uuid.h>

#include <fugio/context_interface.h>

XorBitsNode::XorBitsNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_BITS1, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_INPUT_BITS2, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );
	FUGID( PIN_OUTPUT_BITS, "261cc653-d7fa-4c34-a08b-3603e8ae71d5" );

	mPinInputBits1 = pinInput( "Bits", PIN_INPUT_BITS1 );
	mPinInputBits2 = pinInput( "Bits", PIN_INPUT_BITS2 );

	mValOutputBits = pinOutput<fugio::VariantInterface *>( "Bits", mPinOutputBits, PID_BITARRAY, PIN_OUTPUT_BITS );
}

void XorBitsNode::inputsUpdated( qint64 pTimeStamp )
{
	Q_UNUSED( pTimeStamp )

	QBitArray	InpArr1 = variant( mPinInputBits1 ).toBitArray();
	QBitArray	InpArr2 = variant( mPinInputBits2 ).toBitArray();

	QBitArray	OutArr = InpArr1 ^ InpArr2;

	if( OutArr != mValOutputBits->variant().toBitArray() )
	{
		mValOutputBits->setVariant( OutArr );

		pinUpdated( mPinOutputBits );
	}
}
