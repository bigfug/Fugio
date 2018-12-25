#include "arraypin.h"

#include <fugio/core/uuid.h>

#include <QRect>
#include <QRectF>
#include <QLineF>
#include <QColor>

#include "coreplugin.h"

ArrayPin::ArrayPin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin ), mData( nullptr ), mArray( Q_NULLPTR ), mArraySize( 0 ),
	  mType( QMetaType::UnknownType ), mStride( 0 ),
	  mCount( 0 ), mSize( 0 ), mReserve( 0 )
{

}

ArrayPin::~ArrayPin()
{
	if( mArray )
	{
		qFreeAligned( mArray );

		mArray = Q_NULLPTR;

		mArraySize = 0;
	}
}

QString ArrayPin::toString() const
{
	return( QString( "<p>Type: %1</p><p>Count: %2</p><p>Size: %3</p><p>Total Size: %4</p>" ).arg( QString( QMetaType::typeName( type() ) ) ).arg( count() ).arg( elementCount() ).arg( byteCount() ) );
}

void ArrayPin::setStride( int pStride )
{
	mStride = pStride;

	updateArray();
}

void ArrayPin::setCount( int pCount )
{
	mCount = pCount;

	updateArray();
}

void ArrayPin::variantSetStride( int pStride )
{
	mStride = pStride;

	updateArray();
}

void *ArrayPin::array()
{
	if( mData )
	{
		return( mData );
	}

	updateArray();

	return( mArray );
}

const void *ArrayPin::array() const
{
	if( mData )
	{
		return( mData );
	}

	if( mArraySize >= variantArraySize() )
	{
		return( mArray );
	}

	return( Q_NULLPTR );
}

void ArrayPin::setVariantType( QMetaType::Type pType )
{
	mType = pType;

	switch( mType )
	{
		case QMetaType::QPointF:
			mTypeSize = sizeof( float ) * 2;
			break;

		default:
			mTypeSize = QMetaType::sizeOf( mType );
	}

	mStride = mTypeSize * variantElementCount();

	updateArray();
}

void ArrayPin::setVariantCount( int pCount )
{
	mCount = pCount;

	updateArray();
}

void ArrayPin::setVariantElementCount( int pElementCount )
{
	mSize = pElementCount;

	updateArray();
}

void ArrayPin::setVariant( int pIndex, int pOffset, const QVariant &pValue )
{
	quint8	*A = static_cast<quint8 *>( mData ? mData : mArray );

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

	int				O = ( mStride * pIndex ) + ( mTypeSize * pOffset );

	A += O;

	switch( mType )
	{
		case QMetaType::Float:
			reinterpret_cast<float *>( A )[ 0 ] = pValue.toFloat();
			break;

		case QMetaType::Int:
			reinterpret_cast<float *>( A )[ 0 ] = pValue.toInt();
			break;

		case QMetaType::QPointF:
			{
				QPointF	V;

				switch( pValue.type() )
				{
					case QVariant::PointF:
						V = pValue.toPointF();
						break;

					case QVariant::List:
						{
							QVariantList	L = pValue.toList();

							if( L.size() == 2 )
							{
								V.setX( L[ 0 ].toReal() );
								V.setY( L[ 1 ].toReal() );
							}
						}
						break;

					default:
						break;
				}

				float	*B = reinterpret_cast<float *>( A );

				B[ 0 ] = static_cast<float>( V.x() );
				B[ 1 ] = static_cast<float>( V.y() );
			}
			break;

		default:
			break;
	}

	//QMetaType::construct( mType, A, pValue.constData() );
}

QVariant ArrayPin::variant( int pIndex, int pOffset ) const
{
	const quint8	*A = static_cast<const quint8 *>( mData ? mData : mArray );

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

	int				O = ( mStride * pIndex ) + ( mTypeSize * pOffset );

	A += O;

	QVariant		 V;//( mType, static_cast<const void *>( A ) );

	switch( mType )
	{
		case QMetaType::Float:
			V = QVariant( reinterpret_cast<const float *>( A )[ 0 ] );
			break;

		case QMetaType::Int:
			V = QVariant( reinterpret_cast<const int *>( A )[ 0 ] );
			break;

		case QMetaType::QPointF:
			{
				const float	*B = reinterpret_cast<const float *>( A );

				V = QPointF( static_cast<qreal>( B[ 0 ] ), static_cast<qreal>( B[ 1 ] ) );
			}
			break;

		default:
			break;
	}

	return( V );
}

void ArrayPin::updateArray()
{
	if( !mData )
	{
		size_t	NewSze = mStride * std::max<int>( mCount, mReserve );

		if( mArray && mArraySize != NewSze )
		{
			qFreeAligned( mArray );

			mArray = Q_NULLPTR;
		}

		if( !mArray )
		{
			mArray = qMallocAligned( NewSze, 16 );

			if( mArray )
			{
				mArraySize = NewSze;
			}
		}
	}
}
