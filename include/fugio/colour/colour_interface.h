#ifndef INTERFACE_COLOUR_H
#define INTERFACE_COLOUR_H

#include <QUuid>
#include <QColor>

#include <fugio/global.h>

#define PID_COLOUR		(QUuid("{c3bf73b1-6a0a-4ce0-9842-420064e5c383}"))

FUGIO_NAMESPACE_BEGIN

class ColourInterface
{
public:
	virtual ~ColourInterface( void ) {}

	virtual void setColour( const QColor &pValue ) = 0;

	virtual void setColour( int pIndex, const QColor &pValue ) = 0;

	virtual QColor colour( int pIndex = 0 ) const = 0;
};

FUGIO_NAMESPACE_END

Q_DECLARE_INTERFACE( fugio::ColourInterface, "com.bigfug.fugio.colour/1.0" )

#endif // INTERFACE_COLOUR_H
