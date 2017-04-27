#ifndef INTERFACE_TIMELINE_WIDGET_H
#define INTERFACE_TIMELINE_WIDGET_H

#include <QObject>

#include <fugio/global.h>

FUGIO_NAMESPACE_BEGIN

class TimelineTrackInterface;

class TimelineWidgetInterface
{
public:
	virtual ~TimelineWidgetInterface( void ) {}

	virtual void addTrack( TimelineTrackInterface *pTimeline ) = 0;

	virtual void remTrack( TimelineTrackInterface *pTimeline ) = 0;
};

FUGIO_NAMESPACE_END

Q_DECLARE_INTERFACE( fugio::TimelineWidgetInterface, "com.bigfug.fugio.timeline.widget/1.0" )

#endif // INTERFACE_TIMELINE_WIDGET_H
