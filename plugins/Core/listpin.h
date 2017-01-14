#ifndef LISTPIN_H
#define LISTPIN_H

#include <QObject>

#include <fugio/core/uuid.h>

#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>

#include <fugio/core/variant_interface.h>
#include <fugio/core/list_interface.h>

#include <fugio/pincontrolbase.h>

class ListPin : public fugio::PinControlBase, public fugio::VariantInterface, public fugio::ListInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::VariantInterface fugio::ListInterface )

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "" )
	Q_CLASSINFO( "URL", WIKI_PIN_URL( "List" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit ListPin( QSharedPointer<fugio::PinInterface> pPin );

	virtual ~ListPin( void ) {}

	//-------------------------------------------------------------------------
	// fugio::PinControlInterface

	virtual QString toString( void ) const Q_DECL_OVERRIDE
	{
		return( QString( "%1" ).arg( mValue.size() ) );
	}

	virtual QString description( void ) const Q_DECL_OVERRIDE
	{
		return( "List" );
	}

	//-------------------------------------------------------------------------
	// fugio::VariantInterface

	virtual void setVariant( const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		mValue = pValue.toList();
	}

	virtual QVariant variant( void ) const Q_DECL_OVERRIDE
	{
		return( mValue );
	}

	virtual void setFromBaseVariant( const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		setVariant( pValue );
	}

	virtual QVariant baseVariant( void ) const Q_DECL_OVERRIDE
	{
		return( variant() );
	}

	//-------------------------------------------------------------------------
	// ListInterface interface

	virtual int listSize() const Q_DECL_OVERRIDE
	{
		return( mValue.size() );
	}

	virtual QUuid listPinControl() const Q_DECL_OVERRIDE
	{
		return( PID_VARIANT );
	}

	virtual QVariant listIndex(int pIndex) const Q_DECL_OVERRIDE
	{
		return( pIndex >= 0 && pIndex < mValue.size() ? mValue.at( pIndex ) : QVariant() );
	}

	virtual void listSetIndex( int pIndex, const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		mValue.insert( pIndex, pValue );
	}

	virtual void listSetSize( int pSize ) Q_DECL_OVERRIDE
	{
		Q_UNUSED( pSize )
	}

	virtual void listClear() Q_DECL_OVERRIDE
	{
		mValue.clear();
	}

	virtual void listAppend( const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		mValue.append( pValue );
	}

	virtual bool listIsEmpty() const Q_DECL_OVERRIDE
	{
		return( mValue.isEmpty() );
	}

private:
	QVariantList		mValue;
};


#endif // LISTPIN_H
