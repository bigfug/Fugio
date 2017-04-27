#ifndef INTERFACE_KEYFRAMES_MARKER_H
#define INTERFACE_KEYFRAMES_MARKER_H

#include <QObject>

#include <fugio/global.h>

#define KID_MARKER		(QUuid("{C6360B67-117C-4E6B-858B-8F2B9568CF5F}"))

class QSettings;

class KeyFrameDataMarker;

FUGIO_NAMESPACE_BEGIN

class KeyFramesInterface;

class KeyFramesMarkerInterface
{
public:
	virtual ~KeyFramesMarkerInterface( void ) {}

	virtual KeyFramesInterface *keyframes( void ) = 0;
	virtual KeyFramesInterface *keyframes( void ) const = 0;

	virtual KeyFrameDataMarker *data( qreal pTimeStamp ) = 0;
	virtual const KeyFrameDataMarker *data( qreal pTimeStamp ) const = 0;

	virtual void cfgLoad( QSettings &pSettings ) = 0;
	virtual void cfgSave( QSettings &pSettings ) const = 0;
};

FUGIO_NAMESPACE_END

Q_DECLARE_INTERFACE( fugio::KeyFramesMarkerInterface, "com.bigfug.fugio.keyframes.marker/1.0" )

#endif // INTERFACE_KEYFRAMES_MARKER_H
