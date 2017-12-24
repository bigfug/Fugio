#ifndef VECTOR3PIN_H
#define VECTOR3PIN_H

#include <fugio/core/uuid.h>

#include <QObject>
#include <QVector3D>

#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>

#include <fugio/core/variant_interface.h>

#include <fugio/pincontrolbase.h>

class Vector3Pin : public fugio::PinControlBase, public fugio::VariantInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::VariantInterface )

public:
	Q_INVOKABLE explicit Vector3Pin( QSharedPointer<fugio::PinInterface> pPin ) : PinControlBase( pPin ), mValues( 1 ) {}

	virtual ~Vector3Pin( void ) {}

	//-------------------------------------------------------------------------
	// fugio::PinControlInterface

	virtual QString toString( void ) const Q_DECL_OVERRIDE
	{
		return( QString() ); //QString( "%1,%2,%3" ).arg( mValue.x() ).arg( mValue.y() ).arg( mValue.z() ) );
	}

	virtual QString description( void ) const Q_DECL_OVERRIDE
	{
		return( "Vector3" );
	}

	//-------------------------------------------------------------------------
	// fugio::VariantInterface

	virtual void setVariant( const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		setVariant( 0, pValue );
	}

	virtual void setVariant( int pIndex, const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		mValues[ pIndex ] = pValue.value<QVector3D>();
	}

	virtual QVariant variant( int pIndex = 0 ) const Q_DECL_OVERRIDE
	{
		return( QVariant::fromValue<QVector3D>( mValues[ pIndex ] ) );
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
		return( QMetaType::QVector3D );
	}

	virtual void setFromBaseVariant( const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		setFromBaseVariant( 0, pValue );
	}

	virtual void setFromBaseVariant( int pIndex, const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		QList<QVariant>     L = pValue.toList();
		QVector3D			V;

		if( L.size() > 0 ) V.setX( L.at( 0 ).toReal() );
		if( L.size() > 1 ) V.setY( L.at( 1 ).toReal() );
		if( L.size() > 2 ) V.setZ( L.at( 2 ).toReal() );

		mValues[ pIndex ] = V;
	}

	virtual QVariant baseVariant( int pIndex ) const Q_DECL_OVERRIDE
	{
		QList<QVariant>		L;
		QVector3D			V = mValues.at( pIndex );

		L << V.x();
		L << V.y();
		L << V.z();

		return( L );
	}

	virtual void setVariantType( QMetaType::Type ) Q_DECL_OVERRIDE
	{

	}

private:
	QVector<QVector3D>			mValues;
};

#endif // VECTOR3PIN_H
