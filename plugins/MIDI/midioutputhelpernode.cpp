#include "midioutputhelpernode.h"

#include <fugio/node_signals.h>
#include <fugio/core/uuid.h>
#include <fugio/utils.h>
#include <fugio/core/variant_interface.h>
#include <fugio/context_interface.h>

MidiOutputHelperNode::MidiOutputHelperNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	QSharedPointer<fugio::PinInterface>		PinI, PinO;

	PinI = pinInput( "Value" );

	pinOutput<fugio::PinControlInterface *>( "Value", PinO, PID_INTEGER );

	if( PinI && PinO )
	{
		mNode->pairPins( PinI, PinO );
	}

	PinI->setAutoRename( true );
}

void MidiOutputHelperNode::inputsUpdated( qint64 pTimeStamp )
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

		QUuid							 SrcId = SrcPin->connectedPin()->controlUuid();
		int								 NewVal = DstVar->variant().toInt();

		if( SrcId == PID_BOOL )
		{
			NewVal = SrcVar->variant().toBool() ? 127 : 0;
		}
		else if( SrcId == PID_TRIGGER )
		{
			NewVal = 127;
		}
		else if( SrcId == PID_FLOAT )
		{
			NewVal = qBound( 0, int( SrcVar->variant().toDouble() * 127 ), 127 );
		}

		if( SrcId != PID_TRIGGER && DstVar->variant().toInt() == NewVal )
		{
			continue;
		}

		DstVar->setVariant( NewVal );

		pinUpdated( DstPin );
	}
}

QList<QUuid> MidiOutputHelperNode::pinAddTypesInput() const
{
	static QList<QUuid>	PinLst =
	{
		PID_BOOL,
		PID_TRIGGER,
		PID_INTEGER,
		PID_FLOAT
	};

	return( PinLst );
}

bool MidiOutputHelperNode::canAcceptPin( fugio::PinInterface * ) const
{
	return( true );
}

QUuid MidiOutputHelperNode::pairedPinControlUuid( QSharedPointer<fugio::PinInterface> pPin ) const
{
	Q_UNUSED( pPin )

	return( PID_INTEGER );
}

bool MidiOutputHelperNode::pinShouldAutoRename(fugio::PinInterface *pPin) const
{
	return( pPin->direction() == PIN_INPUT );
}
