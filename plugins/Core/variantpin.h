#ifndef VARIANTPIN_H
#define VARIANTPIN_H

#include <QObject>

#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>

#include <fugio/core/variant_interface.h>

#include <fugio/pincontrolbase.h>

#include <fugio/serialise_interface.h>

class VariantPin : public fugio::PinControlBase, public fugio::VariantInterface, public fugio::SerialiseInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::VariantInterface fugio::SerialiseInterface )
//	Q_PROPERTY( QVariant value READ value WRITE setValue NOTIFY valueChanged )

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "" )
	Q_CLASSINFO( "URL", WIKI_PIN_URL( "Variant" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit VariantPin( QSharedPointer<fugio::PinInterface> pPin );

	virtual ~VariantPin( void ) {}

	//-------------------------------------------------------------------------
	// Q_PROPERTY

//	Q_INVOKABLE QVariant value( void ) const
//	{
//		return( mValue );
//	}

//	Q_INVOKABLE void setValue( QVariant pValue )
//	{
//		if( pValue != mValue )
//		{
//			mValue = pValue;

//			emit valueChanged( pValue );
//		}
//	}

	//-------------------------------------------------------------------------
	// fugio::PinControlInterface

	virtual QString toString( void ) const Q_DECL_OVERRIDE;

	virtual QString description( void ) const Q_DECL_OVERRIDE
	{
		return( "Variant" );
	}

	//-------------------------------------------------------------------------
	// fugio::VariantInterface

	virtual void setVariant( const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		setVariant( 0, pValue );
	}

	virtual void setVariant( int pIndex, const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		mValues[ pIndex ] = pValue;
	}

	virtual QVariant variant( int pIndex = 0 ) const Q_DECL_OVERRIDE
	{
		return( mValues[ pIndex ] );
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
		return( mType );
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

	virtual void setVariantType( QMetaType::Type pType ) Q_DECL_OVERRIDE
	{
		mType = pType;
	}

	//-------------------------------------------------------------------------
	// fugio::SerialiseInterface

	virtual void serialise( QDataStream &pDataStream ) const Q_DECL_OVERRIDE
	{
//		pDataStream << mValue;
	}

	virtual void deserialise( QDataStream &pDataStream ) Q_DECL_OVERRIDE
	{
//		QVariant		NewVal;

//		pDataStream >> NewVal;

//		setValue( NewVal );
	}

signals:
//	void valueChanged( QVariant pValue );

private:
	QVector<QVariant>		mValues;
	QMetaType::Type			mType;
};


#endif // VARIANTPIN_H
