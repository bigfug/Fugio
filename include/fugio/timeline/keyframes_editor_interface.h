#ifndef INTERFACE_KEYFRAMES_EDITOR_H
#define INTERFACE_KEYFRAMES_EDITOR_H

#include <QObject>

#include <fugio/global.h>

class QPainter;
class QMouseEvent;

class KeyFramesWidget;

FUGIO_NAMESPACE_BEGIN

class KeyFramesEditorInterface
{
public:
	virtual ~KeyFramesEditorInterface( void ) {}

	virtual QWidget *controls( void ) = 0;

	virtual void updateBackground( const KeyFramesWidget *pTrackWidget, QPainter &pPainter, const QRect &pUpdateRect ) const = 0;

	virtual void drawControls( const KeyFramesWidget *pTrackWidget, QPainter &pPainter, const QRect &pUpdateRect ) const = 0;

	virtual bool mousePressEvent( KeyFramesWidget *pTrackWidget, QMouseEvent *pEvent ) = 0;

	virtual bool mouseMoveEvent( KeyFramesWidget *pTrackWidget, QMouseEvent *pEvent ) = 0;

	virtual bool mouseReleaseEvent( KeyFramesWidget *pTrackWidget, QMouseEvent *pEvent ) = 0;

	virtual bool mouseDoubleClickEvent( KeyFramesWidget *pTrackWidget, QMouseEvent *pEvent ) = 0;
};

FUGIO_NAMESPACE_END

Q_DECLARE_INTERFACE( fugio::KeyFramesEditorInterface, "com.bigfug.fugio.keyframeseditor/1.0" )

#endif // INTERFACE_KEYFRAMES_EDITOR_H
