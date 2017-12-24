#ifndef BITARRAYPIN_H
#define BITARRAYPIN_H

#include <QObject>

#include <QBitArray>

#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>

#include <fugio/core/variant_interface.h>
#include <fugio/core/size_interface.h>

#include <fugio/pincontrolbase.h>

#include <fugio/serialise_interface.h>

class BitArrayPin : public fugio::PinControlBase, public fugio::VariantInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::VariantInterface )

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "" )
	Q_CLASSINFO( "URL", WIKI_PIN_URL( "BitArray" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit BitArrayPin( QSharedPointer<fugio::PinInterface> pPin );

	virtual ~BitArrayPin( void ) {}

	//-------------------------------------------------------------------------
	// fugio::PinControlInterface

	virtual QString toString( void ) const Q_DECL_OVERRIDE
	{
		return( QString() ); //QString( "%1 bits" ).arg( mValue.size() ) );
	}

	virtual QString description( void ) const Q_DECL_OVERRIDE
	{
		return( "Bit Array" );
	}

	//-------------------------------------------------------------------------
	// fugio::VariantInterface

	virtual void setVariant( const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		setVariant( 0, pValue );
	}

	virtual void setVariant( int pIndex, const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		mValues[ pIndex ] = pValue.toBitArray();
	}

	virtual QVariant variant( int pIndex = 0 ) const Q_DECL_OVERRIDE
	{
		return( QVariant::fromValue<QBitArray>( mValues[ pIndex ] ) );
	}

	virtual void setVariantCount( int pCount ) Q_DECL_OVERRIDE
	{
		mValues.resize( pCount );
	}

	virtual int variantCount( void ) const Q_DECL_OVERRIDE
	{
		return( mValues.size() );
	}

	inline virtual QMetaType::Type variantType( void ) const Q_DECL_OVERRIDE
	{
		return( QMetaType::QBitArray );
	}

	virtual void setFromBaseVariant( const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		setFromBaseVariant( 0, pValue );
	}

	virtual void setFromBaseVariant( int pIndex, const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		QList<QVariant>     L = pValue.toList();
		QBitArray			V;

		V.resize( L.size() );

		for( int i = 0 ; i < L.size() ; i++ )
		{
			V.setBit( i, L.at( i ).toBool() );
		}

		mValues[ pIndex ] = V;
	}

	virtual QVariant baseVariant( int pIndex ) const Q_DECL_OVERRIDE
	{
		QList<QVariant>		L;
		QBitArray			V = mValues.at( pIndex );

		for( int i = 0 ; i < V.size() ; i++ )
		{
			L << V.testBit( i );
		}

		return( L );
	}

	virtual void setVariantType( QMetaType::Type ) Q_DECL_OVERRIDE
	{

	}

private:
	QVector<QBitArray>		mValues;
};

#endif // BITARRAYPIN_H
