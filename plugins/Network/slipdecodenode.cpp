#include "slipdecodenode.h"

#include <fugio/core/uuid.h>

#define END             0300    /* indicates end of packet */
#define ESC             0333    /* indicates byte stuffing */
#define ESC_END         0334    /* ESC ESC_END means END data byte */
#define ESC_ESC         0335    /* ESC ESC_ESC means ESC data byte */

SLIPDecodeNode::SLIPDecodeNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mEscChr( false )
{
	FUGID( PIN_INPUT_STREAM,	"89B35447-3332-4C4E-8705-4471D1DDF917" );
	FUGID( PIN_INPUT_RESET,		"6C79DE4A-A3F1-4FB2-A8DC-4159D22B72BA" );
	FUGID( PIN_OUTPUT_ARRAY,	"8B0B0E4E-33F8-4534-8D61-4C80CB8CA0E1" );

	mPinInput = pinInput( "Input", PIN_INPUT_STREAM );

	mPinInput->registerPinInputType( PID_BYTEARRAY );

	mPinInputReset = pinInput( "Reset", PIN_INPUT_RESET );

	mValOutput = pinOutput<fugio::VariantInterface *>( "Output", mPinOutput, PID_BYTEARRAY, PIN_OUTPUT_ARRAY );
}

void SLIPDecodeNode::inputsUpdated( qint64 pTimeStamp )
{
	if( mPinInputReset->isUpdated( pTimeStamp ) )
	{
		mEscChr = false;

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
			if( mEscChr )
			{
				switch( c )
				{
					case ESC_END:
						c = END;
						break;

					case ESC_ESC:
						c = ESC;
						break;
				}

				mCurPkt.append( c );

				mEscChr = false;
			}
			else
			{
				switch( c )
				{
					case END:
						if( !mCurPkt.isEmpty() )
						{
							mValOutput->variantAppend( mCurPkt );

							mCurPkt.clear();
						}
						break;

					case ESC:
						mEscChr = true;
						break;

					default:
						mCurPkt.append( c );
						break;
				}
			}
		}

		if( mValOutput->variantCount() )
		{
			pinUpdated( mPinOutput );
		}
	}
}
