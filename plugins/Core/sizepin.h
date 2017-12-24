#ifndef SIZEPIN_H
#define SIZEPIN_H

#include <QObject>
#include <QSizeF>

#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>

#include <fugio/core/variant_interface.h>

#include <fugio/pincontrolbase.h>

class SizePin : public fugio::PinControlBase, public fugio::VariantInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::VariantInterface )

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "" )
	Q_CLASSINFO( "URL", WIKI_PIN_URL( "Size" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit SizePin( QSharedPointer<fugio::PinInterface> pPin );

	virtual ~SizePin( void ) {}

	//-------------------------------------------------------------------------
	// fugio::PinControlInterface

	virtual QString toString( void ) const Q_DECL_OVERRIDE
	{
		return( QString( "%1,%2" ).arg( mValues.first().width() ).arg( mValues.first().height() ) );
	}

	virtual QString description( void ) const Q_DECL_OVERRIDE
	{
		return( "Size" );
	}

	//-------------------------------------------------------------------------
	// fugio::VariantInterface

	virtual void setVariant( const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		setVariant( 0, pValue );
	}

	virtual void setVariant( int pIndex, const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		mValues[ pIndex ] = pValue.toSizeF();
	}

	virtual QVariant variant( int pIndex = 0 ) const Q_DECL_OVERRIDE
	{
		return( QVariant::fromValue<QSizeF>( mValues[ pIndex ] ) );
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
		return( QMetaType::QSizeF );
	}

	virtual void setFromBaseVariant( const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		setFromBaseVariant( 0, pValue );
	}

	virtual void setFromBaseVariant( int pIndex, const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		QList<QVariant>     L = pValue.toList();
		QSizeF				V;

		if( L.size() > 0 ) V.setWidth( L.at( 0 ).toReal() );
		if( L.size() > 1 ) V.setHeight( L.at( 1 ).toReal() );

		mValues[ pIndex ] = V;
	}

	virtual QVariant baseVariant( int pIndex ) const Q_DECL_OVERRIDE
	{
		QList<QVariant>		L;
		QSizeF				V = mValues.at( pIndex );

		L << V.width();
		L << V.height();

		return( L );
	}

	virtual void setVariantType( QMetaType::Type ) Q_DECL_OVERRIDE
	{

	}

private:
	QVector<QSizeF>		mValues;
};


#endif // SIZEPIN_H
