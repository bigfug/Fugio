#include "cobsencodenode.h"

#include <fugio/core/uuid.h>
#include <fugio/core/list_interface.h>
#include <fugio/performance.h>
#include <fugio/pin_variant_iterator.h>


COBSEncodeNode::COBSEncodeNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_ARRAY,		"89B35447-3332-4C4E-8705-4471D1DDF917" );
	FUGID( PIN_OUTPUT_STREAM,	"8B0B0E4E-33F8-4534-8D61-4C80CB8CA0E1" );

	mPinInput = pinInput( "Input", PIN_INPUT_ARRAY );

	mPinInput->registerPinInputType( PID_BYTEARRAY );

	mValOutput = pinOutput<fugio::VariantInterface *>( "Output", mPinOutput, PID_BYTEARRAY, PIN_OUTPUT_STREAM );
}

void COBSEncodeNode::inputsUpdated( qint64 pTimeStamp )
{
	fugio::Performance		Perf( mNode, "inputsUpdated", pTimeStamp );

	QByteArray				DatOut;

	fugio::PinVariantIterator	Input( mPinInput );

	for( int i = 0 ; i < Input.count() ; i++ )
	{
		const QVariant		VarInt = Input.index( i );

		if( VarInt.type() == QVariant::ByteArray )
		{
			processByteArray( VarInt.toByteArray(), DatOut );
		}
	}

	mValOutput->setVariant( DatOut );

	pinUpdated( mPinOutput );
}

void COBSEncodeNode::processByteArray( const QByteArray &pDatInp, QByteArray &pDatOut )
{
	if( pDatInp.isEmpty() )
	{
		return;
	}

	pDatOut.append( char( 0 ) );

	int			OutIdx = 0;
	quint8		OutLen = 1;

	for( unsigned char c : pDatInp )
	{
		if( c == 0x00 )
		{
			pDatOut[ OutIdx ] = OutLen;

			OutIdx = pDatOut.size();

			pDatOut.append( char( 0 ) );

			OutLen = 1;
		}
		else
		{
			pDatOut.append( c );

			if( ++OutLen == 0xff )
			{
				pDatOut[ OutIdx ] = OutLen;

				OutIdx = pDatOut.size();

				pDatOut.append( char( 0 ) );

				OutLen = 1;
			}
		}
	}

	pDatOut[ OutIdx ] = OutLen;

	pDatOut.append( char( 0 ) );
}
