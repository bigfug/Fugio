#include "absnode.h"

#include <fugio/core/uuid.h>

#include <fugio/context_interface.h>
#include <fugio/pin_variant_iterator.h>

#include <cmath>

AbsNode::AbsNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	static const QUuid	PII_NUMBER1( "{c13a41c6-544b-46bb-a9f2-19dd156d236c}" );
	static const QUuid	PII_NUMBER2( "{608ac771-490b-4ae6-9c81-12b9af526d09}" );

	mPinInput = pinInput( "Number", PII_NUMBER1 );

	mValOutput = pinOutput<fugio::VariantInterface *>( "Number", mPinOutput, PID_FLOAT, PII_NUMBER2 );

	mPinInput->setAutoRename( true );
}

void AbsNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	fugio::PinVariantIterator	Input( mPinInput );

	bool	OutputUpdated = mValOutput->variantCount() != Input.count() || mPinOutput->alwaysUpdate();

	mValOutput->setVariantCount( Input.count() );

	for( int i = 0 ; i < Input.count() ; i++ )
	{
		float		NewVal = std::abs( Input.index( i ).toFloat() );

		if( NewVal != mValOutput->variant( i ).toFloat() )
		{
			mValOutput->setVariant( i, NewVal );

			OutputUpdated = true;
		}
	}

	if( OutputUpdated )
	{
		pinUpdated( mPinOutput );
	}
}
