#ifndef BYTEARRAYLISTPIN_H
#define BYTEARRAYLISTPIN_H

#include <QObject>

#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>
#include <fugio/core/list_interface.h>

#include <fugio/core/variant_interface.h>

#include <fugio/pincontrolbase.h>

#include <fugio/serialise_interface.h>

class ByteArrayListPin : public fugio::PinControlBase, public fugio::VariantInterface, public fugio::SerialiseInterface, fugio::ListInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::VariantInterface fugio::SerialiseInterface fugio::ListInterface )

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

private:
	QVariantList		mValue;
};

#endif // BYTEARRAYLISTPIN_H
