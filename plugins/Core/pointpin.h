#ifndef POINTPIN_H
#define POINTPIN_H

#include <QObject>
#include <QPointF>

#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>

#include <fugio/core/variant_helper.h>

#include <fugio/pincontrolbase.h>

class PointPin : public fugio::PinControlBase, public fugio::VariantHelper<QPointF>
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

	virtual ~PointPin( void ) {}

	//-------------------------------------------------------------------------
	// fugio::PinControlInterface

	virtual QString toString( void ) const Q_DECL_OVERRIDE
	{
		QStringList		L;

		for( const QPointF &V : mValues )
		{
			L << QString( "(%1,%2)" ).arg( V.x() ).arg( V.y() );
		}

		return( L.join( ',' ) );
	}

	virtual QString description( void ) const Q_DECL_OVERRIDE
	{
		return( "Point" );
	}

	//-------------------------------------------------------------------------
	// fugio::VariantInterface

	virtual void setFromBaseVariant( int pIndex, int pOffset, const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		const int			i = variantIndex( pIndex, pOffset );
		QList<QVariant>     L = pValue.toList();
		QPointF				V;

		if( L.size() > 0 ) V.setX( L.at( 0 ).toReal() );
		if( L.size() > 1 ) V.setY( L.at( 1 ).toReal() );

		mValues[ i ] = V;
	}

	virtual QVariant baseVariant( int pIndex, int pOffset ) const Q_DECL_OVERRIDE
	{
		QList<QVariant>		L;
		QPointF				V = mValues.at( variantIndex( pIndex, pOffset ) );

		L << V.x();
		L << V.y();

		return( L );
	}
};

#endif // POINTPIN_H
