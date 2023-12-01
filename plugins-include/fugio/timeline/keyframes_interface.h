#ifndef INTERFACEKEYFRAMES_H
#define INTERFACEKEYFRAMES_H

#include <QObject>
#include <QList>

class QSettings;

#include <fugio/global.h>

class KeyFrameData;

FUGIO_NAMESPACE_BEGIN

class KeyFramesProviderInterface;
class TimelineTrackInterface;
class KeyFramesSignals;

class KeyFramesInterface
{
public:
	virtual ~KeyFramesInterface( void ) {}

	virtual KeyFramesSignals *qobject( void ) = 0;

	virtual bool isKeyFrame( qreal pTimeStamp ) const = 0;
	virtual bool hasKeyFrame( qreal pTimeStart, qreal pTimeEnd ) const = 0;

	virtual qreal keyframeNext( qreal pTimeStamp ) const = 0;
	virtual qreal keyframePrev( qreal pTimeStamp ) const = 0;

	virtual void keyframeSet( qreal pTimeStamp ) = 0;
	virtual void keyframeClear( qreal pTimeStamp ) = 0;

	virtual void keyframeInsert( KeyFrameData *pKeyFrameData ) = 0;
	virtual void keyframeRemove( KeyFrameData *pKeyFrameData ) = 0;

	virtual KeyFrameData *keyframeDataValue( qreal pTimeStamp ) = 0;
	virtual const KeyFrameData *keyframeDataValue( qreal pTimeStamp ) const = 0;

	virtual void keyframeDelete( qreal pTimeStart, qreal pTimeEnd ) = 0;

	virtual QList<qreal> keyframeTimes( void ) const = 0;
	virtual QList<qreal> keyframeTimes( qreal pTimeStart, qreal pTimeEnd ) const = 0;

	virtual KeyFramesProviderInterface &provider( void ) = 0;
	virtual const KeyFramesProviderInterface &provider( void ) const = 0;

	virtual const QList<KeyFrameData *> &keyframeData( void ) const = 0;

	virtual int keyframeCount( void ) const = 0;
	virtual qreal duration( void ) const = 0;

	virtual KeyFrameData *keyframeFirst( void ) = 0;
	virtual const KeyFrameData *keyframeFirst( void ) const = 0;

	virtual KeyFrameData *keyframeLast( void ) = 0;
	virtual const KeyFrameData *keyframeLast( void ) const = 0;

	virtual KeyFrameData *keyframeAt( int pIndex ) = 0;
	virtual const KeyFrameData *keyframeAt( int pIndex ) const = 0;

	virtual void indexes( qreal pTimeStamp, qreal &i1, qreal &i2 ) const = 0;

	virtual int keyframeIndex( KeyFrameData *pValue ) const = 0;

	virtual void keyframeUpdate( void ) = 0;

	virtual TimelineTrackInterface *trackdata( void ) = 0;

	virtual const TimelineTrackInterface *trackdata( void ) const = 0;

	virtual void cfgSave( QSettings &pDataStream ) const = 0;
	virtual void cfgLoad( QSettings &pDataStream ) = 0;

	virtual bool valuesSupported( void ) const = 0;
	virtual qreal valueNext( qreal pValCur ) const = 0;
	virtual qreal valuePrev( qreal pValCur ) const = 0;
	virtual void valueSet( qreal pValue ) = 0;
	virtual void valueClear( qreal pValue ) = 0;
	virtual qreal valueAt( qreal pTimeStamp ) const = 0;
	virtual qreal valueToView( qreal pValue ) const = 0;
	virtual QList<qreal> values( void ) const = 0;
	virtual qreal keyframeToValue( qreal pTimeStamp, qreal pValue ) = 0;
};

FUGIO_NAMESPACE_END

Q_DECLARE_INTERFACE( fugio::KeyFramesInterface, "com.bigfug.fugio.keyframes/1.0" )

#endif // INTERFACEKEYFRAMES_H
