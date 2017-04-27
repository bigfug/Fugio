#ifndef INTERFACE_TIMELINE_H
#define INTERFACE_TIMELINE_H

#include <QObject>

#include <fugio/global.h>

#include <fugio/context_interface.h>

FUGIO_NAMESPACE_BEGIN

class TimelineControlInterface;
class InterfaceNodeControl;
class KeyFramesProviderInterface;
class KeyFramesWidgetInterface;
class TimelineTrackInterface;
class KeyFramesEditorInterface;
class KeyFramesControlsInterface;

class TimelineInterface
{
public:
	virtual ~TimelineInterface( void ) {}

	virtual void registerKeyFrameProviders( const ClassEntry pClasses[] ) = 0;
	virtual void unregisterKeyFrameProviders( const ClassEntry pClasses[] ) = 0;

	virtual TimelineControlInterface *control( TimelineTrackInterface *pNodeControl ) = 0;

	virtual KeyFramesProviderInterface *keyframes( TimelineTrackInterface *pTrackData, const QUuid &pUuid ) = 0;

	virtual KeyFramesWidgetInterface *keyframesWidget( TimelineTrackInterface *pTimeline, TimelineControlInterface *pControl, KeyFramesEditorInterface *pTrackData ) const = 0;
};

FUGIO_NAMESPACE_END

Q_DECLARE_INTERFACE( fugio::TimelineInterface, "com.bigfug.fugio.timeline/1.0" )

#endif // INTERFACE_TIMELINE_H
