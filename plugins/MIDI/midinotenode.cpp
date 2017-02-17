#include "midinotenode.h"

#include <fugio/context_interface.h>
#include <fugio/core/variant_interface.h>
#include <fugio/midi/uuid.h>

MidiNoteNode::MidiNoteNode( QSharedPointer<fugio::NodeInterface> pNode ) :
	NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_CHANNEL, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_INPUT_NOTE, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );
	FUGID( PIN_OUTPUT_MIDI, "261cc653-d7fa-4c34-a08b-3603e8ae71d5" );

	mPinChannel = pinInput( "Channel", PIN_INPUT_CHANNEL );

	QSharedPointer<fugio::PinInterface>			 PinNote;

	PinNote = pinInput( "Note", PIN_INPUT_NOTE );

	mValOutputMidi = pinOutput<fugio::MidiInterface *>( "MIDI", mPinOutputMidi, PID_MIDI_OUTPUT, PIN_OUTPUT_MIDI );
}

void MidiNoteNode::inputsUpdated( qint64 pTimeStamp )
{
	const int Channel = qBound( 1, variant( mPinChannel ).toInt(), 16 ) - 1;

	for( QSharedPointer<fugio::PinInterface> P : mNode->enumInputPins() )
	{
		if( !P->isUpdated( pTimeStamp ) )
		{
			continue;
		}

		int			Note = qBound( 0, variant( P ).toInt(), 127 );
		int			Last = mLastNote.value( P->localId(), -1 );

		Note = qBound( 0, Note, 127 );

//		if( Note == Last )
//		{
//			return;
//		}

		if( Last != -1 )
		{
			mValOutputMidi->addMessage( Pm_Message( MIDI_NOTE_ON + Channel, Last, 0x00 ) );
		}

		mValOutputMidi->addMessage( Pm_Message( MIDI_NOTE_ON + Channel, Note, 0x7f ) );

		mLastNote.insert( P->localId(), Note );
	}

	pinUpdated( mPinOutputMidi );
}


QList<QUuid> MidiNoteNode::pinAddTypesInput() const
{
	static QList<QUuid> PinLst =
	{
		PID_INTEGER
	};

	return( PinLst );
}

bool MidiNoteNode::canAcceptPin( fugio::PinInterface *pPin ) const
{
	return( pPin->direction() == PIN_OUTPUT );
}
