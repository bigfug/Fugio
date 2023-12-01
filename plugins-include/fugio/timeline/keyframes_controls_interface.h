#ifndef INTERFACE_KEYFRAMES_CONTROLS_H
#define INTERFACE_KEYFRAMES_CONTROLS_H

#include <QObject>

#include <fugio/global.h>

class QPainter;

FUGIO_NAMESPACE_BEGIN

class KeyFramesControlsInterface
{
public:
	virtual ~KeyFramesControlsInterface( void ) {}

	virtual QWidget *widget( void ) = 0;

	virtual void playheadPlay( qreal pTimeLast, qreal pTimeCurr ) = 0;
};

FUGIO_NAMESPACE_END

Q_DECLARE_INTERFACE( fugio::KeyFramesControlsInterface, "com.bigfug.fugio.keyframecontrols/1.0" )

#endif // INTERFACE_KEYFRAMES_CONTROLS_H
