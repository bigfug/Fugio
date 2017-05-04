#ifndef MIDITIMELINENODE_H
#define MIDITIMELINENODE_H

#include <QImage>

#include <fugio/nodecontrolbase.h>

#include <fugio/node_interface.h>
#include <fugio/node_control_interface.h>
#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>

#include <fugio/playhead_interface.h>

#include <fugio/nodecontrolbase.h>

#include <fugio/timeline/timeline_track_interface.h>

#include <fugio/timeline/keyframes_interface.h>
#include <fugio/timeline/keyframes_time_interface.h>
#include <fugio/timeline/keyframes_background_interface.h>

#include <fugio/midi/midi_interface.h>

FUGIO_NAMESPACE_BEGIN
class TimelineControlInterface;
FUGIO_NAMESPACE_END

using namespace fugio;

class MidiTimelineNode : public fugio::NodeControlBase, public fugio::PlayheadInterface, public fugio::TimelineTrackInterface, public fugio::KeyFramesBackgroundInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::PlayheadInterface fugio::TimelineTrackInterface fugio::KeyFramesBackgroundInterface )

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "MIDI Timeline" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Midi_Timeline" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit MidiTimelineNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~MidiTimelineNode( void );

	void setEvents( const QList<fugio::MidiEvent> &pEventList );

	//-------------------------------------------------------------------------
	// fugio::NodeControlBase

	virtual bool initialise( void ) Q_DECL_OVERRIDE;

	virtual bool deinitialise( void ) Q_DECL_OVERRIDE;

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

	virtual void loadSettings( QSettings & ) Q_DECL_OVERRIDE;

	virtual void saveSettings( QSettings & ) const Q_DECL_OVERRIDE;

	virtual QStringList availableInputPins() const Q_DECL_OVERRIDE;
	virtual QList<AvailablePinEntry> availableOutputPins() const Q_DECL_OVERRIDE;

	//-------------------------------------------------------------------------
	// fugio::PlayheadInterface

	virtual void playStart( qreal pTimeStamp ) Q_DECL_OVERRIDE
	{
		playheadMove( pTimeStamp );
	}

	virtual void playheadMove( qreal pTimeStamp ) Q_DECL_OVERRIDE;

	virtual bool playheadPlay( qreal pTimePrev, qreal pTimeCurr ) Q_DECL_OVERRIDE;

	virtual void setTimeOffset( qreal pTimeOffset ) Q_DECL_OVERRIDE;

	virtual qreal latency( void ) const Q_DECL_OVERRIDE;

	//-------------------------------------------------------------------------
	// InterfaceTimelineTrack

	virtual QObject *qobject( void ) Q_DECL_OVERRIDE
	{
		return( this );
	}

	virtual qreal position( void ) const Q_DECL_OVERRIDE;

	virtual qreal duration( void ) const Q_DECL_OVERRIDE;

	virtual fugio::KeyFramesWidgetInterface *newTimelineGui( void ) Q_DECL_OVERRIDE;

	virtual fugio::TimelineControlInterface *control( void ) Q_DECL_OVERRIDE
	{
		return( mTimelineControl );
	}

	virtual fugio::NodeControlInterface *nodeControl( void ) Q_DECL_OVERRIDE
	{
		return( this );
	}

	virtual fugio::KeyFramesInterface *keyframes( void ) Q_DECL_OVERRIDE
	{
		return( mKF->keyframes() );
	}

	virtual QList<fugio::KeyFramesControlsInterface *> editorControls( void ) Q_DECL_OVERRIDE;

	virtual bool canRecord( void ) const Q_DECL_OVERRIDE
	{
		return( false );
	}

	//-------------------------------------------------------------------------
	// InterfaceTimelineTrack

	virtual void drawBackground( const fugio::KeyFramesWidgetInterface *pTrackWidget, const QRect &pUpdateRect, QImage &pBackImage ) const Q_DECL_OVERRIDE;

	//-------------------------------------------------------------------------

protected slots:
	void stateChanged( ContextInterface::TimeState pState );

	void aboutToPlay( void );

private:
	fugio::TimelineControlInterface					*mTimelineControl;

	fugio::KeyFramesTimeInterface					*mKF;

	QSharedPointer<fugio::PinInterface>				 mPinOutputMidi;
	fugio::MidiInterface							*mValOutputMidi;

	QList<fugio::MidiEvent>							 mEvents;
};

#endif // MIDITIMELINENODE_H
