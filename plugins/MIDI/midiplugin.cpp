#include "midiplugin.h"

#include <QTranslator>
#include <QApplication>
#include <QFile>

#include <fugio/global_interface.h>
#include <fugio/global_signals.h>

#include <fugio/midi/uuid.h>
#include <fugio/timeline/uuid.h>

#include <fugio/editor_interface.h>
#include <fugio/timeline/timeline_interface.h>

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

#include "midifileloadnode.h"

#include "import/midifile.h"

#include "miditimelinenode.h"

QList<QUuid>			 NodeControlBase::PID_UUID;

MidiPlugin				*MidiPlugin::mInstance = 0;

ClassEntry	NodeClasses[] =
{
	ClassEntry( "MIDI Decoder",				"MIDI", NID_MIDI_INPUT,  &MidiDecoderNode::staticMetaObject ),
//	ClassEntry( "MIDI File Load",			"MIDI", NID_MIDI_FILE_LOAD,  &MidiFileLoadNode::staticMetaObject ),
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

ClassEntry	TimelineNodeClasses[] =
{
	ClassEntry( "MIDI Timeline",		"MIDI", NID_MIDI_TIMELINE, &MidiTimelineNode::staticMetaObject ),
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

	if( Translator.load( QLocale(), QLatin1String( "translations" ), QLatin1String( "_" ), ":/" ) )
	{
		qApp->installTranslator( &Translator );
	}

	//-------------------------------------------------------------------------

	MidiOutputPin::registerMetaType();
}

PluginInterface::InitResult MidiPlugin::initialise( fugio::GlobalInterface *pApp, bool pLastChance )
{
	fugio::EditorInterface	*EI = qobject_cast<fugio::EditorInterface *>( pApp->findInterface( IID_EDITOR ) );

	if( !EI && !pLastChance )
	{
		return( INIT_DEFER );
	}

	fugio::TimelineInterface	*TI = qobject_cast<fugio::TimelineInterface *>( pApp->findInterface( IID_TIMELINE ) );

	if( !TI && !pLastChance )
	{
		return( INIT_DEFER );
	}

	mInstance = this;

	mApp = pApp;

	mApp->registerNodeClasses( NodeClasses );

	mApp->registerPinClasses( PinClasses );

	if( EI && TI )
	{
		EI->menuAddFileImporter( "Midi (*.mid)", &MidiPlugin::midiFileImportStatic );
	}

	if( TI )
	{
		mApp->registerNodeClasses( TimelineNodeClasses );
	}

	return( INIT_OK );
}

void MidiPlugin::deinitialise( void )
{
	mApp->unregisterNodeClasses( TimelineNodeClasses );

	mApp->unregisterPinClasses( PinClasses );

	mApp->unregisterNodeClasses( NodeClasses );

	mApp = 0;
}

bool MidiPlugin::midiFileImportStatic( QString pFilename, fugio::ContextInterface *pContext )
{
	return( MidiPlugin::instance()->midiFileImport( pFilename, pContext ) );
}

bool MidiPlugin::midiFileImport( QString pFilename, fugio::ContextInterface *pContext )
{
	//fugio::EditorInterface	*EI = qobject_cast<fugio::EditorInterface *>( mApp->findInterface( IID_EDITOR ) );

	QFile		FH( pFilename );

	if( !FH.open( QFile::ReadOnly ) )
	{
		return( false );
	}

	QByteArray	FD = FH.readAll();

	FH.close();

	MidiFile	MF;

	MF.parseMidiData( FD );

	qreal	CtxDur = pContext->duration();
	qreal	NewDur = CtxDur;

#if defined( TIMELINE_SUPPORTED )
	for( int i = 0 ; i < 16 ; i++ )
	{
		qreal	TrkDur = qreal( MF.duration( i ) ) / 1000.0;

		NewDur = std::max( TrkDur, NewDur );

		const QList<fugio::MidiEvent> &EL = MF.events( i );

		if( EL.isEmpty() )
		{
			continue;
		}

		QSharedPointer<fugio::NodeInterface>	TrackNode;

		TrackNode = pContext->createNode( tr( "Track %1" ).arg( i + 1 ), QUuid::createUuid(), NID_MIDI_TIMELINE );

		if( TrackNode )
		{
			MidiTimelineNode	*Timeline = qobject_cast<MidiTimelineNode *>( TrackNode->control()->qobject() );

			if( Timeline )
			{
				Timeline->setEvents( EL );
			}

			pContext->registerNode( TrackNode );
		}
	}
#endif

	if( NewDur > CtxDur )
	{
		pContext->setDuration( NewDur );
	}

	return( false );
}
