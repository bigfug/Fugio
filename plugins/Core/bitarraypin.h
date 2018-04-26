#ifndef BITARRAYPIN_H
#define BITARRAYPIN_H

#include <QObject>

#include <QBitArray>

#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>

#include <fugio/core/variant_helper.h>
#include <fugio/core/size_interface.h>

#include <fugio/pincontrolbase.h>

#include <fugio/serialise_interface.h>

class BitArrayPin : public fugio::PinControlBase, public fugio::VariantHelper<QBitArray>
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
		QStringList		L;

		for( const QBitArray &V : mValues )
		{
			L << QString( "%1 bits" ).arg( V.size() );
		}

		return( L.join( ',' ) );
	}

	virtual QString description( void ) const Q_DECL_OVERRIDE
	{
		return( "Bit Array" );
	}

	//-------------------------------------------------------------------------
	// fugio::VariantInterface

	virtual void setFromBaseVariant( int pIndex, int pOffset, const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		QList<QVariant>     L = pValue.toList();
		QBitArray			V;

		V.resize( L.size() );

		for( int i = 0 ; i < L.size() ; i++ )
		{
			V.setBit( i, L.at( i ).toBool() );
		}

		setVariant( pIndex, pOffset, V );
	}

	virtual QVariant baseVariant( int pIndex, int pOffset ) const Q_DECL_OVERRIDE
	{
		QList<QVariant>		L;
		QBitArray			V = variant( pIndex, pOffset ).toBitArray();

		for( int i = 0 ; i < V.size() ; i++ )
		{
			L << V.testBit( i );
		}

		return( L );
	}
};

#endif // BITARRAYPIN_H
