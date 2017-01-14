#ifndef POINTPIN_H
#define POINTPIN_H

#include <QObject>
#include <QPointF>

#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>

#include <fugio/core/variant_interface.h>

#include <fugio/pincontrolbase.h>

class PointPin : public fugio::PinControlBase, public fugio::VariantInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::VariantInterface )

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "" )
	Q_CLASSINFO( "URL", WIKI_PIN_URL( "Point" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit PointPin( QSharedPointer<fugio::PinInterface> pPin );

	virtual ~PointPin( void );

	//-------------------------------------------------------------------------
	// fugio::PinControlInterface

	virtual QString toString( void ) const Q_DECL_OVERRIDE
	{
		return( QString( "%1,%2" ).arg( mValue.x() ).arg( mValue.y() ) );
	}

	virtual QString description( void ) const Q_DECL_OVERRIDE
	{
		return( "Point" );
	}

	//-------------------------------------------------------------------------
	// fugio::VariantInterface

	virtual void setVariant( const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		mValue = pValue.toPointF();
	}

	virtual QVariant variant( void ) const Q_DECL_OVERRIDE
	{
		return( QVariant::fromValue<QPointF>( mValue ) );
	}

	virtual void setFromBaseVariant( const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		QList<QVariant>		L = pValue.toList();

		if( L.size() > 0 ) mValue.setX( L.at( 0 ).toReal() );
		if( L.size() > 1 ) mValue.setY( L.at( 1 ).toReal() );
	}

	virtual QVariant baseVariant( void ) const Q_DECL_OVERRIDE
	{
		QList<QVariant>		L;

		L << mValue.x();
		L << mValue.y();

		return( L );
	}

private:
	QPointF		mValue;
};

#endif // POINTPIN_H
