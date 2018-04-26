#include "bytearraypin.h"

#include <fugio/core/uuid.h>

ByteArrayPin::ByteArrayPin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin ), VariantHelper( QMetaType::QByteArray, PID_BYTEARRAY )
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
	return( pDimension == 0 ? mValues.first().size() : 0 );
}

float ByteArrayPin::sizeWidth() const
{
	return( mValues.first().size() );
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
	return( QSizeF( mValues.first().size(), 0 ) );
}

QVector3D ByteArrayPin::toVector3D() const
{
	return( QVector3D( mValues.first().size(), 0, 0 ) );
}
