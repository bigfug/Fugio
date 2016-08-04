#include "arraylistpin.h"

#include <fugio/core/uuid.h>

ArrayListPin::ArrayListPin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin )
{

}

int ArrayListPin::count() const
{
	return( mArrayList.size() );
}

fugio::ArrayInterface *ArrayListPin::arrayIndex( int pIndex )
{
	return( &mArrayList[ pIndex ] );
}

void ArrayListPin::setCount( int pIndex )
{
	mArrayList.resize( pIndex );
}

void ArrayListPin::clear()
{
	mArrayList.clear();
}

int ArrayListPin::listSize() const
{
	return( mArrayList.size() );
}

QUuid ArrayListPin::listPinControl() const
{
	return( PID_ARRAY );
}

QVariant ArrayListPin::listIndex( int pIndex ) const
{
	Q_UNUSED( pIndex )

	return( QVariant() );
}

void ArrayListPin::listSetIndex(int pIndex, const QVariant &pValue)
{
	Q_UNUSED( pIndex )
	Q_UNUSED( pValue )
}

void ArrayListPin::listSetSize( int pSize )
{
	Q_UNUSED( pSize )
}

void ArrayListPin::listClear()
{
	mArrayList.clear();
}

void ArrayListPin::listAppend( const QVariant &pValue )
{
	Q_UNUSED( pValue )
}

bool ArrayListPin::listIsEmpty() const
{
	return( mArrayList.isEmpty() );
}
