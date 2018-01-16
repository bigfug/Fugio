#ifndef PIN_VARIANT_ITERATOR_H
#define PIN_VARIANT_ITERATOR_H

#include <QSharedPointer>

#include <fugio/core/variant_interface.h>
#include <fugio/core/list_interface.h>
#include <fugio/pin_interface.h>

FUGIO_NAMESPACE_BEGIN

class PinVariantIterator
{
public:
	PinVariantIterator( const QSharedPointer<fugio::PinInterface> &pPin )
		: mVariant( nullptr ), mList( nullptr )
	{
		QSharedPointer<fugio::PinInterface>	CP = pPin->connectedPin();

		if( CP && CP->hasControl() )
		{
			mVariant = qobject_cast<fugio::VariantInterface *>( CP->control()->qobject() );
			mList    = qobject_cast<fugio::ListInterface *>( CP->control()->qobject() );
		}

		if( !mVariant && !mList )
		{
			mValue = pPin->value();
		}

		mSize = calcsize();
		mType = calctype();
		mIsEmpty = calcisEmpty();
	}

	QVariant index( int pIndex ) const
	{
		if( mList )
		{
			return( mIsEmpty ? QVariant() : mList->listIndex( pIndex % mSize ) );
		}

		return( mVariant ? mVariant->variant( pIndex % mSize ) : mValue );
	}

	inline int size( void ) const
	{
		return( mSize );
	}

	inline bool isEmpty( void ) const
	{
		return( mIsEmpty );
	}

	inline QMetaType::Type type( void ) const
	{
		return( mType );
	}

private:
	int calcsize( void ) const
	{
		if( mList )
		{
			return( mList->listSize() );
		}

		if( mVariant )
		{
			return( mVariant->variantCount() );
		}

		return( mValue.isValid() ? 1 : 0 );
	}

	bool calcisEmpty( void ) const
	{
		if( mList )
		{
			return( mList->listIsEmpty() );
		}

		if( mVariant )
		{
			return( !mVariant->variantCount() );
		}

		return( !mValue.isValid() );
	}

	QMetaType::Type calctype( void ) const
	{
		if( mList )
		{
			return( mList->listType() );
		}

		if( mVariant )
		{
			return( mVariant->variantType() );
		}

		return( QMetaType::Type( mValue.userType() ) );
	}

protected:
	fugio::VariantInterface		*mVariant;
	fugio::ListInterface		*mList;
	QVariant					 mValue;
	QMetaType::Type				 mType;
	int							 mSize;
	bool						 mIsEmpty;
};

FUGIO_NAMESPACE_END

#endif // PIN_VARIANT_ITERATOR_H
