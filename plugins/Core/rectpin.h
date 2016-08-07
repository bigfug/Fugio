#ifndef RECTPIN_H
#define RECTPIN_H

#include <QObject>
#include <QRectF>

#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>

#include <fugio/core/variant_interface.h>

#include <fugio/pincontrolbase.h>

class RectPin : public fugio::PinControlBase, public fugio::VariantInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::VariantInterface )

public:
	Q_INVOKABLE explicit RectPin( QSharedPointer<fugio::PinInterface> pPin );

	virtual ~RectPin( void ) {}

	//-------------------------------------------------------------------------
	// fugio::PinControlInterface

	virtual QString toString( void ) const Q_DECL_OVERRIDE
	{
		return( QString( "x=%1 y=%2 w=%3 h=%4" ).arg( mValue.x() ).arg( mValue.y() ).arg( mValue.width() ).arg( mValue.height() ) );
	}

	virtual QString description( void ) const Q_DECL_OVERRIDE
	{
		return( "Rect" );
	}

	//-------------------------------------------------------------------------
	// fugio::VariantInterface

	virtual void setVariant( const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		mValue = pValue.toRectF();
	}

	virtual QVariant variant( void ) const Q_DECL_OVERRIDE
	{
		return( QVariant::fromValue<QRectF>( mValue ) );
	}

	virtual void setFromBaseVariant( const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		QList<QVariant>		L = pValue.toList();

		if( L.size() > 0 ) mValue.setX( L.at( 0 ).toReal() );
		if( L.size() > 1 ) mValue.setY( L.at( 1 ).toReal() );
		if( L.size() > 2 ) mValue.setWidth( L.at( 0 ).toReal() );
		if( L.size() > 3 ) mValue.setHeight( L.at( 1 ).toReal() );
	}

	virtual QVariant baseVariant( void ) const Q_DECL_OVERRIDE
	{
		QList<QVariant>		L;

		L << mValue.x();
		L << mValue.y();
		L << mValue.width();
		L << mValue.height();

		return( L );
	}

private:
	QRectF		mValue;
};

#endif // RECTPIN_H
