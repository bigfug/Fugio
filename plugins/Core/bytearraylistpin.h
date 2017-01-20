#ifndef BYTEARRAYLISTPIN_H
#define BYTEARRAYLISTPIN_H

#include <QObject>

#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>
#include <fugio/core/list_interface.h>

#include <fugio/core/variant_interface.h>

#include <fugio/pincontrolbase.h>

#include <fugio/serialise_interface.h>

class ByteArrayListPin : public fugio::PinControlBase, public fugio::VariantInterface, public fugio::SerialiseInterface, public fugio::ListInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::VariantInterface fugio::SerialiseInterface fugio::ListInterface )

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "" )
	Q_CLASSINFO( "URL", WIKI_PIN_URL( "ByteArray_List" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit ByteArrayListPin( QSharedPointer<fugio::PinInterface> pPin );

	virtual ~ByteArrayListPin( void ) {}

	//-------------------------------------------------------------------------
	// fugio::PinControlInterface

	virtual QString toString( void ) const Q_DECL_OVERRIDE
	{
		return( QString( "Count: %1" ).arg( mValue.size() ) );
	}

	virtual QString description( void ) const Q_DECL_OVERRIDE
	{
		return( "Byte Array List" );
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
	// fugio::SerialiseInterface

	virtual void serialise( QDataStream &pDataStream ) Q_DECL_OVERRIDE
	{
		pDataStream << mValue;
	}

	virtual void deserialise( QDataStream &pDataStream ) Q_DECL_OVERRIDE
	{
		pDataStream >> mValue;
	}

	//-------------------------------------------------------------------------
	// fugio::ListInterface

	virtual int listSize() const Q_DECL_OVERRIDE
	{
		return( mValue.size() );
	}

	virtual QUuid listPinControl( void ) const Q_DECL_OVERRIDE;

	virtual QVariant listIndex( int pIndex ) const Q_DECL_OVERRIDE
	{
		return( mValue.at( pIndex ) );
	}

	virtual void listSetIndex( int pIndex, const QVariant &pValue ) Q_DECL_OVERRIDE;

	virtual void listSetSize( int pSize ) Q_DECL_OVERRIDE
	{
		if( !pSize )
		{
			mValue.clear();
		}

		while( mValue.size() > pSize )
		{
			mValue.removeLast();
		}

		while( mValue.size() < pSize )
		{
			mValue.append( QByteArray() );
		}
	}

	virtual void listClear() Q_DECL_OVERRIDE
	{
		mValue.clear();
	}

	virtual void listAppend( const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		mValue << pValue.value<QByteArray>();
	}

	virtual bool listIsEmpty() const Q_DECL_OVERRIDE
	{
		return( mValue.isEmpty() );
	}

private:
	QVariantList		mValue;
};

#endif // BYTEARRAYLISTPIN_H
