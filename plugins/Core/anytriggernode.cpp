#include "anytriggernode.h"

#include <fugio/core/uuid.h>

#include <fugio/context_interface.h>

AnyTriggerNode::AnyTriggerNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	pinInput( "Trigger" );

	pinOutput<fugio::PinControlInterface *>( "Trigger", mPinOutput, PID_TRIGGER );

	mPinOutput->setDescription( tr( "If any of the input pins are triggered, the output will be triggered too" ) );
}

void AnyTriggerNode::inputsUpdated( qint64 pTimeStamp )
{
	if( !pTimeStamp )
	{
		return;
	}

	mNode->context()->pinUpdated( mPinOutput );
}


QList<QUuid> AnyTriggerNode::pinAddTypesInput() const
{
	return( mNode->context()->global()->pinIds().keys() );
}

bool AnyTriggerNode::canAcceptPin( fugio::PinInterface *pPin ) const
{
	return( pPin->direction() == PIN_OUTPUT );
}
