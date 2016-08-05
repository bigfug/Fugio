#include "notetofrequencynode.h"

#include <fugio/core/uuid.h>

#include <cmath>

NoteToFrequencyNode::NoteToFrequencyNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_MIDI,			"82d09108-d75d-4d89-bd2c-951454c1e1a8" );
	FUGID( PIN_INPUT_BEND,			"e07829e2-84ad-41a0-8ff7-b6eeb170098a" );
	FUGID( PIN_INPUT_RANGE,			"4268393a-5c9e-4988-b1f1-6f0bca96bb59" );
	FUGID( PIN_OUTPUT_FREQUENCY,	"aefac5f4-db11-4445-bceb-611c89bc56a9" );

	mPinInputNote = pinInput( "MIDI Note", PIN_INPUT_MIDI );

	mPinInputBend = pinInput( "MIDI Pitch Bend", PIN_INPUT_BEND );

	mPinInputRange = pinInput( "Range (Cents)", PIN_INPUT_RANGE );

	mValOutput = pinOutput<fugio::VariantInterface *>( "Frequency (Hz)", mPinOutput, PID_FLOAT, PIN_OUTPUT_FREQUENCY );

	mPinInputNote->setValue( 69 );
	mPinInputBend->setValue( 8192 );
	mPinInputRange->setValue( 100 );
}

void NoteToFrequencyNode::inputsUpdated( qint64 pTimeStamp )
{
	Q_UNUSED( pTimeStamp )

	int		Bend = variant( mPinInputBend ).toInt();
	float	Range = variant( mPinInputRange ).toFloat();

	float	m = qBound( 0, variant( mPinInputNote ).toInt(), 127 );

	m += ( float( Bend - 8192 ) / 8192.0f ) * ( Range / 100.0f );

	float	fm = 440.0 * powf( 2.0f, float( m - 69 ) / 12.0f );

	if( mValOutput->variant().toFloat() != fm )
	{
		mValOutput->setVariant( fm );

		pinUpdated( mPinOutput );
	}
}
