#ifndef VECTOR4PIN_H
#define VECTOR4PIN_H

#include <QVector4D>

#include <fugio/pincontrolbase.h>
#include <fugio/core/variant_interface.h>

class Vector4Pin : public fugio::PinControlBase, public fugio::VariantInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::VariantInterface )

public:
	Q_INVOKABLE explicit Vector4Pin( QSharedPointer<fugio::PinInterface> pPin ) : PinControlBase( pPin ), mValues( 1 ) {}

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

	virtual void setVariant( const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		setVariant( 0, pValue );
	}

	virtual void setVariant( int pIndex, const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		mValues[ pIndex ] = pValue.value<QVector4D>();
	}

	virtual QVariant variant( int pIndex = 0 ) const Q_DECL_OVERRIDE
	{
		return( QVariant::fromValue<QVector4D>( mValues[ pIndex ] ) );
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
		return( QMetaType::QVector4D );
	}

	virtual void setFromBaseVariant( const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		setFromBaseVariant( 0, pValue );
	}

	virtual void setFromBaseVariant( int pIndex, const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		QList<QVariant>     L = pValue.toList();
		QVector4D			V;

		if( L.size() > 0 ) V.setX( L.at( 0 ).toReal() );
		if( L.size() > 1 ) V.setY( L.at( 1 ).toReal() );
		if( L.size() > 2 ) V.setZ( L.at( 2 ).toReal() );
		if( L.size() > 3 ) V.setW( L.at( 3 ).toReal() );

		mValues[ pIndex ] = V;
	}

	virtual QVariant baseVariant( int pIndex ) const Q_DECL_OVERRIDE
	{
		QList<QVariant>		L;
		QVector4D			V = mValues.at( pIndex );

		L << V.x();
		L << V.y();
		L << V.z();
		L << V.w();

		return( L );
	}

	virtual void setVariantType( QMetaType::Type ) Q_DECL_OVERRIDE
	{

	}

private:
	QVector<QVector4D>			mValues;
};

#endif // VECTOR4PIN_H
