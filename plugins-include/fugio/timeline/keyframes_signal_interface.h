#ifndef INTERFACE_KEYFRAMES_SIGNAL_H
#define INTERFACE_KEYFRAMES_SIGNAL_H

#include <QObject>

#include <fugio/global.h>

class QSettings;

#define KID_SIGNAL		(QUuid("{9A0A5A94-E13A-4d0a-88A2-0A6FE33A0D34}"))

class KeyFrames;
class KeyFrameDataSignal;

FUGIO_NAMESPACE_BEGIN

class TimelineControlInterface;
class InterfaceNodeControl;
class KeyFramesInterface;

class KeyFramesSignalInterface
{
public:
	virtual ~KeyFramesSignalInterface( void ) {}

	virtual QObject *qobject( void ) = 0;

	virtual KeyFramesInterface *keyframes( void ) = 0;
	virtual KeyFramesInterface *keyframes( void ) const = 0;

	virtual KeyFrameDataSignal *data( qreal pTimeStamp ) = 0;
	virtual const KeyFrameDataSignal *data( qreal pTimeStamp ) const = 0;

	virtual qreal value( qreal pTimeStamp ) const = 0;

	virtual void cfgLoad( QSettings &pSettings ) = 0;
	virtual void cfgSave( QSettings &pSettings ) const = 0;
};

FUGIO_NAMESPACE_END

Q_DECLARE_INTERFACE( fugio::KeyFramesSignalInterface, "com.bigfug.fugio.keyframes.signal/1.0" )

#endif // INTERFACE_KEYFRAMES_SIGNAL_H
