#ifndef INTERFACE_KEYFRAMES_BACKGROUND_H
#define INTERFACE_KEYFRAMES_BACKGROUND_H

#include <QObject>

#include <fugio/global.h>

class QPainter;

FUGIO_NAMESPACE_BEGIN

class KeyFramesWidgetInterface;

class KeyFramesBackgroundInterface
{
public:
	virtual ~KeyFramesBackgroundInterface( void ) {}

	virtual void drawBackground( const KeyFramesWidgetInterface *pTrackWidget, QPainter &pPainter, const QRect &pUpdateRect ) const = 0;
};

FUGIO_NAMESPACE_END

Q_DECLARE_INTERFACE( fugio::KeyFramesBackgroundInterface, "com.bigfug.fugio.keyframes.background/1.0" )

#endif // INTERFACE_KEYFRAMES_BACKGROUND_H
