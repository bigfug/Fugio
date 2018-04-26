#ifndef RECTPIN_H
#define RECTPIN_H

#include <QObject>
#include <QRectF>

#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>

#include <fugio/core/variant_helper.h>

#include <fugio/pincontrolbase.h>

class RectPin : public fugio::PinControlBase, public fugio::VariantHelper<QRectF>
{
	Q_OBJECT
	Q_INTERFACES( fugio::VariantInterface )

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "" )
	Q_CLASSINFO( "URL", WIKI_PIN_URL( "Rect" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit RectPin( QSharedPointer<fugio::PinInterface> pPin );

	virtual ~RectPin( void ) {}

	//-------------------------------------------------------------------------
	// fugio::PinControlInterface

	virtual QString toString( void ) const Q_DECL_OVERRIDE;

	virtual QString description( void ) const Q_DECL_OVERRIDE
	{
		return( "Rect" );
	}

	//-------------------------------------------------------------------------
	// fugio::VariantInterface

	virtual void setFromBaseVariant( int pIndex, int pOffset, const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		QRectF				V;

		if( QMetaType::Type( pValue.type() ) == QMetaType::QVariantMap )
		{
			const QVariantMap		M = pValue.toMap();

			V.setX( M.value( "x", V.x() ).toReal() );
			V.setY( M.value( "y", V.y() ).toReal() );

			V.setWidth( M.value( "width", V.width() ).toReal() );
			V.setHeight( M.value( "height", V.height() ).toReal() );
		}
		else if( QMetaType::Type( pValue.type() ) == QMetaType::QVariantList )
		{
			const QVariantList		L = pValue.toList();

			if( L.size() > 0 ) V.setX( L.at( 0 ).toReal() );
			if( L.size() > 1 ) V.setY( L.at( 1 ).toReal() );
			if( L.size() > 2 ) V.setWidth( L.at( 0 ).toReal() );
			if( L.size() > 3 ) V.setHeight( L.at( 1 ).toReal() );
		}

		mValues[ variantIndex( pIndex, pOffset ) ] = V;
	}

	virtual QVariant baseVariant( int pIndex, int pOffset ) const Q_DECL_OVERRIDE
	{
		QList<QVariant>		L;
		QRectF				V = mValues.at( variantIndex( pIndex, pOffset ) );

		L << V.x();
		L << V.y();
		L << V.width();
		L << V.height();

		return( L );
	}
};

#endif // RECTPIN_H
