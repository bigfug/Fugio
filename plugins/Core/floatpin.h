#ifndef FLOATPIN_H
#define FLOATPIN_H

#include <QObject>

#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>

#include <fugio/core/variant_interface.h>

#include <fugio/serialise_interface.h>

#include <fugio/pincontrolbase.h>

class FloatPin : public fugio::PinControlBase, public fugio::VariantInterface, public fugio::SerialiseInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::VariantInterface fugio::SerialiseInterface )
//	Q_PROPERTY( double mValue READ value WRITE setValue NOTIFY valueChanged )

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "" )
	Q_CLASSINFO( "URL", WIKI_PIN_URL( "Float" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit FloatPin( QSharedPointer<fugio::PinInterface> pPin );

	virtual ~FloatPin( void ) {}

	//-------------------------------------------------------------------------
	// Q_PROPERTY

//	Q_INVOKABLE double value( void ) const
//	{
//		return( mValue );
//	}

//	Q_INVOKABLE void setValue( double pValue )
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
		return( "Float" );
	}

	//-------------------------------------------------------------------------
	// fugio::VariantInterface

	virtual void setVariant( const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		setVariant( 0, pValue );
	}

	virtual void setVariant( int pIndex, const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		mValues[ pIndex ] = pValue.toDouble();
	}

	virtual QVariant variant( int pIndex = 0 ) const Q_DECL_OVERRIDE
	{
		return( QVariant::fromValue<double>( mValues[ pIndex ] ) );
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
		return( QMetaType::Double );
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

	//-------------------------------------------------------------------------
	// fugio::SerialiseInterface

	virtual void serialise( QDataStream &pDataStream ) const Q_DECL_OVERRIDE
	{
		if( mValues.size() == 1 )
		{
			pDataStream << mValues.first();
		}
		else
		{
			pDataStream << mValues;
		}
	}

	virtual void deserialise( QDataStream &pDataStream ) Q_DECL_OVERRIDE
	{
		double				V;

		pDataStream.startTransaction();

		pDataStream >> V;

		if( pDataStream.commitTransaction() )
		{
			mValues.resize( 1 );

			setVariant( 0, V );

			return;
		}

		pDataStream.rollbackTransaction();

		QVector<double>	L;

		pDataStream >> L;

		mValues = L;
	}

signals:
//	void valueChanged( double pValue );

private:
	QVector<double>		mValues;
};

#endif // FLOATPIN_H
