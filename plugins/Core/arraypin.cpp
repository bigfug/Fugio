#include "arraypin.h"

#include <fugio/core/uuid.h>

#include <QRect>
#include <QRectF>
#include <QLineF>
#include <QColor>

#include "coreplugin.h"

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
	return( CorePlugin::instance()->app()->findPinForMetaType( mType ) );
}

QVariant ArrayPin::listIndex( int pIndex ) const
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

	int				L = QMetaType::sizeOf( mType );

	A += L * pIndex;

	QVariant		 V( mType, (const void *)A );

	return( V );
}

void ArrayPin::listSetIndex( int pIndex, const QVariant &pValue )
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

	int				L = QMetaType::sizeOf( mType );

	A += L * pIndex;

	QMetaType::construct( mType, A, pValue.constData() );
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

	int		NewSze = mArray.size() * QMetaType::sizeOf( mType );

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
