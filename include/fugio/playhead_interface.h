#ifndef INTERFACE_PLAYHEAD_H
#define INTERFACE_PLAYHEAD_H

#include <QtPlugin>

#include "global.h"

FUGIO_NAMESPACE_BEGIN

class PlayheadInterface
{
public:
	virtual ~PlayheadInterface( void ) {}

	virtual void playStart( qreal pTimeStamp ) = 0;

	virtual void playheadMove( qreal pTimeStamp ) = 0;

	virtual bool playheadPlay( qreal pTimePrev, qreal pTimeCurr ) = 0;

	virtual qreal latency( void ) const = 0;

	virtual void setTimeOffset( qreal pTimeOffset ) = 0;
};

FUGIO_NAMESPACE_END

Q_DECLARE_INTERFACE( fugio::PlayheadInterface, "com.bigfug.fugio.playhead/1.0" )

#endif // INTERFACE_PLAYHEAD_H
