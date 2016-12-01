#include "triggerarraynode.h"

#include <fugio/core/uuid.h>

#include <fugio/context_interface.h>

TriggerArrayNode::TriggerArrayNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_RESET,		"415F045E-C93B-494D-984A-1F5D75F35040" );
	FUGID( PIN_OUTPUT_TRIGGER,	"1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );

	mPinInputTrigger = pinInput( "Trigger", PID_FUGIO_NODE_TRIGGER );

	mPinInputReset   = pinInput( "Reset", PIN_INPUT_RESET );

	QSharedPointer<fugio::PinInterface>		PinOut;

	pinOutput<fugio::PinControlInterface *>( "Trigger", PinOut, PID_TRIGGER, PIN_OUTPUT_TRIGGER );
}

void TriggerArrayNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	QList< QSharedPointer<fugio::PinInterface> >	PinLst;

	bool		PerformTrigger = false;

	if( mPinInputReset->isUpdated( pTimeStamp ) )
	{
		mIndex = 0;

		PerformTrigger = true;
	}

	if( mPinInputTrigger->isUpdated( pTimeStamp ) )
	{
		PinLst = mNode->enumOutputPins();

		mIndex = ( mIndex + 1 ) % PinLst.size();

		PerformTrigger = true;
	}

	if( PerformTrigger )
	{
		if( PinLst.isEmpty() )
		{
			PinLst = mNode->enumOutputPins();
		}

		if( PinLst.size() > mIndex )
		{
			pinUpdated( PinLst[ mIndex ] );
		}
	}
}

bool TriggerArrayNode::canAcceptPin( fugio::PinInterface *pPin ) const
{
	return( pPin->direction() == PIN_INPUT );
}

bool TriggerArrayNode::pinShouldAutoRename( fugio::PinInterface *pPin ) const
{
	return( pPin->direction() == PIN_OUTPUT );
}

QList<QUuid> TriggerArrayNode::pinAddTypesOutput() const
{
	static QList<QUuid>	PinLst =
	{
		PID_TRIGGER
	};

	return( PinLst );
}
