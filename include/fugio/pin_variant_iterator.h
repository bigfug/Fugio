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
	}

	int size( void ) const
	{
		if( mList )
		{
			return( mList->listSize() );
		}

		if( mVariant || mValue.isValid() )
		{
			return( 1 );
		}

		return( 0 );
	}

	QVariant index( int pIndex ) const
	{
		if( mList )
		{
			return( mList->listIndex( pIndex ) );
		}

		if( pIndex )
		{
			return( QVariant() );
		}

		return( mVariant ? mVariant->variant() : mValue );
	}

	bool isEmpty( void ) const
	{
		if( mList )
		{
			return( mList->listIsEmpty() );
		}

		if( mVariant || mValue.isValid() )
		{
			return( false );
		}

		return( true );
	}

protected:
	fugio::VariantInterface		*mVariant;
	fugio::ListInterface		*mList;
	QVariant					 mValue;
};

FUGIO_NAMESPACE_END

#endif // PIN_VARIANT_ITERATOR_H
