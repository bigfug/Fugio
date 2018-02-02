#include "cobsdecodenode.h"

#include <fugio/core/uuid.h>

COBSDecodeNode::COBSDecodeNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mCurLen( 0 ), mCurPos( 0 )
{
	FUGID( PIN_INPUT_STREAM,	"89B35447-3332-4C4E-8705-4471D1DDF917" );
	FUGID( PIN_INPUT_RESET,		"6C79DE4A-A3F1-4FB2-A8DC-4159D22B72BA" );
	FUGID( PIN_OUTPUT_ARRAY,	"8B0B0E4E-33F8-4534-8D61-4C80CB8CA0E1" );

	mPinInput = pinInput( "Input", PIN_INPUT_STREAM );

	mPinInput->registerPinInputType( PID_BYTEARRAY );

	mPinInputReset = pinInput( "Reset", PIN_INPUT_RESET );

	mValOutput = pinOutput<fugio::VariantInterface *>( "Output", mPinOutput, PID_BYTEARRAY, PIN_OUTPUT_ARRAY );
}

void COBSDecodeNode::inputsUpdated( qint64 pTimeStamp )
{
	if( mPinInputReset->isUpdated( pTimeStamp ) )
	{
		mCurPkt.clear();
	}

	if( mPinInput->isUpdated( pTimeStamp ) )
	{
		const QByteArray	InpDat = variant( mPinInput ).toByteArray();

		if( InpDat.isEmpty() )
		{
			return;
		}

		mValOutput->variantClear();

		for( unsigned char c : InpDat )
		{
			if( c == 0x00 )
			{
				if( !mCurPkt.isEmpty() )
				{
					mValOutput->variantAppend( mCurPkt );

					mCurPkt.clear();
				}

				mCurLen = 0;
			}
			else if( !mCurLen )
			{
				mCurLen = c;
				mCurPos = 1;
			}
			else if( mCurPos < mCurLen )
			{
				mCurPkt.append( c );
				mCurPos++;
			}
			else
			{
				if( mCurLen < 0xff )
				{
					mCurPkt.append( char( 0 ) );
				}

				mCurLen = c;
				mCurPos = 1;
			}
		}

		if( mValOutput->variantCount() )
		{
			pinUpdated( mPinOutput );
		}
	}
}
