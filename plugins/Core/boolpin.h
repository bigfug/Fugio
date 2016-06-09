#ifndef BOOLPIN_H
#define BOOLPIN_H

#include <QObject>

#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>

#include <fugio/core/variant_interface.h>

#include <fugio/pincontrolbase.h>

#include <fugio/serialise_interface.h>

class BoolPin : public fugio::PinControlBase, public fugio::VariantInterface, public fugio::SerialiseInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::VariantInterface fugio::SerialiseInterface )
	Q_PROPERTY( bool mValue READ value WRITE setValue NOTIFY valueChanged )

public:
	Q_INVOKABLE explicit BoolPin( QSharedPointer<fugio::PinInterface> pPin );

	virtual ~BoolPin( void ) {}

	//-------------------------------------------------------------------------
	// Q_PROPERTY

	Q_INVOKABLE bool value( void ) const
	{
		return( mValue );
	}

	Q_INVOKABLE void setValue( bool pValue )
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
		return( "Bool" );
	}

	//-------------------------------------------------------------------------
	// fugio::VariantInterface

	virtual void setVariant( const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		setValue( pValue.toBool() );
	}

	virtual QVariant variant( void ) const Q_DECL_OVERRIDE
	{
		return( QVariant::fromValue<bool>( mValue ) );
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
		bool		NewVal;

		pDataStream >> NewVal;

		setValue( NewVal );
	}

signals:
	void valueChanged( bool pValue );

private:
	bool		mValue;
};

#endif // BOOLPIN_H
