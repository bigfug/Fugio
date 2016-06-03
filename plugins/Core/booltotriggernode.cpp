#include "booltotriggernode.h"

#include <fugio/core/uuid.h>

#include <fugio/context_interface.h>
#include <fugio/node_signals.h>

BoolToTriggerNode::BoolToTriggerNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	QSharedPointer<fugio::PinInterface>		I, O;

	I = pinInput( "Bool" );

	pinOutput<fugio::PinControlInterface *>( "Trigger", O, PID_TRIGGER );

	mNode->pairPins( I, O );
}

bool BoolToTriggerNode::initialise()
{
	if( !NodeControlBase::initialise() )
	{
		return( false );
	}

	connect( mNode->qobject(), SIGNAL(pinAdded(QSharedPointer<fugio::NodeInterface>,QSharedPointer<fugio::PinInterface>)), this, SLOT(pinAdded(QSharedPointer<fugio::NodeInterface>,QSharedPointer<fugio::PinInterface>)) );

	return( true );
}

void BoolToTriggerNode::inputsUpdated( qint64 pTimeStamp )
{
	if( !pTimeStamp )
	{
		return;
	}

	for( fugio::NodeInterface::UuidPair UP : mNode->pairedPins() )
	{
		QSharedPointer<fugio::PinInterface> SrcPin = mNode->findPinByLocalId( UP.first );
		QSharedPointer<fugio::PinInterface> DstPin = mNode->findPinByLocalId( UP.second );

		if( !SrcPin || !SrcPin->isUpdated( pTimeStamp ) || !DstPin )
		{
			continue;
		}

		if( variant( SrcPin ).toBool() )
		{
			pinUpdated( DstPin );
		}
	}
}

void BoolToTriggerNode::pinAdded( QSharedPointer<fugio::NodeInterface> pNode, QSharedPointer<fugio::PinInterface> pPin )
{
	Q_UNUSED( pNode )

	if( pPin->direction() == PIN_INPUT )
	{
		QSharedPointer<fugio::PinInterface> DstPin;

		if( !pPin->pairedUuid().isNull() )
		{
			DstPin = mNode->findPinByLocalId( pPin->pairedUuid() );

			if( DstPin )
			{
				return;
			}
		}

		if( !DstPin )
		{
			pinOutput<fugio::VariantInterface *>( pPin->name(), DstPin, PID_TRIGGER );

			if( DstPin )
			{
				mNode->pairPins( pPin, DstPin );
			}
		}
	}
}

QList<QUuid> BoolToTriggerNode::pinAddTypesInput() const
{
	QList<QUuid>		PinLst;

	PinLst << PID_BOOL;

	return( PinLst );
}

bool BoolToTriggerNode::canAcceptPin( fugio::PinInterface *pPin ) const
{
	return( pPin->direction() == PIN_OUTPUT );
}

