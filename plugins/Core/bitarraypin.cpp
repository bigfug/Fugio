#include "bitarraypin.h"

BitArrayPin::BitArrayPin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin )
{
}

int BitArrayPin::sizeDimensions() const
{
	return( 1 );
}

float BitArrayPin::size(int pDimension) const
{
	return( pDimension == 0 ? mValue.size() : 0 );
}

float BitArrayPin::sizeWidth() const
{
	return( mValue.size() );
}

float BitArrayPin::sizeHeight() const
{
	return( 0 );
}

float BitArrayPin::sizeDepth() const
{
	return( 0 );
}

QSizeF BitArrayPin::toSizeF() const
{
	return( QSizeF( mValue.size(), 0 ) );
}

QVector3D BitArrayPin::toVector3D() const
{
	return( QVector3D( mValue.size(), 0, 0 ) );
}
