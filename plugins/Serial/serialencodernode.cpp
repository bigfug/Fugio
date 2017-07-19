#include "serialencodernode.h"

#include <QBitArray>

#include <fugio/core/uuid.h>

#include <fugio/context_interface.h>

SerialEncoderNode::SerialEncoderNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_DATA,	"7AC449C1-0CC8-4DEA-A404-BB439BDD976E" );
	FUGID( PIN_OUTPUT_BITS,	"7A49997F-F720-4EBA-81D3-347F00C55CB9" );

	mPinInputData = pinInput( "Data", PIN_INPUT_DATA );

	mValOutputBits = pinOutput<fugio::VariantInterface *>( "Bits", mPinOutputBits, PID_BITARRAY, PIN_OUTPUT_BITS );
}

void SerialEncoderNode::inputsUpdated( qint64 pTimeStamp )
{
	if( mPinInputData->isUpdated( pTimeStamp ) )
	{
		QByteArray	A = variant( mPinInputData ).toByteArray();

		if( !A.isEmpty() )
		{
			const int		DataSize = 8;

			int				OutCnt = A.size() * ( DataSize + 2 );
			QBitArray		DstDat( OutCnt );

			int				OutPos = 0;

			for( quint8 C : A )
			{
				DstDat.setBit( OutPos++, false );		// Start Bit

				for( int i = 0 ; i < DataSize ; i++ )
				{
					DstDat.setBit( OutPos++, ( C & 0x01 ) != 0 );

					C >>= 1;
				}

				DstDat.setBit( OutPos++, true );		// Stop Bit
			}

			mValOutputBits->setVariant( DstDat );

			pinUpdated( mPinOutputBits );
		}
	}
}
