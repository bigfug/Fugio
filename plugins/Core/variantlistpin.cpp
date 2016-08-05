#include "variantlistpin.h"

#include <fugio/core/uuid.h>

#include <QMetaType>

VariantListPin::VariantListPin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin )
{

}

int VariantListPin::listSize() const
{
	return( mValue.size() );
}

QUuid VariantListPin::listPinControl() const
{
	return( PID_VARIANT );
}

QVariant VariantListPin::listIndex( int pIndex ) const
{
	return( mValue.at( pIndex ) );
}

void VariantListPin::listSetIndex( int pIndex, const QVariant &pValue )
{
	mValue[ pIndex ] = pValue;
}

void VariantListPin::listSetSize( int pSize )
{
	mValue.reserve( pSize );

	while( mValue.size() < pSize )
	{
		mValue << QVariant();
	}

	while( mValue.size() > pSize )
	{
		mValue.removeLast();
	}
}

void VariantListPin::listClear()
{
	mValue.clear();
}

void VariantListPin::listAppend( const QVariant &pValue )
{
	mValue << pValue;
}

bool VariantListPin::listIsEmpty() const
{
	return( mValue.isEmpty() );
}
