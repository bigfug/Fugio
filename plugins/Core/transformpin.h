#ifndef TRANSFORMPIN_H
#define TRANSFORMPIN_H

#include <QTransform>

#include <fugio/pincontrolbase.h>

#include <fugio/core/variant_interface.h>

#include <fugio/serialise_interface.h>

class TransformPin : public fugio::PinControlBase, public fugio::VariantInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::VariantInterface )
//	Q_PROPERTY( QTransform mValue READ value WRITE setValue NOTIFY valueChanged )

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "" )
	Q_CLASSINFO( "URL", WIKI_PIN_URL( "QTransform" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit TransformPin( QSharedPointer<fugio::PinInterface> pPin );

	virtual ~TransformPin( void ) {}

	//-------------------------------------------------------------------------
	// Q_PROPERTY

//	Q_INVOKABLE QTransform value( void ) const
//	{
//		return( mValue );
//	}

//	Q_INVOKABLE void setValue( QTransform pValue )
//	{
//		if( pValue != mValue )
//		{
//			mValue = pValue;

//			emit valueChanged( pValue );
//		}
//	}

	//-------------------------------------------------------------------------
	// fugio::PinControlInterface

	virtual QString toString( void ) const Q_DECL_OVERRIDE
	{
		return( QString() );
	}

	virtual QString description( void ) const Q_DECL_OVERRIDE
	{
		return( "Transform" );
	}

	//-------------------------------------------------------------------------
	// fugio::VariantInterface

	virtual void setVariant( const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		setVariant( 0, pValue );
	}

	virtual void setVariant( int pIndex, const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		mValues[ pIndex ] = pValue.value<QTransform>();
	}

	virtual QVariant variant( int pIndex = 0 ) const Q_DECL_OVERRIDE
	{
		return( QVariant::fromValue<QTransform>( mValues[ pIndex ] ) );
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
		return( QMetaType::QTransform );
	}

	virtual void setFromBaseVariant( const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		setFromBaseVariant( 0, pValue );
	}

	virtual void setFromBaseVariant( int pIndex, const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		setVariant( pIndex, pValue );
	}

	virtual QVariant baseVariant( int pIndex ) const Q_DECL_OVERRIDE
	{
		return( variant( pIndex ) );
	}

	virtual void setVariantType( QMetaType::Type ) Q_DECL_OVERRIDE
	{

	}

signals:
	void valueChanged( QTransform pValue );

private:
	QVector<QTransform>		mValues;
};

#endif // TRANSFORMPIN_H
