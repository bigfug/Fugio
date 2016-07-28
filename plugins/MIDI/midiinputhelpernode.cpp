#include "midiinputhelpernode.h"

#include <fugio/node_signals.h>
#include <fugio/core/uuid.h>
#include <fugio/utils.h>
#include <fugio/core/variant_interface.h>
#include <fugio/context_interface.h>

MidiInputHelperNode::MidiInputHelperNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	QSharedPointer<fugio::PinInterface>		PinI, PinO;

	PinI = pinInput( "Value" );

	pinOutput<fugio::PinControlInterface *>( "Value", PinO, PID_FLOAT );

	if( PinI && PinO )
	{
		mNode->pairPins( PinI, PinO );
	}

	PinI->setAutoRename( true );
}

void MidiInputHelperNode::inputsUpdated( qint64 pTimeStamp )
{
	Q_UNUSED( pTimeStamp )

	for( fugio::NodeInterface::UuidPair UP : mNode->pairedPins() )
	{
		QSharedPointer<fugio::PinInterface> SrcPin = mNode->findPinByLocalId( UP.first );
		QSharedPointer<fugio::PinInterface> DstPin = mNode->findPinByLocalId( UP.second );

		if( !SrcPin || !DstPin || !DstPin->hasControl() )
		{
			continue;
		}

		fugio::VariantInterface			*SrcVar = input<fugio::VariantInterface *>( SrcPin );
		fugio::VariantInterface			*DstVar = qobject_cast<fugio::VariantInterface *>( DstPin->control()->qobject() );

		if( !SrcVar || !DstVar )
		{
			continue;
		}

		double						NewVal = double( qBound( 0, SrcVar->variant().toInt(), 127 ) ) / 127.0;

		if( DstVar->variant().toDouble() == NewVal )
		{
			continue;
		}

		DstVar->setVariant( NewVal );

		pinUpdated( DstPin );
	}
}

QList<QUuid> MidiInputHelperNode::pinAddTypesInput() const
{
	static QList<QUuid>	PinLst =
	{
		PID_INTEGER
	};

	return( PinLst );
}

bool MidiInputHelperNode::canAcceptPin( fugio::PinInterface * ) const
{
	return( true );
}

QUuid MidiInputHelperNode::pairedPinControlUuid( QSharedPointer<fugio::PinInterface> pPin ) const
{
	Q_UNUSED( pPin )

	return( PID_FLOAT );
}

bool MidiInputHelperNode::pinShouldAutoRename(fugio::PinInterface *pPin) const
{
	return( pPin->direction() == PIN_INPUT );
}
