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

	virtual QString toString( void ) const Q_DECL_OVERRIDE
	{
		return( QString() ); //QString( "x=%1 y=%2 w=%3 h=%4" ).arg( mValue.x() ).arg( mValue.y() ).arg( mValue.width() ).arg( mValue.height() ) );
	}

	virtual QString description( void ) const Q_DECL_OVERRIDE
	{
		return( "Rect" );
	}

	//-------------------------------------------------------------------------
	// fugio::VariantInterface

	virtual void setVariant( const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		setVariant( 0, pValue );
	}

	virtual void setVariant( int pIndex, const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		mValues[ pIndex ] = pValue.toRectF();
	}

	virtual QVariant variant( int pIndex = 0 ) const Q_DECL_OVERRIDE
	{
		return( QVariant::fromValue<QRectF>( mValues[ pIndex ] ) );
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
		return( QMetaType::QRectF );
	}

	virtual void setFromBaseVariant( const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		setFromBaseVariant( 0, pValue );
	}

	virtual void setFromBaseVariant( int pIndex, const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		QList<QVariant>		L = pValue.toList();
		QRectF				V;

		if( L.size() > 0 ) V.setX( L.at( 0 ).toReal() );
		if( L.size() > 1 ) V.setY( L.at( 1 ).toReal() );
		if( L.size() > 2 ) V.setWidth( L.at( 0 ).toReal() );
		if( L.size() > 3 ) V.setHeight( L.at( 1 ).toReal() );

		mValues[ pIndex ] = V;
	}

	virtual QVariant baseVariant( int pIndex ) const Q_DECL_OVERRIDE
	{
		QList<QVariant>		L;
		QRectF				V = mValues.at( pIndex );

		L << V.x();
		L << V.y();
		L << V.width();
		L << V.height();

		return( L );
	}

	virtual void setVariantType( QMetaType::Type ) Q_DECL_OVERRIDE
	{

	}

private:
	QVector<QRectF>		mValues;
};

#endif // RECTPIN_H
