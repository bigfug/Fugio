#include "example1node.h"

#include <fugio/core/uuid.h>

Example1Node::Example1Node( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	mPinInput = pinInput( "Input" );

	mOutput = pinOutput<fugio::VariantInterface *>( "Output", mPinOutput, PID_FLOAT );
}

void Example1Node::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	float		InputValue = variant( mPinInput ).toFloat();

	float		OutputValue = InputValue * 2.0f;

	mOutput->setVariant( OutputValue );

	pinUpdated( mPinOutput );
}

