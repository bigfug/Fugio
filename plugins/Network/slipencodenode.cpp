#include "slipencodenode.h"

#include <fugio/core/uuid.h>
#include <fugio/core/list_interface.h>
#include <fugio/performance.h>
#include <fugio/pin_variant_iterator.h>

#define END             0300    /* indicates end of packet */
#define ESC             0333    /* indicates byte stuffing */
#define ESC_END         0334    /* ESC ESC_END means END data byte */
#define ESC_ESC         0335    /* ESC ESC_ESC means ESC data byte */

SLIPEncodeNode::SLIPEncodeNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_ARRAY,		"89B35447-3332-4C4E-8705-4471D1DDF917" );
	FUGID( PIN_OUTPUT_STREAM,	"8B0B0E4E-33F8-4534-8D61-4C80CB8CA0E1" );

	mPinInput = pinInput( "Input", PIN_INPUT_ARRAY );

	mPinInput->registerPinInputType( PID_BYTEARRAY );

	mValOutput = pinOutput<fugio::VariantInterface *>( "Output", mPinOutput, PID_BYTEARRAY, PIN_OUTPUT_STREAM );
}

void SLIPEncodeNode::inputsUpdated( qint64 pTimeStamp )
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

void SLIPEncodeNode::processByteArray( const QByteArray &pDatInp, QByteArray &pDatOut )
{
	pDatOut.append( END );

	for( unsigned char c : pDatInp )
	{
		switch( c )
		{
			case END:
				pDatOut.append( ESC );
				pDatOut.append( ESC_END );
				break;

			case ESC:
				pDatOut.append( ESC );
				pDatOut.append( ESC_ESC );
				break;

			default:
				pDatOut.append( c );
				break;
		}
	}

	pDatOut.append( END );
}
