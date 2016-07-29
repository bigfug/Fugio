#include "midirotarynode.h"

#include <fugio/core/uuid.h>

#include <fugio/context_interface.h>

MidiRotaryNode::MidiRotaryNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	mPinInput = pinInput( "MIDI Control" );
	mPinForce = pinInput( "Force" );

	mValOutput = pinOutput<fugio::VariantInterface *>( "Force", mPinOutput, PID_FLOAT );

	mPinForce->setValue( 0.1 );
}

void MidiRotaryNode::inputsUpdated( qint64 pTimeStamp )
{
	Q_UNUSED( pTimeStamp )

	double		Force = mPinForce->value().toDouble();

	fugio::VariantInterface	*V;

	if( ( V = input<fugio::VariantInterface *>( mPinForce ) ) )
	{
		Force = V->variant().toDouble();
	}

	if( ( V = input<fugio::VariantInterface *>( mPinInput ) ) )
	{
		if( V->variant().toInt() >= 63 )
		{
			Force *= -1.0;
		}
		else
		{
			Force *=  1.0;
		}
	}

	mValOutput->setVariant( Force );

	mNode->context()->pinUpdated( mPinOutput );
}
