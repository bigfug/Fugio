#include "pownode.h"

#include <fugio/core/uuid.h>

#include <fugio/context_interface.h>

PowNode::PowNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_X, "cbcb06fe-a558-4de0-9c9a-96d10c455453" )
	FUGID( PIN_INPUT_Y, "fa36dd69-dc59-493f-bc40-50fd72b55b8d" )
	FUGID( PIN_OUTPUT_RESULT, "0ab78535-a5a4-44a0-bcd5-0f1bf8b7f9f3" )

	mPinInputX = pinInput( "x", PIN_INPUT_X );
	mPinInputY = pinInput( "y", PIN_INPUT_Y );

	mValOutput = pinOutput<fugio::VariantInterface *>( "Result", mPinOutput, PID_FLOAT, PIN_OUTPUT_RESULT );
}

void PowNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	qreal	x = variant( mPinInputX ).toReal();
	qreal	y = variant( mPinInputX ).toReal();

	qreal	r = std::pow( x, y );

	if( r != mValOutput->variant().toFloat() )
	{
		mValOutput->setVariant( r );

		pinUpdated( mPinOutput );
	}
}
