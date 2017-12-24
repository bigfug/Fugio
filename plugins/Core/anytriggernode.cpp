#include "anytriggernode.h"

#include <fugio/core/uuid.h>

#include <fugio/context_interface.h>

AnyTriggerNode::AnyTriggerNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_TRIGGER,	"9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_OUTPUT_TRIGGER,	"1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );

	QSharedPointer<fugio::PinInterface>	PinInputTrigger = pinInput( "Trigger", PIN_INPUT_TRIGGER );

	PinInputTrigger->setAutoRename( true );

	pinOutput<fugio::PinControlInterface *>( "Trigger", mPinOutput, PID_TRIGGER, PIN_OUTPUT_TRIGGER );

	mPinOutput->setDescription( tr( "If any of the input pins are triggered, the output will be triggered too" ) );
}

void AnyTriggerNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	pinUpdated( mPinOutput );
}

QList<QUuid> AnyTriggerNode::pinAddTypesInput() const
{
	return( QList<QUuid>() );
}

bool AnyTriggerNode::canAcceptPin( fugio::PinInterface *pPin ) const
{
	return( pPin->direction() == PIN_OUTPUT );
}

bool AnyTriggerNode::pinShouldAutoRename( fugio::PinInterface *pPin ) const
{
	return( pPin->direction() == PIN_INPUT );
}
