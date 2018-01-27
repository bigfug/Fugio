#ifndef VECTOR3PIN_H
#define VECTOR3PIN_H

#include <fugio/core/uuid.h>

#include <QObject>
#include <QVector3D>

#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>

#include <fugio/core/variant_helper.h>

#include <fugio/pincontrolbase.h>

class Vector3Pin : public fugio::PinControlBase, public fugio::VariantHelper<QVector3D>
{
	Q_OBJECT
	Q_INTERFACES( fugio::VariantInterface )

public:
	Q_INVOKABLE explicit Vector3Pin( QSharedPointer<fugio::PinInterface> pPin );

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

	virtual void setFromBaseVariant( int pIndex, int pOffset, const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		QList<QVariant>     L = pValue.toList();
		QVector3D			V;

		if( L.size() > 0 ) V.setX( L.at( 0 ).toReal() );
		if( L.size() > 1 ) V.setY( L.at( 1 ).toReal() );
		if( L.size() > 2 ) V.setZ( L.at( 2 ).toReal() );

		setVariant( pIndex, pOffset, V );
	}

	virtual QVariant baseVariant( int pIndex, int pOffset ) const Q_DECL_OVERRIDE
	{
		QList<QVariant>		L;
		QVector3D			V = variant( pIndex, pOffset ).value<QVector3D>();

		L << V.x();
		L << V.y();
		L << V.z();

		return( L );
	}
};

#endif // VECTOR3PIN_H
