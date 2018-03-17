#include "arraypin.h"

#include <fugio/core/uuid.h>

#include <QRect>
#include <QRectF>
#include <QLineF>
#include <QColor>

#include "coreplugin.h"

ArrayPin::ArrayPin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin ), mData( nullptr ), mType( QMetaType::UnknownType ), mStride( 0 ),
	  mCount( 0 ), mSize( 0 ), mReserve( 0 )
{

}

QString ArrayPin::toString() const
{
	return( QString( "<p>Type: %1</p><p>Count: %2</p><p>Size: %3</p><p>Total Size: %4</p>" ).arg( QString( QMetaType::typeName( type() ) ) ).arg( count() ).arg( size() ).arg( count() * stride() ) );
}

void *ArrayPin::array()
{
	if( mData )
	{
		return( mData );
	}

	mArray.resize( ( ( mStride * qMax( mCount, mReserve ) ) / sizeof( qlonglong ) ) + 1 );

	return( mArray.data() );

}

const void *ArrayPin::array() const
{
	if( mData )
	{
		return( mData );
	}

	int		ArrayCount = ( ( mStride * qMax( mCount, mReserve ) ) / sizeof( qlonglong ) ) + 1;

	if( mArray.size() == ArrayCount )
	{
		return( mArray.constData() );
	}

	return( nullptr );
}

void ArrayPin::setVariant(int pIndex, int pOffset, const QVariant &pValue)
{
	quint8	*A = (quint8 *)( mData ? mData : ( !mArray.isEmpty() ? mArray.data() : nullptr ) );

	if( !A )
	{
		return;
	}

	if( pIndex < 0 || pIndex >= mCount )
	{
		return;
	}

	if( pOffset < 0 || pOffset >= mSize )
	{
		return;
	}

	int				L = QMetaType::sizeOf( mType );

	A += ( mStride * pIndex ) + ( L * pOffset );

	QMetaType::construct( mType, A, pValue.constData() );
}

QVariant ArrayPin::variant(int pIndex, int pOffset) const
{
	const quint8	*A = (const quint8 *)( mData ? mData : ( !mArray.isEmpty() ? mArray.data() : nullptr ) );

	if( !A )
	{
		return( QVariant() );
	}

	if( pIndex < 0 || pIndex >= mCount )
	{
		return( QVariant() );
	}

	if( pOffset < 0 || pOffset >= mSize )
	{
		return( QVariant() );
	}

	int				L = QMetaType::sizeOf( mType );

	A += ( mStride * pIndex ) + ( L * pOffset );

	QVariant		 V( mType, (const void *)A );

	return( V );
}
