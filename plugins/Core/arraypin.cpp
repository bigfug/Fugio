#include "arraypin.h"

#include <fugio/core/uuid.h>

#include <QRect>
#include <QRectF>
#include <QLineF>

ArrayPin::ArrayPin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin ), mData( nullptr ),
	  mType( QMetaType::UnknownType ), mStride( 0 ), mCount( 0 ), mSize( 0 ), mReserve( 0 )
{

}

QString ArrayPin::toString() const
{
	return( QString( "<p>Type: %1</p><p>Count: %2</p><p>Size: %3</p><p>Total Size: %4</p>" ).arg( QString( QMetaType::typeName( type() ) ) ).arg( count() ).arg( size() ).arg( count() * stride() ) );
}

void ArrayPin::loadSettings( QSettings &pSettings )
{
	mArray = pSettings.value( "array", mArray ).toByteArray();
}

void ArrayPin::saveSettings( QSettings &pSettings ) const
{
	pSettings.setValue( "array", mArray );
}

QVariant ArrayPin::baseVariant() const
{
	QVariantList		VarLst;

	const void		*A = ( mData ? mData : ( !mArray.isEmpty() ? mArray.data() : nullptr ) );

	if( A )
	{
		switch( mType )
		{
			case QMetaType::Float:
				{
					const float	*V = static_cast<const float *>( A );

					for( int i = 0 ; i < mCount ; i++ )
					{
						VarLst << QVariant( V[ i ] );
					}
				}
				break;

			default:
				break;
		}
	}

	return( VarLst );
}

int ArrayPin::listSize() const
{
	return( mCount );
}

QUuid ArrayPin::listPinControl() const
{
	switch( mType )
	{
		case QMetaType::Bool:
			return( PID_BOOL );

		case QMetaType::UInt:
		case QMetaType::Int:
		case QMetaType::Long:
		case QMetaType::LongLong:
		case QMetaType::Short:
			return( PID_INTEGER );

		case QMetaType::QChar:
		case QMetaType::QString:
			return( PID_STRING );

		case QMetaType::QByteArray:
			return( PID_BYTEARRAY );

		case QMetaType::Float:
		case QMetaType::Double:
			return( PID_FLOAT );

		case QMetaType::QPoint:
		case QMetaType::QPointF:
			return( PID_POINT );

		case QMetaType::QSize:
		case QMetaType::QSizeF:
			return( PID_SIZE );

		default:
			break;
	}

	return( PID_VARIANT );
}

QVariant ArrayPin::listIndex( int pIndex ) const
{
	QVariant		 V;
	const void		*A = ( mData ? mData : ( !mArray.isEmpty() ? mArray.data() : nullptr ) );

	if( !A )
	{
		return( V );
	}

	if( pIndex < 0 || pIndex >= mCount )
	{
		return( V );
	}

	switch( mType )
	{
		case QMetaType::Float:
			V = static_cast<const float *>( A )[ pIndex ];
			break;

		case QMetaType::QRect:
			V = static_cast<const QRect *>( A )[ pIndex ];
			break;

		case QMetaType::QRectF:
			V = static_cast<const QRectF *>( A )[ pIndex ];
			break;

		case QMetaType::QLineF:
			V = static_cast<const QLineF *>( A )[ pIndex ];
			break;

		case QMetaType::QPointF:
			V = static_cast<const QPointF *>( A )[ pIndex ];
			break;

		default:
			break;
	}

	return( V );
}

void ArrayPin::listSetIndex( int pIndex, const QVariant &pValue )
{
	void		*A = ( mData ? mData : ( !mArray.isEmpty() ? mArray.data() : nullptr ) );

	if( !A )
	{
		return;
	}

	if( pIndex < 0 || pIndex >= mCount )
	{
		return;
	}

	switch( mType )
	{
		case QMetaType::Float:
			{
				static_cast<float *>( A )[ pIndex ] = pValue.toFloat();
			}
			break;

		default:
			break;
	}
}

void ArrayPin::listSetSize( int pSize )
{
	if( mData )
	{
		return;
	}

	if( pSize == mCount )
	{
		return;
	}

	int		NewSze = mArray.size();

	switch( mType )
	{
		case QMetaType::Float:
			{
				NewSze = pSize * mStride;
			}
			break;

		default:
			break;
	}

	if( mArray.size() != NewSze )
	{
		mArray.resize( NewSze );
	}
}


int ArrayPin::sizeDimensions() const
{
	return( 1 );
}

float ArrayPin::size(int pDimension) const
{
	return( pDimension == 0 ? mCount : 0 );
}

float ArrayPin::sizeWidth() const
{
	return( mCount );
}

float ArrayPin::sizeHeight() const
{
	return( 0 );
}

float ArrayPin::sizeDepth() const
{
	return( 0 );
}

QSizeF ArrayPin::toSizeF() const
{
	return( QSizeF( mCount, 0 ) );
}

QVector3D ArrayPin::toVector3D() const
{
	return( QVector3D( mCount, 0, 0 ) );
}
