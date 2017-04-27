#ifndef INTERFACE_KEYFRAMES_TIME_H
#define INTERFACE_KEYFRAMES_TIME_H

#include <QObject>

#include <fugio/global.h>

class QSettings;

#define KID_TIME		(QUuid("{C926B322-FE3D-4469-ACC2-AB935CB5BB42}"))

class KeyFrames;
class KeyFrameDataTime;

FUGIO_NAMESPACE_BEGIN

class TimelineControlInterface;
class InterfaceNodeControl;
class KeyFramesInterface;

class KeyFramesTimeInterface
{
public:
	virtual ~KeyFramesTimeInterface( void ) {}

	virtual QObject *qobject( void ) = 0;

	virtual KeyFramesInterface *keyframes( void ) = 0;
	virtual KeyFramesInterface *keyframes( void ) const = 0;

	virtual KeyFrameDataTime *keyframeTimeValue( qreal pTimeStamp ) = 0;
	virtual const KeyFrameDataTime *keyframeTimeValue( qreal pTimeStamp ) const = 0;

	virtual KeyFrameDataTime *data( qreal pTimeStamp ) = 0;
	virtual const KeyFrameDataTime *data( qreal pTimeStamp ) const = 0;

	virtual void setTime( qreal pTimeStamp, qreal pTime ) = 0;

	virtual qreal time( qreal pTimeStamp ) const = 0;

	virtual void setTimeMax( qreal pMaxTim ) = 0;

	virtual qreal duration( void ) const = 0;

	virtual void cfgLoad( QSettings &pSettings ) = 0;
	virtual void cfgSave( QSettings &pSettings ) const = 0;
};

FUGIO_NAMESPACE_END

Q_DECLARE_INTERFACE( fugio::KeyFramesTimeInterface, "com.bigfug.fugio.keyframes.time/1.0" )

#endif // INTERFACE_KEYFRAMES_TIME_H
