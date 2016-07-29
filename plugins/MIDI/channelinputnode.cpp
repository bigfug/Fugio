#include "channelinputnode.h"

#include <QPushButton>

#include <fugio/core/uuid.h>
#include <fugio/midi/uuid.h>
#include <fugio/context_signals.h>

ChannelInputNode::ChannelInputNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mListen( false ), mRPNState( NONE ), mRPNCount( 0 )
{
	FUGID( PIN_INPUT_MIDI,	"b85ce7d2-b21b-49da-861e-74d22e63d50b" );

	mValInputMidi = pinInput<fugio::MidiInputInterface *>( "MIDI", mPinInputMidi, PID_MIDI_INPUT, PIN_INPUT_MIDI );
}

bool ChannelInputNode::initialise()
{
	if( !NodeControlBase::initialise() )
	{
		return( false );
	}

	for( QSharedPointer<fugio::PinInterface> P : mNode->enumOutputPins() )
	{
		const QString		MidiType = P->setting( "midi-type", QString() ).toString();

		if( MidiType.isEmpty() )
		{
			continue;
		}

		if( MidiType.indexOf( '/' ) != -1 )
		{
			const QStringList	TypeList = MidiType.split( '/' );

			if( TypeList.size() == 2 )
			{
				const QString		SubType  = TypeList.at( 0 );
				const int			SubValue = TypeList.at( 1 ).toInt();

				if( SubType == "note" )
				{
					MidiPinPair		MPP;

					pinToMidiEntry( P, MPP );

					mNotePins.insert( SubValue, MPP );

					continue;
				}

				if( SubType == "note-on" )
				{
					MidiPinPair		MPP;

					pinToMidiEntry( P, MPP );

					mNoteOnPins.insert( SubValue, MPP );

					continue;
				}

				if( SubType == "note-off" )
				{
					MidiPinPair		MPP;

					pinToMidiEntry( P, MPP );

					mNoteOffPins.insert( SubValue, MPP );

					continue;
				}

				if( SubType == "note-pressure" )
				{
					MidiPinPair		MPP;

					pinToMidiEntry( P, MPP );

					mNotePressurePins.insert( SubValue, MPP );

					continue;
				}

				if( SubType == "control" )
				{
					MidiPinPair		MPP;

					pinToMidiEntry( P, MPP );

					mControlsPins.insert( SubValue, MPP );

					continue;
				}

				if( SubType == "program" )
				{
					MidiPinPair		MPP;

					pinToMidiEntry( P, MPP );

					mProgramPins.insert( SubValue, MPP );

					continue;
				}

				if( SubType == "channel-pressure" )
				{
					MidiPinPair		MPP;

					pinToMidiEntry( P, MPP );

					mChannelPressurePins.insert( SubValue, MPP );

					continue;
				}
			}

			continue;
		}

		if( MidiType == "notes-active" )
		{
			pinToMidiEntry( P, mNotesActive );

			continue;
		}

		if( MidiType == "note-value" )
		{
			pinToMidiEntry( P, mNoteValue );

			continue;
		}

		if( MidiType == "pitch-bend" )
		{
			pinToMidiEntry( P, mPitchBend );

			continue;
		}
	}

	return( true );
}

void ChannelInputNode::pinToMidiEntry( QSharedPointer<fugio::PinInterface> P, MidiPinPair &MPP )
{
	MPP.mPin = P;
	MPP.mVal = qobject_cast<fugio::VariantInterface *>( P->hasControl() ? P->control()->qobject() : nullptr );
}

void ChannelInputNode::setRPNControl( quint16 pValue )
{
	qDebug() << CombineBytes( mRPNParamLSB, mRPNParamMSB ) << pValue;
}

QList<fugio::NodeControlInterface::AvailablePinEntry> ChannelInputNode::availableOutputPins() const
{
	static QList<fugio::NodeControlInterface::AvailablePinEntry>		PinLst;

	return( PinLst );
}

bool ChannelInputNode::mustChooseNamedOutputPin() const
{
	return( true );
}

void ChannelInputNode::midiProcessInput( const fugio::MidiEvent *pMessages, quint32 pMessageCount )
{
	const int				NoteCount1 = mNoteMap.size();

	MidiEntryMap::iterator	PinItr;
	bool					SortPins = false;

	for( quint32 i = 0 ; i < pMessageCount ; i++ )
	{
		const fugio::MidiEvent	&CurEvt = pMessages[ i ];

		const quint8		MsgStatus  = Pm_MessageStatus( CurEvt.message ) & 0xf0;
		const quint8		MsgData1   = Pm_MessageData1( CurEvt.message );
		const quint8		MsgData2   = Pm_MessageData2( CurEvt.message );

		if( MsgStatus != MIDI_CONTROL && mRPNState != NONE )
		{
			mRPNState = NONE;
			mRPNCount = 0;
		}

		switch( MsgStatus )
		{
			case MIDI_NOTE_OFF:
				{
					PinItr = findOrCreatePin( QString( "note/%1" ).arg( MsgData1 ), mNotePins, MsgData1, mListen, SortPins, PID_INTEGER );

					if( PinItr != mNotePins.end() && PinItr->mVal )
					{
						PinItr->mVal->setVariant( 0 );

						pinUpdated( PinItr->mPin );
					}

					PinItr = findOrCreatePin( QString( "note-off/%1" ).arg( MsgData1 ), mNoteOffPins, MsgData1, mListen, SortPins, PID_TRIGGER );

					if( PinItr != mNoteOffPins.end() && PinItr->mPin )
					{
						pinUpdated( PinItr->mPin );
					}

					mNoteMap.remove( MsgData1 );
				}
				break;

			case MIDI_NOTE_ON:
				{
					if( !mNoteValue.mVal && mListen )
					{
						createMidiPin( mNotesActive, "note-value", PID_INTEGER );

						SortPins = true;
					}

					if( mNoteValue.mVal )
					{
						PinItr->mVal->setVariant( MsgData1 );

						pinUpdated( PinItr->mPin );
					}

					PinItr = findOrCreatePin( QString( "note/%1" ).arg( MsgData1 ), mNotePins, MsgData1, mListen, SortPins, PID_INTEGER );

					if( PinItr != mNotePins.end() && PinItr->mVal )
					{
						PinItr->mVal->setVariant( MsgData1 );

						pinUpdated( PinItr->mPin );
					}

					if( MsgData2 > 0 )
					{
						PinItr = findOrCreatePin( QString( "note-on/%1" ).arg( MsgData1 ), mNoteOnPins, MsgData1, mListen, SortPins, PID_TRIGGER );

						if( PinItr != mNoteOnPins.end() && PinItr->mPin )
						{
							pinUpdated( PinItr->mPin );
						}

						mNoteMap.insert( MsgData1, MsgData2 );
					}
					else
					{
						mNoteMap.remove( MsgData1 );
					}
				}
				break;

			case MIDI_POLY_AT:		// Key Pressure
				{
					PinItr = findOrCreatePin( QString( "note-pressure/%1" ).arg( MsgData1 ), mNotePressurePins, MsgData1, mListen, SortPins, PID_INTEGER );

					if( PinItr != mNotePressurePins.end() )
					{
						PinItr->mVal->setVariant( MsgData2 );

						pinUpdated( PinItr->mPin );
					}
				}
				break;

			case MIDI_CONTROL:		// Control Change
				{
					if( mRPNCount == 3 )
					{
						if( MsgData1 == 38 )
						{
							mRPNValueLSB   = MsgData2;

							setRPNControl( CombineBytes( mRPNValueLSB, mRPNValueMSB ) );

							mRPNState = NONE;
							mRPNCount = 0;

							continue;
						}
						else
						{
							setRPNControl( CombineBytes( mRPNValueMSB, 0 ) );
						}

						mRPNState = NONE;
						mRPNCount = 0;
					}

					if( mRPNState == NONE )
					{
						if( MsgData1 == 101 )
						{
							mRPNState  = RPN;
							mRPNCount  = 1;
							mRPNParamMSB = MsgData2;

							continue;
						}

						if( MsgData1 == 99 )
						{
							mRPNState  = NRPN;
							mRPNCount  = 1;
							mRPNParamMSB = MsgData2;

							continue;
						}
					}
					else if( mRPNCount == 1 )
					{
						if( mRPNState == RPN && MsgData1 == 100 )
						{
							mRPNCount = 2;
							mRPNParamLSB = MsgData2;

							if( mRPNParamMSB == 127 && mRPNParamLSB == 127 )
							{
								mRPNState = NONE;
								mRPNCount = 0;
							}

							continue;
						}

						if( mRPNState == NRPN && MsgData1 == 98 )
						{
							mRPNCount = 2;
							mRPNParamLSB = MsgData2;

							if( mRPNParamMSB == 127 && mRPNParamLSB == 127 )
							{
								mRPNState = NONE;
								mRPNCount = 0;
							}

							continue;
						}

						if( mRPNState == RPN )
						{
							//setControl( 101, mRPNParam1 );
						}
						else if( mRPNState == NRPN )
						{
							//setControl( 99, mRPNParam1 );
						}

						mRPNState = NONE;
						mRPNCount = 0;
					}
					else
					{
						if( mRPNCount == 2 && MsgData1 == 6 )
						{
							mRPNValueMSB   = MsgData2;
							mRPNCount = 3;

							continue;
						}

						if( mRPNState == RPN )
						{
							//setControl( 101, mRPNParam1 );
							//setControl( 100, mRPNParam2 );
						}
						else if( mRPNState == NRPN )
						{
							//setControl( 99, mRPNParam1 );
							//setControl( 98, mRPNParam2 );
						}

						mRPNState = NONE;
						mRPNCount = 0;
					}

					PinItr = findOrCreatePin( QString( "control/%1" ).arg( MsgData1 ), mControlsPins, MsgData1, mListen, SortPins, PID_INTEGER );

					if( PinItr != mControlsPins.end() )
					{
						PinItr->mVal->setVariant( MsgData2 );

						pinUpdated( PinItr->mPin );
					}
				}
				break;

			case MIDI_PROGRAM:		// Program Number
				{
					PinItr = findOrCreatePin( QString( "program/%1" ).arg( MsgData1 ), mProgramPins, MsgData1, mListen, SortPins, PID_INTEGER );

					if( PinItr != mProgramPins.end() )
					{
						PinItr->mVal->setVariant( MsgData2 );

						pinUpdated( PinItr->mPin );
					}
				}
				break;

			case MIDI_CHANNEL_AT:		// Channel Pressure
				{
					PinItr = findOrCreatePin( QString( "channel-pressure/%1" ).arg( MsgData1 ), mChannelPressurePins, MsgData1, mListen, SortPins, PID_INTEGER );

					if( PinItr != mChannelPressurePins.end() )
					{
						PinItr->mVal->setVariant( MsgData2 );

						pinUpdated( PinItr->mPin );
					}
				}
				break;

			case MIDI_PITCHBEND:		// Pitch Bend
				{
					if( !mPitchBend.mVal && mListen )
					{
						createMidiPin( mPitchBend, "pitch-bend", PID_INTEGER );

						SortPins = true;
					}

					if( mPitchBend.mVal )
					{
						mPitchBend.mVal->setVariant( CombineBytes( MsgData1, MsgData2 ) );

						pinUpdated( mPitchBend.mPin );
					}
				}
				break;
		}
	}

	const int		NoteCount2 = mNoteMap.size();

	if( NoteCount1 && !NoteCount2 )
	{
		if( !mNotesActive.mVal )
		{
			createMidiPin( mNotesActive, "notes-active", PID_BOOL );
		}

		if( mNotesActive.mVal )
		{
			mNotesActive.mVal->setVariant( false );

			pinUpdated( mNotesActive.mPin );
		}
	}
	else if( !NoteCount1 && NoteCount2 )
	{
		if( !mNotesActive.mVal )
		{
			createMidiPin( mNotesActive, "notes-active", PID_BOOL );
		}

		if( mNotesActive.mVal )
		{
			mNotesActive.mVal->setVariant( true );

			pinUpdated( mNotesActive.mPin );
		}
	}

	if( SortPins )
	{
		if( mListen )
		{
			emit listenState( false );

			mListen = false;
		}
	}
}

void ChannelInputNode::setListenState( bool pChecked )
{
	mListen = pChecked;
}

void ChannelInputNode::createMidiPin( MidiPinPair &MPP, const QString &pName, const QUuid &pPinType )
{
	MPP.mVal = qobject_cast<fugio::VariantInterface *>( mNode->createPin( pName, PIN_OUTPUT, QUuid::createUuid(), MPP.mPin, pPinType ) );

	if( MPP.mPin )
	{
		MPP.mPin->setSetting( "midi-type", pName );

		MPP.mPin->setRemovable( true );
	}
}

ChannelInputNode::MidiEntryMap::iterator ChannelInputNode::findOrCreatePin( const QString &pName, MidiEntryMap &pEntryMap, const quint8 pEntryIndex, const bool pListening, bool &pSortPins, const QUuid &pPinType )
{
	MidiEntryMap::iterator		PinItr = pEntryMap.find( pEntryIndex );

	if( pListening && PinItr == pEntryMap.end() )
	{
		MidiPinPair		MPP;

		createMidiPin( MPP, pName, pPinType );

		if( MPP.mPin )
		{
			pEntryMap.insert( pEntryIndex, MPP );

			PinItr = pEntryMap.find( pEntryIndex );

			pSortPins = true;
		}
	}

	return( PinItr );
}

QWidget *ChannelInputNode::gui()
{
	QPushButton		*GUI = new QPushButton( "Listen" );

	if( GUI )
	{
		GUI->setAutoDefault( false );
		GUI->setCheckable( true );

		connect( GUI, SIGNAL(toggled(bool)), this, SLOT(setListenState(bool)) );

		connect( this, SIGNAL(listenState(bool)), GUI, SLOT(setChecked(bool)) );
	}

	return( GUI );
}

