#ifndef INTERFACE_TIMELINE_TRACK_H
#define INTERFACE_TIMELINE_TRACK_H

#include <QObject>

#include <fugio/global.h>

class QWidget;
class QTabWidget;

FUGIO_NAMESPACE_BEGIN

class NodeControlInterface;
class KeyFramesWidgetInterface;
class TimelineControlInterface;
class KeyFramesInterface;
class KeyFramesControlsInterface;

class TimelineTrackInterface
{
public:
	virtual ~TimelineTrackInterface( void ) {}

	virtual QObject *qobject( void ) = 0;

	virtual NodeControlInterface *nodeControl( void ) = 0;

	virtual qreal position( void ) const = 0;

	virtual qreal duration( void ) const = 0;

	virtual KeyFramesInterface *keyframes( void ) = 0;

	virtual KeyFramesWidgetInterface *newTimelineGui( void ) = 0;

	virtual TimelineControlInterface *control( void ) = 0;

	virtual QList<KeyFramesControlsInterface *> editorControls( void ) = 0;

	virtual bool canRecord( void ) const = 0;
};

FUGIO_NAMESPACE_END

Q_DECLARE_INTERFACE( fugio::TimelineTrackInterface, "com.bigfug.fugio.timeline.track/1.0" )

#endif // INTERFACE_TIMELINE_TRACK_H
