#include "arraypin.h"

#include <fugio/core/uuid.h>

ArrayPin::ArrayPin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin ), mData( nullptr ), mType( QMetaType::UnknownType ), mStride( 0 ), mCount( 0 ), mSize( 0 )
{

}

QString ArrayPin::toString() const
{
	return( QString( "<p>Count: %1</p><p>Size: %2</p><p>Total Size: %3</p>" ).arg( count() ).arg( size() ).arg( count() * stride() ) );
}

void ArrayPin::loadSettings( QSettings &pSettings )
{
	mArray = pSettings.value( "array", mArray ).toByteArray();
}

void ArrayPin::saveSettings( QSettings &pSettings ) const
{
	pSettings.setValue( "array", mArray );
}


int ArrayPin::listSize() const
{
	return( mCount );
}

QUuid ArrayPin::listPinControl() const
{
	switch( mType )
	{
		case QMetaType::Float:
			return( PID_FLOAT );

		default:
			break;
	}

	return( QUuid() );
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

		default:
			break;
	}

	return( V );
}
