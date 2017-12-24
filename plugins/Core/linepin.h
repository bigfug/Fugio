#ifndef LINEPIN_H
#define LINEPIN_H

#include <QObject>
#include <QLineF>

#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>

#include <fugio/core/variant_interface.h>

#include <fugio/pincontrolbase.h>

#include <fugio/serialise_interface.h>

class LinePin : public fugio::PinControlBase, public fugio::VariantInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::VariantInterface )
//	Q_PROPERTY( QLineF mValue READ value WRITE setValue NOTIFY valueChanged )

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "" )
	Q_CLASSINFO( "URL", WIKI_PIN_URL( "Line" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit LinePin( QSharedPointer<fugio::PinInterface> pPin );

	virtual ~LinePin( void ) {}

	//-------------------------------------------------------------------------
	// Q_PROPERTY

//	Q_INVOKABLE QLineF value( void ) const
//	{
//		return( mValue );
//	}

//	Q_INVOKABLE void setValue( QLineF pValue )
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
//		return( QString( "%1,%2 - %3,%4 (%5)" )
//				.arg( mValue.p1().x() ).arg( mValue.p1().y() )
//				.arg( mValue.p2().x() ).arg( mValue.p2().y() )
//				.arg( mValue.length() ) );

		return( QString() );
	}

	virtual QString description( void ) const Q_DECL_OVERRIDE
	{
		return( "Line" );
	}

	//-------------------------------------------------------------------------
	// fugio::VariantInterface

	virtual void setVariant( const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		setVariant( 0, pValue );
	}

	virtual void setVariant( int pIndex, const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		mValues[ pIndex ] = pValue.toLineF();
	}

	virtual QVariant variant( int pIndex = 0 ) const Q_DECL_OVERRIDE
	{
		return( QVariant::fromValue<QLineF>( mValues[ pIndex ] ) );
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
		return( QMetaType::QLineF );
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
//	void valueChanged( QLineF pValue );

private:
	QVector<QLineF>		mValues;
};

#endif // LINEPIN_H
