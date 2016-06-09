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
	Q_PROPERTY( double mValue READ value WRITE setValue NOTIFY valueChanged )

public:
	Q_INVOKABLE explicit FloatPin( QSharedPointer<fugio::PinInterface> pPin );

	virtual ~FloatPin( void );

	//-------------------------------------------------------------------------
	// Q_PROPERTY

	Q_INVOKABLE double value( void ) const
	{
		return( mValue );
	}

	Q_INVOKABLE void setValue( double pValue )
	{
		if( pValue != mValue )
		{
			mValue = pValue;

			emit valueChanged( pValue );
		}
	}

	//-------------------------------------------------------------------------
	// fugio::PinControlInterface

	virtual QString toString( void ) const Q_DECL_OVERRIDE
	{
		return( QString( "%1" ).arg( mValue ) );
	}

	virtual QString description( void ) const Q_DECL_OVERRIDE
	{
		return( "Float" );
	}

	//-------------------------------------------------------------------------
	// fugio::VariantInterface

	virtual void setVariant( const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		setValue( pValue.toDouble() );
	}

	virtual QVariant variant( void ) const Q_DECL_OVERRIDE
	{
		return( QVariant::fromValue<double>( value() ) );
	}

	virtual void setFromBaseVariant( const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		setVariant( pValue );
	}

	virtual QVariant baseVariant( void ) const Q_DECL_OVERRIDE
	{
		return( variant() );
	}

	//-------------------------------------------------------------------------
	// fugio::SerialiseInterface

	virtual void serialise( QDataStream &pDataStream ) Q_DECL_OVERRIDE
	{
		pDataStream << mValue;
	}

	virtual void deserialise( QDataStream &pDataStream ) Q_DECL_OVERRIDE
	{
		double		Value;

		pDataStream >> Value;

		setValue( Value );
	}

signals:
	void valueChanged( double pValue );

private:
	double		mValue;
};

#endif // FLOATPIN_H
