#ifndef INTERFACE_TIMELINE_CONTROL_H
#define INTERFACE_TIMELINE_CONTROL_H

#include <QObject>

#include <fugio/global.h>
#include <fugio/node_control_interface.h>

#include "keyframes_marker_interface.h"
#include "keyframes_widget_interface.h"

FUGIO_NAMESPACE_BEGIN

class TimelineControlInterface
{
public:
	virtual ~TimelineControlInterface( void ) {}

	virtual QObject *qobject( void ) = 0;

	virtual bool initialise( void ) = 0;

	virtual bool deinitialise( void ) = 0;

	virtual void playStart( qreal pTimeStamp ) = 0;

	virtual qreal timeoffset( void ) const = 0;

	virtual qreal latency( void ) const = 0;

	virtual void setTimeOffset( qreal pTimeOffset ) = 0;

	virtual qreal position( void ) const = 0;

	virtual KeyFramesMarkerInterface &markers( void ) = 0;

	virtual const KeyFramesMarkerInterface &markers( void ) const = 0;

	virtual void update( void ) = 0;

	virtual void update( qreal pKeyFrameTimeStamp ) = 0;

	virtual void update( qreal pTimeStart, qreal pTimeEnd ) = 0;

	virtual bool canRecord( void ) const = 0;

	virtual void setSolo( bool pSolo ) = 0;
	virtual void setMute( bool pMute ) = 0;
	virtual void setTrigger( bool pTrigger ) = 0;
	virtual void setRecord( bool pRecord ) = 0;
	virtual void setPlaying( bool pPlay ) = 0;

	virtual bool isSolo( void ) const = 0;
	virtual bool isMute( void ) const = 0;
	virtual bool isTrigger( void ) const = 0;
	virtual bool isRecording( void ) const = 0;
	virtual bool isPlaying( void ) const = 0;

	virtual void inputsUpdated( qint64 pTimeStamp ) = 0;

	virtual void cfgSave( QSettings &pDataStream ) const = 0;
	virtual void cfgLoad( QSettings &pDataStream ) = 0;

	virtual QStringList availableInputPins( void ) const = 0;
	virtual QList<fugio::NodeControlInterface::AvailablePinEntry> availableOutputPins( void ) const = 0;

	virtual void setSelectionStart( qreal pTimeStamp ) = 0;
	virtual void setSelectionEnd( qreal pTimeStamp ) = 0;

	virtual qreal selectionRange( void ) const = 0;
	virtual qreal selectionStart( void ) const = 0;
	virtual qreal selectionEnd( void ) const = 0;

    enum SnapType
    {
        SNAP_NONE   = 0,
        SNAP_KEY    = 1 << 0,
        SNAP_MARKER = 1 << 1,
        SNAP_VALUE  = 1 << 2
    };

    virtual void setSnapType( SnapType pSnapType, bool pEnabled ) = 0;
    virtual void setSnapType( SnapType pSnapType ) = 0;
    virtual void clearSnapType( SnapType pSnapType ) = 0;

    virtual bool snapKey( void ) const = 0;
    virtual bool snapMarker( void ) const = 0;
    virtual bool snapValue( void ) const = 0;

	virtual void snap( KeyFramesWidgetInterface *pTrackWidget, qreal &x ) const = 0;
	virtual void snap( KeyFramesWidgetInterface *pTrackWidget, qreal &x, qreal &y ) const = 0;

	virtual void selectedKeyFrameTimes( KeyFramesInterface *pKeyFrames, QList<qreal> &pKeyFrameList, const qreal pCurrentTime, qreal &pUpdateStartTime, qreal &pUpdateEndTime ) const = 0;
};

FUGIO_NAMESPACE_END

Q_DECLARE_INTERFACE( fugio::TimelineControlInterface, "com.bigfug.fugio.timeline.control/1.0" )

#endif // INTERFACE_TIMELINE_CONTROL_H
