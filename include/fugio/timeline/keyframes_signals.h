#ifndef KEYFRAMES_SIGNALS_H
#define KEYFRAMES_SIGNALS_H

#include <QObject>

#include <fugio/global.h>

FUGIO_NAMESPACE_BEGIN

class KeyFramesSignals : public QObject
{
	Q_OBJECT

signals:
	void keyframeInserted( qreal pTimeStamp );
	void keyframeDeleted( qreal pTimeStamp );
	void keyframeUpdated( qreal pTimeStamp );

protected:
	KeyFramesSignals( QObject *pParent = 0 );

	virtual ~KeyFramesSignals( void );
};

FUGIO_NAMESPACE_END

#endif // KEYFRAMES_SIGNALS_H
