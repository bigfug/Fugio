#ifndef INTERFACE_KEYFRAMES_WIDGET_H
#define INTERFACE_KEYFRAMES_WIDGET_H

#include <QObject>

#include <fugio/global.h>

class QWidget;

FUGIO_NAMESPACE_BEGIN

class KeyFramesEditorInterface;
class KeyFramesBackgroundInterface;

class KeyFramesWidgetInterface
{
public:
	virtual ~KeyFramesWidgetInterface( void ) {}

	virtual QWidget *widget( void ) = 0;

	virtual KeyFramesEditorInterface *editor( void ) = 0;

	virtual void setBackground( KeyFramesBackgroundInterface *pBackground ) = 0;

	virtual void updateBackground( qreal pTimeStart, qreal pTimeEnd ) = 0;

	virtual void updateBackground( qreal pTimeStamp ) = 0;

	virtual bool hasControls( void ) const = 0;

	virtual QRect trackSize( void ) const = 0;

	virtual const QImage &viewImage( void ) const = 0;

	virtual qreal viewStart( void ) const = 0;
	virtual qreal viewEnd( void ) const = 0;
	virtual qreal viewRange( void ) const = 0;

	virtual qint32 timestampToView( qreal pTimeStamp ) const = 0;

	virtual qreal viewToTimestamp( qint32 pView ) const = 0;

	virtual qreal viewToTimestampRaw( qint32 pView ) const = 0;

	virtual qreal viewToValue( qint32 pView ) const = 0;

	virtual qint32 valueToView( qreal pValue ) const = 0;

	virtual void setViewPosition( qreal pViewStart, qreal pViewEnd ) = 0;

	virtual void setViewRange( qreal pViewRange ) = 0;
};

FUGIO_NAMESPACE_END

Q_DECLARE_INTERFACE( fugio::KeyFramesWidgetInterface, "com.bigfug.fugio.keyframeswidget/1.0" )

#endif // INTERFACE_KEYFRAMES_WIDGET_H
