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
	Q_INVOKABLE explicit Vector4Pin( QSharedPointer<fugio::PinInterface> pPin ) : PinControlBase( pPin ) {}

	virtual ~Vector4Pin( void ) {}

	//-------------------------------------------------------------------------
	// fugio::PinControlInterface

	virtual QString toString( void ) const Q_DECL_OVERRIDE
	{
		return( QString( "%1,%2,%3,%4" ).arg( mValue.x() ).arg( mValue.y() ).arg( mValue.z() ).arg( mValue.w() ) );
	}

	virtual QString description( void ) const Q_DECL_OVERRIDE
	{
		return( "Vector4" );
	}

	//-------------------------------------------------------------------------
	// fugio::VariantInterface

	virtual void setVariant( const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		mValue = pValue.value<QVector4D>();
	}

	virtual QVariant variant( void ) const Q_DECL_OVERRIDE
	{
		return( QVariant::fromValue<QVector4D>( mValue ) );
	}

	virtual void setFromBaseVariant( const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		QList<QVariant>		L = pValue.toList();

		if( L.size() > 0 ) mValue.setX( L.at( 0 ).toReal() );
		if( L.size() > 1 ) mValue.setY( L.at( 1 ).toReal() );
		if( L.size() > 2 ) mValue.setZ( L.at( 2 ).toReal() );
		if( L.size() > 3 ) mValue.setW( L.at( 3 ).toReal() );
	}

	virtual QVariant baseVariant( void ) const Q_DECL_OVERRIDE
	{
		QList<QVariant>		L;

		L << mValue.x();
		L << mValue.y();
		L << mValue.z();
		L << mValue.w();

		return( L );
	}

private:
	QVector4D			mValue;
};


#endif // VECTOR4PIN_H
