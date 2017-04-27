#ifndef INTERFACE_KEYFRAMES_GRID_H
#define INTERFACE_KEYFRAMES_GRID_H

#include <QObject>

#include <fugio/global.h>

class QSettings;

#define KID_GRID		(QUuid("{BD06735D-3173-4F3B-B247-69AFBF0DB26E}"))

class KeyFrameDataGrid;

FUGIO_NAMESPACE_BEGIN

class KeyFramesInterface;

class KeyFramesGridInterface
{
public:
	virtual ~KeyFramesGridInterface( void ) {}

	virtual QObject *qobject( void ) = 0;

	virtual KeyFramesInterface *keyframes( void ) = 0;
	virtual const KeyFramesInterface *keyframes( void ) const = 0;

	virtual QString name( void ) const = 0;

	virtual qreal beatvalue( qreal pTimeStamp ) const = 0;
	virtual qreal value( qreal pTimeStamp ) const = 0;

//	virtual QColor colour( qreal pTimeStamp ) const = 0;

//	virtual void setColour( qreal pTimeStamp, const QColor &pColor ) = 0;

//	virtual KeyFrameDataColour *data( qreal pTimeStamp ) = 0;
//	virtual const KeyFrameDataColour *data( qreal pTimeStamp ) const = 0;

	virtual void cfgLoad( QSettings &pSettings ) = 0;
	virtual void cfgSave( QSettings &pSettings ) const = 0;
};

FUGIO_NAMESPACE_END

Q_DECLARE_INTERFACE( fugio::KeyFramesGridInterface, "com.bigfug.fugio.keyframes.grid/1.0" )

#endif // INTERFACE_KEYFRAMES_GRID_H
