#include "frequencytonotenode.h"

#include <fugio/core/uuid.h>

#include <cmath>

FrequencyToNoteNode::FrequencyToNoteNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mLastNote( -10000 ), mLastFreq( -10000 )
{
	FUGID( PIN_INPUT_RANGE,			"4268393a-5c9e-4988-b1f1-6f0bca96bb59" );
	FUGID( PIN_INPUT_FREQUENCY,		"aefac5f4-db11-4445-bceb-611c89bc56a9" );
	FUGID( PIN_OUTPUT_NOTE,			"82d09108-d75d-4d89-bd2c-951454c1e1a8" );
	FUGID( PIN_OUTPUT_BEND,			"e07829e2-84ad-41a0-8ff7-b6eeb170098a" );

	mPinInputFrequency = pinInput( "Frequency (Hz)", PIN_INPUT_FREQUENCY );

	mPinInputRange = pinInput( "Range (Cents)", PIN_INPUT_RANGE );

	mValOutputNote = pinOutput<fugio::VariantInterface *>( "MIDI Note", mPinOutputNote, PID_INTEGER, PIN_OUTPUT_NOTE );

	mValOutputBend = pinOutput<fugio::VariantInterface *>( "MIDI Pitch Bend", mPinOutputBend, PID_INTEGER, PIN_OUTPUT_BEND );

	mPinInputFrequency->setValue( 440.0 );
	mValOutputBend->setVariant( 8192 );
	mPinInputRange->setValue( 100 );

	mPinInputRange->setDescription( "The range of pitch bend in cents (100 cents = 1 semitone)" );

	mPinOutputNote->setDescription( "The MIDI note value (0-127)" );

	mPinOutputBend->setDescription( "The MIDI pitch bend value (0-16383)" );
}

void FrequencyToNoteNode::inputsUpdated( qint64 pTimeStamp )
{
	Q_UNUSED( pTimeStamp )

	float	Freq  = variant( mPinInputFrequency ).toFloat();
	float	Range = variant( mPinInputRange ).toFloat();

	int		MidiNote = mLastNote;
	float	MidiFreq = mLastFreq;
	float	Bend     = mValOutputBend->variant().toInt();

	if( MidiNote < 0 || std::fabs( std::log2f( Freq / mLastFreq ) * 1200.0f ) > Range )
	{
		MidiNote = qBound<int>( 0, 12.0f * std::log2f( Freq / 440.0f ) + 69.0f, 127 );
		MidiFreq = 440.0 * std::powf( 2.0f, float( MidiNote - 69 ) / 12.0f );
	}

	float	CentDiff = std::log2f( Freq / MidiFreq ) * 1200.0f;

	Bend = qBound<int>( 0, 8192.0f + ( ( CentDiff / Range ) * 8192.0f ), 16383 );

	if( mValOutputNote->variant().toInt() != MidiNote )
	{
		mValOutputNote->setVariant( MidiNote );

		pinUpdated( mPinOutputNote );
	}

	if( mValOutputBend->variant().toInt() != Bend )
	{
		mValOutputBend->setVariant( Bend );

		pinUpdated( mPinOutputBend );
	}

	mLastNote = MidiNote;
	mLastFreq = MidiFreq;
}
