#include "bytearraypin.h"

ByteArrayPin::ByteArrayPin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin )
{
}

ByteArrayPin::~ByteArrayPin()
{

}


int ByteArrayPin::sizeDimensions() const
{
	return( 1 );
}

float ByteArrayPin::size(int pDimension) const
{
	return( pDimension == 0 ? mValue.size() : 0 );
}

float ByteArrayPin::sizeWidth() const
{
	return( mValue.size() );
}

float ByteArrayPin::sizeHeight() const
{
	return( 0 );
}

float ByteArrayPin::sizeDepth() const
{
	return( 0 );
}

QSizeF ByteArrayPin::toSizeF() const
{
	return( QSizeF( mValue.size(), 0 ) );
}

QVector3D ByteArrayPin::toVector3D() const
{
	return( QVector3D( mValue.size(), 0, 0 ) );
}
