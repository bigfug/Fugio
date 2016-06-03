#include "absnode.h"

#include <fugio/core/uuid.h>

#include <fugio/context_interface.h>

AbsNode::AbsNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	static const QUuid	PII_NUMBER1( "{c13a41c6-544b-46bb-a9f2-19dd156d236c}" );
	static const QUuid	PII_NUMBER2( "{608ac771-490b-4ae6-9c81-12b9af526d09}" );

	mPinInput = pinInput( "Number", PII_NUMBER1 );

	mValOutput = pinOutput<fugio::VariantInterface *>( "Number", mPinOutput, PID_FLOAT, PII_NUMBER2 );
}

void AbsNode::inputsUpdated( qint64 pTimeStamp )
{
	Q_UNUSED( pTimeStamp )

	float		NewVal = qAbs( variant( mPinInput ).toFloat() );

	if( NewVal != mValOutput->variant().toFloat() )
	{
		mValOutput->setVariant( NewVal );

		pinUpdated( mPinOutput );
	}
}
