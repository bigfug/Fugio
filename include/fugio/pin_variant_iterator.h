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

		mCount = calcCount();
		mType = calcType();
		mIsEmpty = calcisEmpty();
	}

	QVariant index( int pIndex ) const
	{
		if( mList )
		{
			return( mIsEmpty ? QVariant() : mList->listIndex( pIndex % mCount ) );
		}

		return( mVariant ? mVariant->variant( pIndex % mCount ) : mValue );
	}

	inline int count( void ) const
	{
		return( mCount );
	}

	inline bool isEmpty( void ) const
	{
		return( mIsEmpty );
	}

	inline QMetaType::Type type( void ) const
	{
		return( mType );
	}

	inline QVariant size( int pIndex )
	{
		if( mList )
		{
			return( mList->listSize() );
		}

		return( mVariant ? mVariant->variantSize( pIndex % mCount ) : mCount );
	}

private:
	int calcCount( void ) const
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

	QMetaType::Type calcType( void ) const
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
	int							 mCount;
	bool						 mIsEmpty;
};

FUGIO_NAMESPACE_END

#endif // PIN_VARIANT_ITERATOR_H
