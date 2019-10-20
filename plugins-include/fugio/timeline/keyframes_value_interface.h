#ifndef INTERFACE_KEYFRAMES_VALUE_H
#define INTERFACE_KEYFRAMES_VALUE_H

#include <QObject>

#include <fugio/global.h>

class QSettings;

#define KID_NUMBER		(QUuid("{DAA7A40C-951A-4b31-AFCC-10E6726DE0A2}"))

class KeyFrameDataValue;

FUGIO_NAMESPACE_BEGIN

class KeyFramesInterface;

typedef enum KeyInterpolate
{
	DEFAULT,
	LINEAR,
	COSINE,
	LINEAR_FALL,
	LINEAR_RISE,
	LOG_FALL,
	HOLD,
	FORWARD,
	BACKWARD
} KeyInterpolate;

class KeyFramesValueInterface
{
public:
	virtual ~KeyFramesValueInterface( void ) {}

	virtual QObject *qobject( void ) = 0;

	virtual KeyFramesInterface *keyframes( void ) = 0;
	virtual KeyFramesInterface *keyframes( void ) const = 0;

	virtual void setValue( qreal pTimeStamp, qreal pValue ) = 0;

	virtual qreal value( qreal pTimeStamp ) const = 0;

	virtual qreal rawvalue( qreal pTimeStamp ) const = 0;

	virtual qreal keyValue( qreal pTimeStamp ) const = 0;

	virtual KeyFrameDataValue *data( qreal pTimeStamp ) = 0;
	virtual const KeyFrameDataValue *data( qreal pTimeStamp ) const = 0;

	virtual void cfgLoad( QSettings &pSettings ) = 0;
	virtual void cfgSave( QSettings &pSettings ) const = 0;

	virtual void setDefaultInterpolation( KeyInterpolate pValue ) = 0;

	virtual int steps( void ) const = 0;

	virtual void setSteps( int pSteps ) = 0;

	virtual void setInterpolateIn( qreal pTimeStamp, KeyInterpolate pInterpolate ) = 0;
	virtual void setInterpolateOut( qreal pTimeStamp, KeyInterpolate pInterpolate ) = 0;
	virtual void setInterpolate( qreal pTimeStamp, KeyInterpolate pInterpolateIn, KeyInterpolate pInterpolateOut ) = 0;
};

FUGIO_NAMESPACE_END

Q_DECLARE_INTERFACE( fugio::KeyFramesValueInterface, "com.bigfug.fugio.keyframes.value/1.0" )

#endif // INTERFACE_KEYFRAMES_VALUE_H
