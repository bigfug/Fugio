#include "channeloutputnode.h"

#include <fugio/core/uuid.h>
#include <fugio/midi/uuid.h>
#include <fugio/context_signals.h>
#include <fugio/node_signals.h>

QStringList	ChannelOutputNode::PrgLst =
{
	"Acoustic Grand Piano",
	"Bright Acoustic Piano",
	"Electric Grand Piano",
	"Honky-tonk Piano",
	"Electric Piano 1",
	"Electric Piano 2",
	"Harpsichord",
	"Clavi",
	"Celesta",
	"Glockenspiel",
	"Music Box",
	"Vibraphone",
	"Marimba",
	"Xylophone",
	"Tubular Bells",
	"Dulcimer",
	"Drawbar Organ",
	"Percussive Organ",
	"Rock Organ",
	"Church Organ",
	"Reed Organ",
	"Accordion",
	"Harmonica",
	"Tango Accordion",
	"Acoustic Guitar (nylon)",
	"Acoustic Guitar (steel)",
	"Electric Guitar (jazz)",
	"Electric Guitar (clean)",
	"Electric Guitar (muted)",
	"Overdriven Guitar",
	"Distortion Guitar",
	"Guitar harmonics",
	"Acoustic Bass",
	"Electric Bass (finger)",
	"Electric Bass (pick)",
	"Fretless Bass",
	"Slap Bass 1",
	"Slap Bass 2",
	"Synth Bass 1",
	"Synth Bass 2",
	"Violin",
	"Viola",
	"Cello",
	"Contrabass",
	"Tremolo Strings",
	"Pizzicato Strings",
	"Orchestral Harp",
	"Timpani",
	"String Ensemble 1",
	"String Ensemble 2",
	"SynthStrings 1",
	"SynthStrings 2",
	"Choir Aahs",
	"Voice Oohs",
	"Synth Voice",
	"Orchestra Hit",
	"Trumpet",
	"Trombone",
	"Tuba",
	"Muted Trumpet",
	"French Horn",
	"Brass Section",
	"SynthBrass 1",
	"SynthBrass 2",
	"Soprano Sax",
	"Alto Sax",
	"Tenor Sax",
	"Baritone Sax",
	"Oboe",
	"English Horn",
	"Bassoon",
	"Clarinet",
	"Piccolo",
	"Flute",
	"Recorder",
	"Pan Flute",
	"Blown Bottle",
	"Shakuhachi",
	"Whistle",
	"Ocarina",
	"Lead 1 (square)",
	"Lead 2 (sawtooth)",
	"Lead 3 (calliope)",
	"Lead 4 (chiff)",
	"Lead 5 (charang)",
	"Lead 6 (voice)",
	"Lead 7 (fifths)",
	"Lead 8 (bass + lead)",
	"Pad 1 (new age)",
	"Pad 2 (warm)",
	"Pad 3 (polysynth)",
	"Pad 4 (choir)",
	"Pad 5 (bowed)",
	"Pad 6 (metallic)",
	"Pad 7 (halo)",
	"Pad 8 (sweep)",
	"FX 1 (rain)",
	"FX 2 (soundtrack)",
	"FX 3 (crystal)",
	"FX 4 (atmosphere)",
	"FX 5 (brightness)",
	"FX 6 (goblins)",
	"FX 7 (echoes)",
	"FX 8 (sci-fi)",
	"Sitar",
	"Banjo",
	"Shamisen",
	"Koto",
	"Kalimba",
	"Bag pipe",
	"Fiddle",
	"Shanai",
	"Tinkle Bell",
	"Agogo",
	"Steel Drums",
	"Woodblock",
	"Taiko Drum",
	"Melodic Tom",
	"Synth Drum",
	"Reverse Cymbal",
	"Guitar Fret Noise",
	"Breath Noise",
	"Seashore",
	"Bird Tweet",
	"Telephone Ring",
	"Helicopter",
	"Applause",
	"Gunshot"
};

ChannelOutputNode::ChannelOutputNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mLastUpdate( 0 )
{
	FUGID( PIN_INPUT_CHANNEL,	"c6a297c9-09b3-439a-9d8f-11b44355c419" );
	FUGID( PIN_INPUT_PROGRAM,	"4366d9dc-6f60-4d3c-9de4-9875c9d379da" );
	FUGID( PIN_OUTPUT_MIDI,		"5b952893-45c7-4e51-995c-d3ae564a9fb8" );

	mPinInputChannel = pinInput( "Channel", PIN_INPUT_CHANNEL );

	mPinInputChannel->registerPinInputType( PID_INTEGER );

	mValInputProgram = pinInput<fugio::ChoiceInterface *>( "Program", mPinInputProgram, PID_CHOICE, PIN_INPUT_PROGRAM );

	mValInputProgram->setChoices( PrgLst );

	mPinInputProgram->setSetting( "midi-type", "program" );

	mValOutputMidi = pinOutput<fugio::MidiInterface *>( "MIDI", mPinOutputMidi, PID_MIDI_OUTPUT, PIN_OUTPUT_MIDI );
}

bool ChannelOutputNode::initialise()
{
	if( !NodeControlBase::initialise() )
	{
		return( false );
	}

	connect( mNode->context()->qobject(), SIGNAL(frameFinalise(qint64)), this, SLOT(frameFinalised(qint64)) );

	connect( mNode->qobject(), SIGNAL(pinAdded(QSharedPointer<fugio::PinInterface>)), this, SLOT(pinAdded(QSharedPointer<fugio::PinInterface>)) );

	return( true );
}

bool ChannelOutputNode::deinitialise()
{
	disconnect( mNode->context()->qobject(), SIGNAL(frameFinalise(qint64)), this, SLOT(frameFinalised(qint64)) );

	return( NodeControlBase::deinitialise() );
}

QList<QUuid> ChannelOutputNode::pinAddTypesInput() const
{
	static QList<QUuid> PinLst =
	{
		PID_INTEGER
	};

	return( PinLst );
}

bool ChannelOutputNode::canAcceptPin( fugio::PinInterface *pPin ) const
{
	return( pPin->direction() == PIN_OUTPUT );
}

void ChannelOutputNode::pinAdded( QSharedPointer<fugio::PinInterface> pPin )
{
	pPin->setSetting( "midi-type", pPin->name() );
}

QStringList ChannelOutputNode::availableInputPins() const
{
	static QStringList		InputPins;
	QStringList				PinLst;

	if( InputPins.isEmpty() )
	{
		for( int i = 0 ; i < 128 ; i++ )
		{
			InputPins.append( QString( "control/%1" ).arg( i ) );
		}

		for( int i = 0 ; i < 128 ; i++ )
		{
			InputPins.append( QString( "note/%1" ).arg( i ) );
		}

		InputPins.append( "pitch-bend" );
		InputPins.append( "program" );
		InputPins.append( "channel-pressure" );
	}

	for( QString S : InputPins )
	{
		if( mNode->findInputPinByName( S ) )
		{
			continue;
		}

		PinLst << S;
	}

	std::sort( PinLst.begin(), PinLst.end() );

	return( PinLst );

}

bool ChannelOutputNode::mustChooseNamedInputPin() const
{
	return( true );
}

void ChannelOutputNode::frameFinalised( qint64 pTimeStamp )
{
	int		Channel = qBound( 1, variant( mPinInputChannel ).toInt(), 16 ) - 1;

	mValOutputMidi->clearData();

	for( QSharedPointer<fugio::PinInterface> P : mNode->enumInputPins() )
	{
		if( P->localId() == mPinInputChannel->localId() )
		{
			continue;
		}

		if( !P->isUpdated( mLastUpdate ) )
		{
			continue;
		}

		QVariant		V = variant( P );

		if( V.isValid() )
		{
			const QString		N = P->setting( "midi-type", P->name() ).toString();

			QStringList			PrtLst = N.split( '/' );

			if( PrtLst.size() == 1 )
			{
				if( PrtLst.first() == "program" )
				{
					int			NV = 0;

					if( QMetaType::Type( V.type() ) == QMetaType::Int )
					{
						NV = V.toInt();
					}
					else if( QMetaType::Type( V.type() ) == QMetaType::QString )
					{
						const QString		S = V.toString();

						for( int i = 0 ; i < PrgLst.size() ; i++ )
						{
							if( PrgLst.at( i ) == S )
							{
								NV = i;

								break;
							}
						}
					}

					NV = qBound( 0, NV, 127 );

					fugio::MidiEvent		EV;

					EV.message   = Pm_Message( MIDI_PROGRAM + Channel, NV, 0 );
					EV.timestamp = 0;

					mValOutputMidi->addMessage( EV.message );

					continue;
				}

				if( PrtLst.first() == "channel-pressure" )
				{
					int			NV = V.toInt();

					NV = qBound( 0, NV, 127 );

					fugio::MidiEvent		EV;

					EV.message   = Pm_Message( MIDI_CHANNEL_AT + Channel, NV, 0 );
					EV.timestamp = 0;

					mValOutputMidi->addMessage( EV.message );

					continue;
				}

				if( PrtLst.first() == "pitch-bend" )
				{
					int			NV = V.toInt();

					NV = qBound( 0, NV, 16383 );

					fugio::MidiEvent		EV;

					EV.message   = Pm_Message( MIDI_PITCHBEND + Channel, ( NV >> 0 ) & 0x7f, ( NV >> 7 ) & 0x7f );
					EV.timestamp = 0;

					mValOutputMidi->addMessage( EV.message );

					continue;
				}
			}
			else if( PrtLst.size() == 2 )
			{
				if( PrtLst.first() == "control" )
				{
					int			CC = PrtLst.last().toInt();
					int			MV = V.toInt();

					CC = qBound( 0, CC, 127 );
					MV = qBound( 0, MV, 127 );

					fugio::MidiEvent		EV;

					EV.message   = Pm_Message( MIDI_CONTROL + Channel, CC, MV );
					EV.timestamp = 0;

					mValOutputMidi->addMessage( EV.message );

					continue;
				}

				if( PrtLst.first() == "note-pressure" )
				{
					int			NN = PrtLst.last().toInt();
					int			NV = V.toInt();

					NN = qBound( 0, NN, 127 );
					NV = qBound( 0, NV, 127 );

					fugio::MidiEvent		EV;

					EV.message   = Pm_Message( MIDI_POLY_AT + Channel, NN, NV );
					EV.timestamp = 0;

					mValOutputMidi->addMessage( EV.message );

					continue;
				}

				if( PrtLst.first().startsWith( "note" ) )
				{
					int			NN = PrtLst.last().toInt();
					int			NV = 0;

					if( PrtLst.first() == "note" )
					{
						NV = V.toInt();
					}
					else if( PrtLst.first() == "note-on" )
					{
						NV = 127;
					}

					NN = qBound( 0, NN, 127 );
					NV = qBound( 0, NV, 127 );

					fugio::MidiEvent		EV;

					EV.message   = Pm_Message( ( NV > 0 ? MIDI_NOTE_ON : MIDI_NOTE_ON ) + Channel, NN, NV );
					EV.timestamp = 0;

					mValOutputMidi->addMessage( EV.message );

					continue;
				}
			}
		}
	}

	if( mValOutputMidi->hasMessages() || mValOutputMidi->hasSysEx() )
	{
		pinUpdated( mPinOutputMidi );
	}

	mLastUpdate = pTimeStamp;
}

