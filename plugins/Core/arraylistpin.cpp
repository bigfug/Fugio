#include "arraylistpin.h"

ArrayListPin::ArrayListPin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin )
{

}

int ArrayListPin::count() const
{
	return( mArrayList.size() );
}

fugio::ArrayInterface *ArrayListPin::array( int pIndex )
{
	return( mArrayList.value( pIndex ) );
}

void ArrayListPin::appendArray( fugio::ArrayInterface *pArray )
{
	return( mArrayList.append( pArray ) );
}

void ArrayListPin::setArray( int pIndex, fugio::ArrayInterface *pArray)
{
	mArrayList[ pIndex ] = pArray;
}

void ArrayListPin::clear()
{
	mArrayList.clear();
}

void ArrayListPin::removeAll(fugio::ArrayInterface *pArray)
{
	mArrayList.removeAll( pArray );
}

void ArrayListPin::removeAt(int pIndex)
{
	mArrayList.removeAt( pIndex );
}
