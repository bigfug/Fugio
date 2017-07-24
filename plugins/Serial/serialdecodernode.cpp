#include "serialdecodernode.h"

#include <QBitArray>

#include <fugio/core/uuid.h>

#include <fugio/context_interface.h>

SerialDecoderNode::SerialDecoderNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_BITS,	"7AC449C1-0CC8-4DEA-A404-BB439BDD976E" );
	FUGID( PIN_OUTPUT_DATA,	"7A49997F-F720-4EBA-81D3-347F00C55CB9" );

	mPinInputBits = pinInput( "Bits", PIN_INPUT_BITS );

	mValOutputData = pinOutput<fugio::VariantInterface *>( "Data", mPinOutputData, PID_BYTEARRAY, PIN_OUTPUT_DATA );
}

void SerialDecoderNode::inputsUpdated( qint64 pTimeStamp )
{
	if( mPinInputBits->isUpdated( pTimeStamp ) )
	{
		QBitArray	InpDat = variant( mPinInputBits ).toBitArray();

		if( !InpDat.isEmpty() )
		{
			QBitArray		SrcDat;
			QByteArray		DstDat;

			// Prepend any buffered data

			if( !mBitBuf.isEmpty() )
			{
				int		InpSze = InpDat.size();
				int		BufSze = mBitBuf.size();

				SrcDat.resize( BufSze + InpSze );

				for( int i = 0 ; i < BufSze ; i++ )
				{
					SrcDat.setBit( i, mBitBuf.testBit( i ) );
				}

				for( int i = 0 ; i < InpSze ; i++ )
				{
					SrcDat.setBit( BufSze + i, InpDat.testBit( i ) );
				}

				mBitBuf.clear();
			}
			else
			{
				SrcDat.swap( InpDat );
			}

			// Look for data

//			qDebug() << "R:" << SrcDat;

			int SrcOff;

			for( SrcOff = 0 ; SrcOff < SrcDat.size() - 9 ; )
			{
				if( SrcDat.testBit( SrcOff ) || !SrcDat.testBit( SrcOff + 9 ) )
				{
					SrcOff++;

					continue;
				}

				QBitArray	T( 8 );

				quint8		C = 0;

				for( int j = 0 ; j < 8 ; j++ )
				{
					C |= ( SrcDat.testBit( SrcOff + 1 + j ) ? 0x01 : 0x00 ) << j;

					T.setBit( j, SrcDat.testBit( SrcOff + 1 + j ) );
				}

//				qDebug() << SrcOff << T;

				DstDat.append( C );

				SrcOff += 10;
			}

			if( SrcOff < SrcDat.size() )
			{
				if( SrcOff > 0 )
				{
					mBitBuf.resize( SrcDat.size() - SrcOff );

					for( int i = 0 ; i < mBitBuf.size() ; i++ )
					{
						mBitBuf.setBit( i, SrcDat.testBit( SrcOff + i ) );
					}
				}
				else
				{
					SrcDat.swap( mBitBuf );
				}

//				qDebug() << "B" << mBitBuf;
			}

			if( !DstDat.isEmpty() )
			{
				mValOutputData->setVariant( DstDat );

				pinUpdated( mPinOutputData );
			}
		}
	}
}
