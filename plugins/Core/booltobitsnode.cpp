#include "booltobitsnode.h"

#include <fugio/context_interface.h>

#include <fugio/core/uuid.h>

BoolToBits::BoolToBits( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mBitPos( 0 )
{
	FUGID( PIN_INPUT_BOOL,	"9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_INPUT_COUNT,	"1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );
	FUGID( PIN_OUTPUT_BITS, "3d995a98-0f56-4371-a1fa-67ecaefeba1c" );

	mPinInputTrigger = pinInput( "Trigger", PID_FUGIO_NODE_TRIGGER );

	mPinInputBool = pinInput( "Bool", PIN_INPUT_BOOL );

	mPinInputBool->setUpdatable( false );

	mPinInputCount = pinInput( "Count", PIN_INPUT_COUNT );

	mPinInputCount->setValue( 8 );

	mPinInputCount->setUpdatable( false );

	mValOutputBits = pinOutput<fugio::VariantInterface *>( "Bits", mPinOutputBits, PID_BITARRAY, PIN_OUTPUT_BITS );
}

void BoolToBits::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	int			BitCnt = variant( mPinInputCount ).toInt();

	if( BitCnt > 0 && BitCnt != mBitDat.size() )
	{
		mBitDat.resize( BitCnt );

		mBitPos = 0;
	}

	if( mPinInputTrigger->isUpdated( pTimeStamp ) )
	{
		bool		CurVal = variant( mPinInputBool ).toBool();

		mBitDat.setBit( mBitPos++, CurVal );

		if( mBitPos == mBitDat.size() )
		{
//			qDebug() << "S:" << mBitDat;

			mValOutputBits->setVariant( mBitDat );

			pinUpdated( mPinOutputBits );

			mBitPos = 0;
		}
	}
}
