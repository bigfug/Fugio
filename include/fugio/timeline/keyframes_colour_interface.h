#ifndef INTERFACE_KEYFRAMES_COLOUR_H
#define INTERFACE_KEYFRAMES_COLOUR_H

#include <QObject>
#include <QColor>

class QSettings;

#include <fugio/global.h>

#define KID_COLOUR		(QUuid("{BD06255D-3173-4F3B-B247-69AFBF0DB26E}"))

class KeyFrameDataColour;

FUGIO_NAMESPACE_BEGIN

class KeyFramesInterface;

typedef struct NamedColour
{
	QString		mName;
	QColor		mColour;

	NamedColour( QColor C = QColor(), QString S = QString() )
		: mName( S ), mColour( C )
	{

	}

} NamedColour;

class KeyFramesColourInterface
{
public:
	virtual ~KeyFramesColourInterface( void ) {}

	virtual KeyFramesInterface *keyframes( void ) = 0;
	virtual KeyFramesInterface *keyframes( void ) const = 0;

	virtual QColor colour( qreal pTimeStamp ) const = 0;

	virtual void setColour( qreal pTimeStamp, const QColor &pColor ) = 0;

	virtual KeyFrameDataColour *data( qreal pTimeStamp ) = 0;
	virtual const KeyFrameDataColour *data( qreal pTimeStamp ) const = 0;

	virtual void cfgLoad( QSettings &pSettings ) = 0;
	virtual void cfgSave( QSettings &pSettings ) const = 0;

	virtual QList<NamedColour> &palette( void ) = 0;
	virtual const QList<NamedColour> &palette( void ) const = 0;
};

FUGIO_NAMESPACE_END

Q_DECLARE_INTERFACE( fugio::KeyFramesColourInterface, "com.bigfug.fugio.keyframes.colour/1.0" )

#endif // INTERFACE_KEYFRAMES_COLOUR_H
