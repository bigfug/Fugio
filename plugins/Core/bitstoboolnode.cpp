#include "bitstoboolnode.h"

#include <fugio/core/uuid.h>

#include <fugio/context_interface.h>

BitsToBoolNode::BitsToBoolNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mBitPos( 0 )
{
	FUGID( PIN_INPUT_BITS,	"9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_OUTPUT_BOOL,	"1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );
	FUGID( PIN_OUTPUT_EMPTY, "3d995a98-0f56-4371-a1fa-67ecaefeba1c" );

	mPinInputTrigger = pinInput( "Trigger", PID_FUGIO_NODE_TRIGGER );

	mPinInputBits = pinInput( "Bits", PIN_INPUT_BITS );

	mPinInputBits->registerPinInputType( PID_BITARRAY );

	mValOutputBool = pinOutput<fugio::VariantInterface *>( "Bool", mPinOutputBool, PID_BOOL, PIN_OUTPUT_BOOL );

	mPinOutputEmpty = pinOutput( "Empty", PIN_OUTPUT_EMPTY );
}

void BitsToBoolNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	if( mPinInputBits->isUpdated( pTimeStamp ) )
	{
		QBitArray	A = variant( mPinInputBits ).toBitArray();

		if( !A.isEmpty() )
		{
			mBitDat.append( A );
		}
	}

	if( mPinInputTrigger->isUpdated( pTimeStamp ) )
	{
		bool		OutVal = true;

		if( !mBitDat.isEmpty() )
		{
			OutVal = mBitDat.first().testBit( mBitPos++ );

			if( mBitPos == mBitDat.first().size() )
			{
				mBitDat.removeFirst();

				mBitPos = 0;
			}

			if( mBitDat.isEmpty() )
			{
				pinUpdated( mPinOutputEmpty );
			}
		}

		if( mValOutputBool->variant().toBool() != OutVal )
		{
			mValOutputBool->setVariant( OutVal );

			pinUpdated( mPinOutputBool );
		}
	}
}
