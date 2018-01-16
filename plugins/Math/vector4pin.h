#ifndef VECTOR4PIN_H
#define VECTOR4PIN_H

#include <QVector4D>

#include <fugio/pincontrolbase.h>
#include <fugio/core/variant_helper.h>

class Vector4Pin : public fugio::PinControlBase, public fugio::VariantHelper<QVector4D>
{
	Q_OBJECT
	Q_INTERFACES( fugio::VariantInterface )

public:
	Q_INVOKABLE explicit Vector4Pin( QSharedPointer<fugio::PinInterface> pPin );

	virtual ~Vector4Pin( void ) {}

	//-------------------------------------------------------------------------
	// fugio::PinControlInterface

	virtual QString toString( void ) const Q_DECL_OVERRIDE
	{
		return( QString() );//QString( "%1,%2,%3,%4" ).arg( mValue.x() ).arg( mValue.y() ).arg( mValue.z() ).arg( mValue.w() ) );
	}

	virtual QString description( void ) const Q_DECL_OVERRIDE
	{
		return( "Vector4" );
	}

	//-------------------------------------------------------------------------
	// fugio::VariantInterface

	virtual void setFromBaseVariant( int pIndex, int pOffset, const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		QList<QVariant>     L = pValue.toList();
		QVector4D			V;

		if( L.size() > 0 ) V.setX( L.at( 0 ).toReal() );
		if( L.size() > 1 ) V.setY( L.at( 1 ).toReal() );
		if( L.size() > 2 ) V.setZ( L.at( 2 ).toReal() );
		if( L.size() > 3 ) V.setW( L.at( 3 ).toReal() );

		mValues[ variantIndex( pIndex, pOffset ) ] = V;
	}

	virtual QVariant baseVariant( int pIndex, int pOffset ) const Q_DECL_OVERRIDE
	{
		QList<QVariant>		L;
		QVector4D			V = mValues[ variantIndex( pIndex, pOffset ) ];

		L << V.x();
		L << V.y();
		L << V.z();
		L << V.w();

		return( L );
	}
};

#endif // VECTOR4PIN_H
