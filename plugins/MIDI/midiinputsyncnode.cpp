#include "midiinputsyncnode.h"

#include <fugio/core/uuid.h>
#include <fugio/midi/uuid.h>

MidiInputSyncNode::MidiInputSyncNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mMidiPlay( false ), mMidiPlayOnNext( false ),
	  mSngPosPtr( 0 ), mMidiClockCount( 0 ), mMidiClockPeriod( 0 ), mMidiClockBPM( 0 )
{
	FUGID( PIN_INPUT_MIDI,			"47D0B254-9CD9-46A9-891C-2D4483C3ECE6" );
	FUGID( PIN_OUTPUT_SYNC,			"1FFD4966-7715-4457-A83E-4601A72C4E4F" );
	FUGID( PIN_OUTPUT_BPM,			"72A75653-5B79-4147-9A3E-7DF341942F52" );

	mValInputMidi = pinInput<fugio::MidiInputInterface *>( "MIDI", mPinInputMidi, PID_MIDI_INPUT, PIN_INPUT_MIDI );

	mValSync = pinOutput<fugio::VariantInterface *>( "Sync", mPinSync, PID_FLOAT, PIN_OUTPUT_SYNC );

	mValBPM  = pinOutput<fugio::VariantInterface *>( "BPM", mPinBPM, PID_FLOAT, PIN_OUTPUT_BPM );
}

void MidiInputSyncNode::midiProcessInput( const fugio::MidiEvent *pMessages, quint32 pMessageCount )
{
	fugio::ListInterface		*BufDat = input<fugio::ListInterface *>( mPinInputMidi );

	if( BufDat )
	{
		for( int i = 0 ; i < BufDat->listSize() ; i++ )
		{
			const QByteArray	&BA = BufDat->listIndex( i ).toByteArray();

			if( BA.size() == 8 )
			{
				if( BA[ 0 ] == 0x7f && BA[ 1 ] == 0x7f && BA[ 2 ] == 0x01 && BA[ 3 ] == 0x01 )
				{
					mst = ( BA[ 4 ] & 0x60 ) >> 5;
					mhr = ( BA[ 4 ] & 0x1f );
					mmn = BA[ 5 ];
					msc = BA[ 6 ];
					mfr = BA[ 7 ];

					processMTC( mhr, mmn, msc, mfr, mst, false );
				}
			}
		}
	}
	else
	{
		for( quint32 m = 0 ; m < pMessageCount ; m++ )
		{
			const fugio::MidiEvent &CurEvt = pMessages[ m ];

			const quint8			 MsgStatus  = Pm_MessageStatus( CurEvt.message ) & 0xf0;

			switch( MsgStatus )
			{
				case 0xF1:		// MTC Quarter Frame
					{
						quint8		DataType = Pm_MessageData1( CurEvt.message ) >> 4;
						quint8		DataByte = Pm_MessageData1( CurEvt.message ) & 0x0f;

						switch( DataType )
						{
							case 0:
								mfr = ( mfr & 0xf0 ) | ( DataByte << 0 );
								break;

							case 1:
								mfr = ( mfr & 0x0f ) | ( DataByte << 4 );
								break;

							case 2:
								msc = ( msc & 0xf0 ) | ( DataByte << 0 );
								break;

							case 3:
								msc = ( msc & 0x0f ) | ( DataByte << 4 );
								break;

							case 4:
								mmn = ( mmn & 0xf0 ) | ( DataByte << 0 );
								break;

							case 5:
								mmn = ( mmn & 0x0f ) | ( DataByte << 4 );
								break;

							case 6:
								mhr = ( mhr & 0xf0 ) | ( DataByte << 0 );
								break;

							case 7:
								mhr = ( mhr & 0x0f ) | ( ( DataByte & 0x01 ) << 4 );
								mst = ( DataByte & 0x06 ) >> 1;

								processMTC( mhr, mmn, msc, mfr, mst, true );

								break;
						}

						mls = DataType;
					}
					break;

				case 0xF2:		// Song Position Ptr
					{
						mSngPosPtr = CombineBytes( Pm_MessageData1( CurEvt.message ), Pm_MessageData2( CurEvt.message ) );

						mMidiClockCount = mSngPosPtr * 6;
					}
					break;

				case 0xF8:		// MIDI Clock
					{
						processMidiClockMessage( CurEvt.timestamp );
					}
					break;

				case 0xF9:		// MIDI Tick
					break;

				case 0xFA:		// MIDI Start
					mMidiClockCount = 0;

					mMidiTime = 0;

					mMidiPlay       = false;
					mMidiPlayOnNext = true;
					break;

				case 0xFB:		// MIDI Continue
					mMidiPlay       = false;
					mMidiPlayOnNext = true;
					break;

				case 0xFC:		// MIDI Stop
					mMidiPlay = false;
					mMidiPlayOnNext = false;
					break;
			}
		}
	}
}

void MidiInputSyncNode::processMTC( int h, int m, int s, int f, int t, bool pPlay )
{
	qreal	ts = 0;

	ts += qreal( h ) * 24.0 * 60.0;
	ts += qreal( m ) * 60.0;
	ts += qreal( s );

	switch( t )
	{
		case 0:		ts += qreal( f ) / 24.0;	break;
		case 1:		ts += qreal( f ) / 25.0;	break;
		case 2:		ts += qreal( f ) / 30.0;	break;
		case 3:		ts += qreal( f ) / 30.0;	break;
	}

	mMidiTime = ts;

	if( pPlay )
	{
		mMidiPlay = true;
	}
}

void MidiInputSyncNode::processMidiClockMessage(const fugio::PmTimestamp EV)
{
	if( mMidiPlayOnNext )
	{
		mMidiClockEvents.clear();

		mMidiPlay = true;

		mMidiPlayOnNext = false;
	}

	if( mMidiPlay )
	{
		mMidiClockCount++;
	}

	mMidiClockEvents.append( EV );

	if( mMidiClockEvents.size() < 2 )
	{
		return;
	}

	while( mMidiClockEvents.size() > 24 )
	{
		mMidiClockEvents.removeFirst();
	}

	fugio::PmTimestamp TS = mMidiClockEvents.first();
	fugio::PmTimestamp TE = mMidiClockEvents.last();

	qreal		Sync = qreal( mMidiClockEvents.size() - 1 ) / 24.0;

	mMidiClockPeriod = qreal( TE - TS ) / Sync;
	mMidiClockBPM    = qRound( 60000.0 / mMidiClockPeriod );
}
