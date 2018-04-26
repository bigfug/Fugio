#include "miditimelinenode.h"

#include <QPainter>

#include <fugio/global_interface.h>
#include <fugio/context_interface.h>
#include <fugio/context_signals.h>
#include <fugio/timeline/uuid.h>
#include <fugio/midi/uuid.h>
#include <fugio/midi/midi_input_interface.h>

#include <fugio/timeline/timeline_interface.h>
#include <fugio/timeline/timeline_control_interface.h>
#include <fugio/timeline/timeline_widget_interface.h>
#include <fugio/timeline/keyframes_provider_interface.h>
#include <fugio/timeline/keyframes_time_interface.h>
#include <fugio/timeline/keyframes_editor_interface.h>
#include <fugio/timeline/keyframes_widget_interface.h>

#include <fugio/timeline/keyframedatatime.h>

#include "midiplugin.h"

MidiTimelineNode::MidiTimelineNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mTimelineControl( 0 ), mKF( 0 )
{
	FUGID( PIN_OUTPUT_MIDI, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );

	mValOutputMidi = pinOutput<fugio::MidiInterface *>( "MIDI", mPinOutputMidi, PID_MIDI_OUTPUT, PIN_OUTPUT_MIDI );

	fugio::TimelineInterface *TL = qobject_cast<fugio::TimelineInterface *>( MidiPlugin::instance()->app()->findInterface( IID_TIMELINE ) );

	if( TL )
	{
		mTimelineControl = TL->control( this );

		mKF = qobject_cast<fugio::KeyFramesTimeInterface *>( TL->keyframes( this, KID_TIME )->qobject() );
	}
}

MidiTimelineNode::~MidiTimelineNode( void )
{
}

void MidiTimelineNode::setEvents(const QList<MidiEvent> &pEventList)
{
	mEvents = pEventList;

	if( mEvents.isEmpty() )
	{
		mKF->setTimeMax( 0 );
	}
	else
	{
		mKF->setTimeMax( qreal( mEvents.last().timestamp ) / 1000.0 );
	}
}

bool MidiTimelineNode::initialise()
{
	if( !NodeControlBase::initialise() )
	{
		return( false );
	}

	if( !mTimelineControl )
	{
		return( false );
	}

	if( !mTimelineControl->initialise() )
	{
		return( false );
	}

	fugio::TimelineWidgetInterface		*TLW = qobject_cast<fugio::TimelineWidgetInterface *>( mNode->context()->findInterface( IID_TIMELINE_WIDGET ) );

	if( TLW )
	{
		TLW->addTrack( this );
	}

	connect( mNode->context()->qobject(), SIGNAL(stateChanged(ContextInterface::TimeState)), this, SLOT(stateChanged(ContextInterface::TimeState)) );
	connect( mNode->context()->qobject(), SIGNAL(aboutToPlay()), this, SLOT(aboutToPlay()) );

	playheadMove( 0 );

	return( true );
}

bool MidiTimelineNode::deinitialise()
{
	if( !mTimelineControl->deinitialise() )
	{
		return( false );
	}

	fugio::TimelineWidgetInterface		*TLW = qobject_cast<fugio::TimelineWidgetInterface *>( mNode->context()->findInterface( IID_TIMELINE_WIDGET ) );

	if( TLW )
	{
		TLW->remTrack( this );
	}

	return( NodeControlBase::deinitialise() );
}

void MidiTimelineNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	if( mTimelineControl )
	{
		mTimelineControl->inputsUpdated( pTimeStamp );
	}
}

void MidiTimelineNode::loadSettings( QSettings &pSettings )
{
	pSettings.beginGroup( "timeline" );

	mTimelineControl->cfgLoad( pSettings );

	pSettings.endGroup();

	pSettings.beginGroup( "time" );

	mKF->cfgLoad( pSettings );

	pSettings.endGroup();

	mEvents.clear();

	int		EvtCnt = pSettings.beginReadArray( "events" );

	mEvents.reserve( EvtCnt );

	fugio::MidiEvent	ME = { 0, 0 };

	for( int i = 0 ; i < EvtCnt ; i++ )
	{
		pSettings.setArrayIndex( i );

		ME.timestamp = pSettings.value( "t" ).toUInt();
		ME.message   = pSettings.value( "m" ).toUInt();

		mEvents << ME;
	}

	pSettings.endArray();

	mKF->setTimeMax( qreal( ME.timestamp ) / 1000.0 );
}

void MidiTimelineNode::saveSettings( QSettings &pSettings ) const
{
	pSettings.beginGroup( "timeline" );

	mTimelineControl->cfgSave( pSettings );

	pSettings.endGroup();

	pSettings.beginGroup( "time" );

	mKF->cfgSave( pSettings );

	pSettings.endGroup();

	pSettings.beginWriteArray( "events", mEvents.size() );

	for( int i = 0 ; i < mEvents.size() ; i++ )
	{
		pSettings.setArrayIndex( i );

		pSettings.setValue( "t", mEvents[ i ].timestamp );
		pSettings.setValue( "m", mEvents[ i ].message );
	}

	pSettings.endArray();
}

qreal MidiTimelineNode::position() const
{
	return( mTimelineControl->position() );
}

qreal MidiTimelineNode::duration() const
{
	return( mNode->context()->duration() );
}

fugio::KeyFramesWidgetInterface *MidiTimelineNode::newTimelineGui()
{
	TimelineInterface *TL = qobject_cast<TimelineInterface *>( mNode->context()->global()->findInterface( IID_TIMELINE ) );

	if( TL )
	{
		KeyFramesEditorInterface	*KFE = qobject_cast<KeyFramesProviderInterface *>( mKF->qobject() )->newEditor();

		if( KFE )
		{
			KeyFramesWidgetInterface	*KFW = TL->keyframesWidget( this, mTimelineControl, KFE );

			if( KFW )
			{
				KFW->setBackground( this );

				return( KFW );
			}

			delete KFE;
		}
	}

	return( 0 );
}

void MidiTimelineNode::drawBackground( const KeyFramesWidgetInterface *pTrackWidget, const QRect &pUpdateRect, QImage &pBackImage ) const
{
	QPainter		Painter( &pBackImage );

	Painter.drawImage( pUpdateRect, pTrackWidget->viewImage(), pUpdateRect );

	if( !mEvents.isEmpty() )
	{
		Painter.setPen( Qt::red );

		QList<quint8>	Notes;

		int				j = 0;

		for( int i = pUpdateRect.left() ; i <= pUpdateRect.right() ; i++ )
		{
			const qint64	t1 = qint64( mKF->time( pTrackWidget->viewToTimestamp( i ) ) * 1000.0 );

			QList<quint8>	SliceNotes = Notes;

			while( j > 0 )
			{
				if( j < mEvents.size() )
				{
					fugio::MidiEvent	ME = mEvents[ j ];

					if( ME.timestamp <= t1 )
					{
						break;
					}
				}

				j--;
			}

			while( j < mEvents.size() )
			{
				fugio::MidiEvent	ME = mEvents[ j ];

				if( ME.timestamp > t1 )
				{
					break;
				}

				quint8	Status = Pm_MessageStatus( ME.message ) & 0xf0;
				quint8	Data1  = Pm_MessageData1( ME.message );
				quint8	Data2  = Pm_MessageData2( ME.message );

				if( Status == MIDI_NOTE_ON && Data2 > 0 )
				{
					Notes.removeAll( Data1 );
					Notes.append( Data1 );

					SliceNotes.removeAll( Data1 );
					SliceNotes.append( Data1 );
				}
				else if( Status == MIDI_NOTE_OFF || ( Status == MIDI_NOTE_ON && Data2 == 0 ) )
				{
					Notes.removeAll( Data1 );
				}

				j++;
			}

			if( i > pUpdateRect.left() )
			{
				for( quint8 n : SliceNotes )
				{
					qint32			y1 = pTrackWidget->valueToView( qreal( n ) / 127.0 );

					Painter.drawPoint( i, y1 );
				}
			}
			else
			{
				for( quint8 n : Notes )
				{
					qint32			y1 = pTrackWidget->valueToView( qreal( n ) / 127.0 );

					Painter.drawPoint( i, y1 );
				}
			}
		}
	}
}

QList<fugio::KeyFramesControlsInterface *> MidiTimelineNode::editorControls()
{
	QList<fugio::KeyFramesControlsInterface *>	CtlLst;

//	MediaPlayerVideoPreview	*CTL = new MediaPlayerVideoPreview( this );

//	CTL->setObjectName( tr( "Video" ) );

//	CtlLst.append( CTL );

	return( CtlLst );
}

void MidiTimelineNode::aboutToPlay()
{
//	setInstanceSampleOffset( 0 );

//	mTimeOffset = mTimeLast;
}

void MidiTimelineNode::stateChanged( fugio::ContextInterface::TimeState pState )
{
	if( pState == fugio::ContextInterface::Stopped )
	{
//		setInstanceSampleOffset( 0 );

//		mTimeOffset   = mTimeLast;
	}
	else if( pState == fugio::ContextInterface::Playing )
	{
//		setInstanceSampleOffset( 0 );

//		mTimeOffset   = mTimeLast;
	}
}

void MidiTimelineNode::playheadMove( qreal pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

//	mTimeOffset   = pTimeStamp;
//	mTimeLast     = pTimeStamp;

//	setInstanceSampleOffset( 0 );

//	if( mSegment == 0 )
//	{
//		return;
//	}

//	updateVideo( pTimeStamp );
}

bool MidiTimelineNode::playheadPlay( qreal pTimePrev, qreal pTimeCurr )
{
	Q_UNUSED( pTimePrev )

//	mTimeLast = pTimeCurr;

	if( !mTimelineControl->isPlaying() )
	{
		return( true );
	}

	mValOutputMidi->clearData();

	if( mTimelineControl->isMute() )
	{
		return( true );
	}

	qint64	TP = mKF->time( pTimePrev ) * 1000.0;
	qint64	TC = mKF->time( pTimeCurr ) * 1000.0;

	for( const fugio::MidiEvent &SE : mEvents )
	{
		if( SE.timestamp > TC )
		{
			break;
		}

		if( !TP || SE.timestamp > TP )
		{
			mValOutputMidi->addMessage( SE.message );
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

	return( true );
}

void MidiTimelineNode::setTimeOffset( qreal pTimeOffset )
{
	mTimelineControl->setTimeOffset( pTimeOffset );

//	mTimeOffset   = pTimeOffset;
	//	mSampleOffset = -1;
}

qreal MidiTimelineNode::latency() const
{
	return( mTimelineControl->latency() );
}

QList<fugio::NodeControlInterface::AvailablePinEntry> MidiTimelineNode::availableOutputPins() const
{
	QList<fugio::NodeControlInterface::AvailablePinEntry>		PinLst = NodeControlBase::availableOutputPins();

	PinLst.append( mTimelineControl->availableOutputPins() );

	std::sort( PinLst.begin(), PinLst.end() );

	return( PinLst );
}

QStringList MidiTimelineNode::availableInputPins() const
{
	QStringList		PinLst = NodeControlBase::availableInputPins();

	PinLst.append( mTimelineControl->availableInputPins() );

	PinLst.sort();

	return( PinLst );
}
