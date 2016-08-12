#ifndef SIZE_INTERFACE_H
#define SIZE_INTERFACE_H

#include <QSizeF>
#include <QVector3D>

#include <fugio/global.h>

FUGIO_NAMESPACE_BEGIN

class SizeInterface
{
public:
	virtual ~SizeInterface( void ) {}

	virtual int sizeDimensions( void ) const = 0;

	virtual float size( int pDimension ) const = 0;

	virtual float sizeWidth( void ) const = 0;
	virtual float sizeHeight( void ) const = 0;
	virtual float sizeDepth( void ) const = 0;

	virtual QSizeF toSizeF( void ) const = 0;
	virtual QVector3D toVector3D( void ) const = 0;
};

FUGIO_NAMESPACE_END

Q_DECLARE_INTERFACE( fugio::SizeInterface, "com.bigfug.fugio.size/1.0" )

#endif // SIZE_INTERFACE_H
