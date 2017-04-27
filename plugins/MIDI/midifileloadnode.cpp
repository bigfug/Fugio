#include "midifileloadnode.h"

#include <QBuffer>
#include <QDataStream>
#include <QtEndian>

#include <fugio/context_interface.h>
#include <fugio/core/variant_interface.h>
#include <fugio/midi/uuid.h>
#include <fugio/context_signals.h>
#include <fugio/midi/midi_input_interface.h>

MidiFileLoadNode::MidiFileLoadNode( QSharedPointer<fugio::NodeInterface> pNode ) :
	NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_FILE_DATA, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
//	FUGID( PIN_INPUT_NOTE, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );
	FUGID( PIN_OUTPUT_MIDI, "261cc653-d7fa-4c34-a08b-3603e8ae71d5" );

	mPinInputFileData = pinInput( "FileData", PIN_INPUT_FILE_DATA );

	mValOutputMidi = pinOutput<fugio::MidiInterface *>( "MIDI", mPinOutputMidi, PID_MIDI_OUTPUT, PIN_OUTPUT_MIDI );
}


bool MidiFileLoadNode::initialise()
{
	if( !NodeControlBase::initialise() )
	{
		return( false );
	}

	connect( node()->context()->qobject(), SIGNAL(frameStart(qint64)), this, SLOT(contextFrame(qint64)) );

	return( true );
}

bool MidiFileLoadNode::deinitialise()
{
	disconnect( node()->context()->qobject(), SIGNAL(frameStart(qint64)), this, SLOT(contextFrame(qint64)) );

	return( NodeControlBase::deinitialise() );
}

void MidiFileLoadNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	if( mPinInputFileData->isUpdated( pTimeStamp ) )
	{
		QByteArray		MidDat = variant( mPinInputFileData ).toByteArray();

		mMidiFile.parseMidiData( MidDat );

		mTimeOffset = pTimeStamp;
		mTimeLast   = -1;
	}
}

void MidiFileLoadNode::contextFrame( qint64 pTimeStamp )
{
	qint64		MidiTime = pTimeStamp - mTimeOffset;

	mValOutputMidi->clearData();

	for( int i = 0 ; i < 16 ; i++ )
	{
		for( const fugio::MidiEvent &SE : mMidiFile.events( i ) )
		{
			if( SE.timestamp > MidiTime )
			{
				break;
			}

			if( SE.timestamp > mTimeLast )
			{
				mValOutputMidi->addMessage( SE.message );
			}
		}
	}

	pinUpdated( mPinOutputMidi );

	for( QSharedPointer<fugio::PinInterface> PI : mPinOutputMidi->connectedPins() )
	{
		if( PI->hasControl() )
		{
			fugio::MidiInputInterface	*MI = qobject_cast<fugio::MidiInputInterface *>( PI->control()->qobject() );

			if( MI )
			{
				MI->midiProcessInput( mValOutputMidi->messages().constData(), mValOutputMidi->messages().size() );
			}
		}
	}

	mTimeLast = MidiTime;
}
