#include "midiplugin.h"

#include <QTranslator>
#include <QApplication>

#include <fugio/global_interface.h>
#include <fugio/global_signals.h>

#include <fugio/midi/uuid.h>

#include "midinotenode.h"
#include "midioutputhelpernode.h"
#include "midirotarynode.h"
#include "midiinputhelpernode.h"

#include "notetofrequencynode.h"
#include "frequencytonotenode.h"
#include "scalenode.h"
#include "intervalnode.h"
#include "channeloutputnode.h"
#include "channelinputnode.h"
#include "mididecodernode.h"
#include "midiinputsyncnode.h"

#include "midioutputpin.h"
#include "midiinputpin.h"

QList<QUuid>	NodeControlBase::PID_UUID;

ClassEntry	NodeClasses[] =
{
	ClassEntry( "MIDI Decoder",				"MIDI", NID_MIDI_INPUT,  &MidiDecoderNode::staticMetaObject ),
	ClassEntry( "MIDI Input Helper",		"MIDI", NID_MIDI_INPUT_HELPER,  &MidiInputHelperNode::staticMetaObject ),
	ClassEntry( "MIDI Input Sync",			"MIDI", NID_MIDI_INPUT_SYNC,  &MidiInputSyncNode::staticMetaObject ),
	ClassEntry( "MIDI Note",				"MIDI", NID_MIDI_NOTE, &MidiNoteNode::staticMetaObject ),
	ClassEntry( "MIDI Output Helper",		"MIDI", NID_MIDI_OUTPUT_HELPER, &MidiOutputHelperNode::staticMetaObject ),
	ClassEntry( "MIDI Rotary Control",		"MIDI", NID_MIDI_ROTARY_CONTROL, &MidiRotaryNode::staticMetaObject ),
	ClassEntry( "MIDI Note To Frequency",	"MIDI", NID_MIDI_NOTE_TO_FREQUENCY, &NoteToFrequencyNode::staticMetaObject ),
	ClassEntry( "Frequency To MIDI Note",	"MIDI", NID_MIDI_FREQUENCY_TO_NOTE, &FrequencyToNoteNode::staticMetaObject ),
	ClassEntry( "MIDI Scale",				"MIDI", NID_MIDI_SCALE, &ScaleNode::staticMetaObject ),
	ClassEntry( "MIDI Interval",			"MIDI", NID_MIDI_INTERVAL, &IntervalNode::staticMetaObject ),
	ClassEntry( "MIDI Channel Output",		"MIDI", NID_MIDI_CHANNEL_OUTPUT, &ChannelOutputNode::staticMetaObject ),
	ClassEntry( "MIDI Channel Input",		"MIDI", NID_MIDI_CHANNEL_INPUT, &ChannelInputNode::staticMetaObject ),
	ClassEntry()
};

ClassEntry PinClasses[] =
{
	ClassEntry( "MIDI Output Pin", PID_MIDI_OUTPUT, &MidiOutputPin::staticMetaObject ),
	ClassEntry( "MIDI Input Pin", PID_MIDI_INPUT, &MidiInputPin::staticMetaObject ),
	ClassEntry()
};

MidiPlugin::MidiPlugin()
{
	//-------------------------------------------------------------------------
	// Install translator

	static QTranslator		Translator;

	if( Translator.load( QLocale(), QLatin1String( "fugio_midi" ), QLatin1String( "_" ), ":/translations" ) )
	{
		qApp->installTranslator( &Translator );
	}
}

PluginInterface::InitResult MidiPlugin::initialise( fugio::GlobalInterface *pApp, bool pLastChance )
{
	Q_UNUSED( pLastChance )

	mApp = pApp;

	mApp->registerNodeClasses( NodeClasses );

	mApp->registerPinClasses( PinClasses );

	return( INIT_OK );
}

void MidiPlugin::deinitialise( void )
{
	mApp->unregisterPinClasses( PinClasses );

	mApp->unregisterNodeClasses( NodeClasses );

	mApp = 0;
}
