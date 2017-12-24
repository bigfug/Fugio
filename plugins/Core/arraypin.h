#ifndef ARRAYPIN_H
#define ARRAYPIN_H

#include <QObject>

#include <QByteArray>
#include <QSettings>

#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>

#include <fugio/core/variant_interface.h>
#include <fugio/core/array_interface.h>
#include <fugio/core/list_interface.h>
#include <fugio/core/size_interface.h>

#include <fugio/pincontrolbase.h>

#include <fugio/serialise_interface.h>

#include "arraylistentry.h"

class ArrayPin : public fugio::PinControlBase, public fugio::VariantInterface, public fugio::SerialiseInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::VariantInterface fugio::SerialiseInterface )

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "" )
	Q_CLASSINFO( "URL", WIKI_PIN_URL( "Array" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit ArrayPin( QSharedPointer<fugio::PinInterface> pPin );

	virtual ~ArrayPin( void ) {}

	//-------------------------------------------------------------------------
	// fugio::PinControlInterface

	virtual QString toString( void ) const Q_DECL_OVERRIDE;

	virtual QString description( void ) const Q_DECL_OVERRIDE
	{
		return( "Array" );
	}

	virtual void loadSettings( QSettings &pSettings ) Q_DECL_OVERRIDE;

	virtual void saveSettings( QSettings &pSettings ) const Q_DECL_OVERRIDE;

	//-------------------------------------------------------------------------
	// fugio::VariantInterface

	virtual void setVariant( const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		setVariant( 0, pValue );
	}

	virtual void setVariant( int pIndex, const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		mValues[ pIndex ] = pValue.value<ArrayListEntry>();
	}

	virtual QVariant variant( int pIndex = 0 ) const Q_DECL_OVERRIDE
	{
		return( QVariant::fromValue<ArrayListEntry>( mValues[ pIndex ] ) );
	}

	virtual void setVariantCount( int pCount )
	{
		mValues.resize( pCount );
	}

	virtual int variantCount( void ) const
	{
		return( mValues.size() );
	}

	inline virtual QMetaType::Type variantType( void ) const
	{
		return( QMetaType::Type( QMetaType::type( "ArrayListEntry" ) ) );
	}

	virtual void setFromBaseVariant( const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		setFromBaseVariant( 0, pValue );
	}

	virtual void setFromBaseVariant( int pIndex, const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		setVariant( pIndex, pValue );
	}

	virtual QVariant baseVariant( int pIndex ) const Q_DECL_OVERRIDE
	{
		return( variant( pIndex ) );
	}

	virtual void setVariantType( QMetaType::Type ) Q_DECL_OVERRIDE
	{

	}

	//-------------------------------------------------------------------------
	// fugio::SerialiseInterface

	virtual void serialise( QDataStream &pDataStream ) const Q_DECL_OVERRIDE
	{
//		mArray.serialise( pDataStream );
	}

	virtual void deserialise( QDataStream &pDataStream ) Q_DECL_OVERRIDE
	{
//		mArray.deserialise( pDataStream );
	}

private:
	QVector<ArrayListEntry>		 mValues;
};

#endif // ARRAYPIN_H
