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
	Q_INVOKABLE explicit Vector3Pin( QSharedPointer<fugio::PinInterface> pPin ) : PinControlBase( pPin ) {}

	virtual ~Vector3Pin( void ) {}

	//-------------------------------------------------------------------------
	// fugio::PinControlInterface

	virtual QString toString( void ) const Q_DECL_OVERRIDE
	{
		return( QString( "%1,%2,%3" ).arg( mValue.x() ).arg( mValue.y() ).arg( mValue.z() ) );
	}

	virtual QString description( void ) const Q_DECL_OVERRIDE
	{
		return( "Vector3" );
	}

	//-------------------------------------------------------------------------
	// fugio::VariantInterface

	virtual void setVariant( const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		mValue = pValue.value<QVector3D>();
	}

	virtual QVariant variant( void ) const Q_DECL_OVERRIDE
	{
		return( QVariant::fromValue<QVector3D>( mValue ) );
	}

	virtual void setFromBaseVariant( const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		QList<QVariant>		L = pValue.toList();

		if( L.size() > 0 ) mValue.setX( L.at( 0 ).toReal() );
		if( L.size() > 1 ) mValue.setY( L.at( 1 ).toReal() );
		if( L.size() > 2 ) mValue.setZ( L.at( 2 ).toReal() );
	}

	virtual QVariant baseVariant( void ) const Q_DECL_OVERRIDE
	{
		QList<QVariant>		L;

		L << mValue.x();
		L << mValue.y();
		L << mValue.z();

		return( L );
	}

private:
	QVector3D			mValue;
};

#endif // VECTOR3PIN_H
