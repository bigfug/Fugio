#include "notbitsnode.h"

#include <QBitArray>

#include <fugio/core/uuid.h>

#include <fugio/context_interface.h>

NotBitsNode::NotBitsNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PII_NUMBER1, "c13a41c6-544b-46bb-a9f2-19dd156d236c" );
	FUGID( PIN_OUTPUT_BOOL, "0fb3ba87-ff71-41bc-84ee-4f488a18068f" );

	mPinInput = pinInput( "Bits", PII_NUMBER1 );

	mValOutput = pinOutput<fugio::VariantInterface *>( "Boolean", mPinOutput, PID_BITARRAY, PIN_OUTPUT_BOOL );
}

void NotBitsNode::inputsUpdated( qint64 pTimeStamp )
{
	Q_UNUSED( pTimeStamp )

	QBitArray	InpVar = variant( mPinInput ).toBitArray();

	QBitArray	OutArr = ~InpVar;

	if( OutArr != mValOutput->variant().toBitArray() )
	{
		mValOutput->setVariant( OutArr );

		pinUpdated( mPinOutput );
	}
}
