#include "mididecodernode.h"

#include <fugio/core/uuid.h>
#include <fugio/midi/uuid.h>

#include <QCheckBox>
#include <QSettings>

MidiDecoderNode::MidiDecoderNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mSysEx( false )
{
	FUGID( PIN_INPUT_MIDI,			"47D0B254-9CD9-46A9-891C-2D4483C3ECE6" );
	FUGID( PIN_OUTPUT_SYSTEM,		"c09265b7-71a8-4428-b519-1ee47c135628" );
	FUGID( PIN_OUTPUT_SYSEX,		"334D262B-A897-468C-851B-37900F20B4C0" );
	FUGID( PIN_OUTPUT_CHANNEL_01,	"2262107c-acc5-41d3-b23a-528944685d70" );

	mValInputMidi = pinInput<fugio::MidiInputInterface *>( "MIDI", mPinInputMidi, PID_MIDI_INPUT, PIN_INPUT_MIDI );

	mValOutputSystem = pinOutput<fugio::MidiInterface *>( "System", mPinOutputSystem, PID_MIDI_OUTPUT, PIN_OUTPUT_SYSTEM );

	mValOutputSysEx = pinOutput<fugio::VariantInterface *>( "SysEx", mPinOutputSysEx, PID_BYTEARRAY, PIN_OUTPUT_SYSEX );

	mOutputChannels.resize( 16 );

	connect( mNode->qobject(), SIGNAL(pinAdded(QSharedPointer<fugio::PinInterface>)), this, SLOT(pinAdded(QSharedPointer<fugio::PinInterface>)) );

	MidiPinPair		MPP;

	MPP.second = pinOutput<fugio::MidiInterface *>( "Channel 01", MPP.first, PID_MIDI_OUTPUT, PIN_OUTPUT_CHANNEL_01 );
}

void MidiDecoderNode::midiProcessInput( const fugio::MidiEvent *pMessages, quint32 pMessageCount )
{
	for( int i = 0 ; i < 16 ; i++ )
	{
		MidiPinPair		&MPP = mOutputChannels[ i ];

		if( MPP.second && MPP.second->hasMessages() )
		{
			MPP.second->clearData();
		}
	}

	if( mValOutputSystem->hasMessages() )
	{
		mValOutputSystem->clearData();
	}

	mValOutputSysEx->variantClear();

	mSysExEvents.clear();

	for( quint32 i = 0 ; i < pMessageCount ; i++ )
	{
		const fugio::MidiEvent	&CurEvt = pMessages[ i ];

		const quint8			 MsgStatus  = Pm_MessageStatus( CurEvt.message ) & 0xf0;

		// Have we received a command outside of SysEx?

		if( mSysEx && MsgStatus >= 0x80 )
		{
			processSysEx();
		}

		// If we're currently processing SysEx, pass the message there

		if( mSysEx )
		{
			addSysEx( 0, CurEvt );

			continue;
		}

		// Else, make sure we've cleared the SysEx buffer

		processSysEx();

		// Check for invalid command

		if( MsgStatus < 0x80 )
		{
			continue;
		}

		// Is this a channel message?

		if( MsgStatus >= 0x80 && MsgStatus < 0xf0 )
		{
			processChannelMessage( CurEvt );

			continue;
		}

		// Must be a system message then...

		processSystemMessage( MsgStatus, CurEvt );
	}

	// Update channel pins

	for( int i = 0 ; i < 16 ; i++ )
	{
		MidiPinPair		&MPP = mOutputChannels[ i ];

		if( MPP.second && MPP.second->hasMessages() )
		{
			updateMidiPin( MPP.first, MPP.second );
		}
	}

	// Update system pin

	if( mValOutputSystem->hasMessages() )
	{
		updateMidiPin( mPinOutputSystem, mValOutputSystem );
	}

	if( mValOutputSysEx->variantCount() )
	{
		for( fugio::MidiInputInterface *MII : outputs<fugio::MidiInputInterface *>( mPinOutputSysEx ) )
		{
			MII->midiProcessInput( mSysExEvents.constData(), mSysExEvents.size() );
		}

		pinUpdated( mPinOutputSysEx );
	}
}

void MidiDecoderNode::pinAdded( QSharedPointer<fugio::PinInterface> P )
{
	if( P->direction() == PIN_INPUT )
	{
		return;
	}

	int		Channel = P->setting( "channel", -1 ).toInt();

	for( int i = 0 ; Channel == -1 && i < 16 ; i++ )
	{
		if( P->name() == channelName( i + 1 ) )
		{
			Channel = i;
		}
	}

	if( Channel != -1 )
	{
		MidiPinPair		MPP;

		MPP.first  = P;
		MPP.second = qobject_cast<fugio::MidiInterface *>( P->hasControl() ? P->control()->qobject() : nullptr );

		mOutputChannels[ Channel ] = MPP;

		P->setSetting( "channel", Channel );
	}
}

QString MidiDecoderNode::channelName( int pChannel ) const
{
	return( tr( "Channel %1" ).arg( pChannel, int( 2 ), int( 10 ), QChar( '0' ) ) );
}

void MidiDecoderNode::processSysEx()
{
	if( !mSysExCurrent.isEmpty() )
	{
		clearSysEx();
	}
}

void MidiDecoderNode::processChannelMessage( const fugio::MidiEvent &pMidiEvent )
{
	const quint8	MsgChannel = Pm_MessageStatus( pMidiEvent.message ) & 0x0f;

	MidiPinPair		MPP = mOutputChannels[ MsgChannel ];

	if( MPP.second )
	{
		MPP.second->addMessage( pMidiEvent.message );
	}
}

void MidiDecoderNode::processSystemMessage( const quint8 pMsgStatus, const fugio::MidiEvent &pMidiEvent )
{
	mValOutputSystem->addMessage( pMidiEvent.message );

	// SysEx Start

	if( pMsgStatus == MIDI_SYSEX )
	{
		mSysEx = true;

		addSysEx( 1, pMidiEvent );
	}
}

void MidiDecoderNode::updateMidiPin( QSharedPointer<fugio::PinInterface> pPin, fugio::MidiInterface *pVal )
{
	for( fugio::MidiInputInterface *MII : outputs<fugio::MidiInputInterface *>( pPin ) )
	{
		MII->midiProcessInput( pVal->messages().constData(), pVal->messages().size() );
	}

	pinUpdated( pPin );
}

QList<fugio::NodeControlInterface::AvailablePinEntry> MidiDecoderNode::availableOutputPins() const
{
	QList<fugio::NodeControlInterface::AvailablePinEntry>	PinLst;

	for( int i = 1 ; i <= 16 ; i++ )
	{
		fugio::NodeControlInterface::AvailablePinEntry APE( channelName( i ), PID_MIDI_OUTPUT );

		if( !mNode->findOutputPinByName( APE.mName ) )
		{
			PinLst << APE;
		}
	}

	return( PinLst );
}

bool MidiDecoderNode::mustChooseNamedOutputPin() const
{
	return( true );
}

QList<QUuid> MidiDecoderNode::pinAddTypesOutput() const
{
	static QList<QUuid> PinLst =
	{
		PID_MIDI_OUTPUT
	};

	return( PinLst );
}
