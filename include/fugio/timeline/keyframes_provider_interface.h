#ifndef INTERFACE_KEYFRAMES_PROVIDER_H
#define INTERFACE_KEYFRAMES_PROVIDER_H

#include <QObject>

#include <fugio/global.h>

class QSettings;

#define	SNAP_NONE		( 0 )
#define	SNAP_KEY_FRAMES	( 1 << 0 )
#define	SNAP_MARKERS	( 1 << 1 )
#define	SNAP_VALUES		( 1 << 2 )

class KeyFrameData;

FUGIO_NAMESPACE_BEGIN

class KeyFramesInterface;
class KeyFramesEditorInterface;

class KeyFramesProviderInterface
{
public:
	virtual ~KeyFramesProviderInterface( void ) {}

	virtual QObject *qobject( void ) = 0;

	virtual void initialise( void ) = 0;

	virtual bool multi( void ) const = 0;
	virtual bool ranged( void ) const = 0;

	virtual KeyFramesInterface *keyframes( void ) = 0;
	virtual const KeyFramesInterface *keyframes( void ) const = 0;

	virtual KeyFrameData *newValue( qreal pTimeStamp, const KeyFrameData *pValue = 0 ) const = 0;
	virtual KeyFrameData *newValue( qreal pTimeStamp, qreal pValue ) const = 0;

	virtual KeyFramesEditorInterface *newEditor( void ) = 0;

	virtual int snappingSupported( void ) const = 0;

	virtual bool valuesSupported( void ) const = 0;

	virtual qreal valueAt( qreal pTimeStamp ) const = 0;
	virtual qreal valueToView( qreal pValue ) const = 0;
	virtual qreal keyframeToValue( qreal pTimeStamp, qreal pValue ) = 0;
};

FUGIO_NAMESPACE_END

Q_DECLARE_INTERFACE( fugio::KeyFramesProviderInterface, "com.bigfug.fugio.keyframeprovider/1.0" )

#endif // INTERFACE_KEYFRAMES_PROVIDER_H
