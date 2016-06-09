#ifndef STRINGLISTPIN_H
#define STRINGLISTPIN_H

#include <QObject>

#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>

#include <fugio/core/variant_interface.h>
#include <fugio/core/list_interface.h>

#include <fugio/pincontrolbase.h>

#include <fugio/serialise_interface.h>

class StringListPin : public fugio::PinControlBase, public fugio::VariantInterface, public fugio::SerialiseInterface, public fugio::ListInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::VariantInterface fugio::SerialiseInterface fugio::ListInterface )
	Q_PROPERTY( QStringList value READ value WRITE setValue NOTIFY valueChanged )

public:
	Q_INVOKABLE explicit StringListPin( QSharedPointer<fugio::PinInterface> pPin );

	virtual ~StringListPin( void ) {}

	//-------------------------------------------------------------------------
	// Q_PROPERTY

	Q_INVOKABLE QStringList value( void ) const
	{
		return( mValue );
	}

	Q_INVOKABLE void setValue( const QStringList &pValue )
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
		return( QString( "List of %1 strings" ).arg( mValue.size() ) );
	}

	virtual QString description( void ) const Q_DECL_OVERRIDE
	{
		return( "StringList" );
	}

	//-------------------------------------------------------------------------
	// fugio::VariantInterface

	virtual void setVariant( const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		setValue( pValue.toStringList() );
	}

	virtual QVariant variant( void ) const Q_DECL_OVERRIDE
	{
		return( mValue );
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
		QStringList		NewVal;

		pDataStream >> NewVal;

		setValue( NewVal );
	}

	//-------------------------------------------------------------------------
	// fugio::ListInterface

	virtual int listSize() const Q_DECL_OVERRIDE
	{
		return( mValue.size() );
	}

	virtual QUuid listPinControl( void ) const Q_DECL_OVERRIDE;

	virtual QVariant listIndex( int pIndex ) const Q_DECL_OVERRIDE
	{
		return( mValue.at( pIndex ) );
	}

signals:
	void valueChanged( const QStringList &pValue );

private:
	QStringList		mValue;
};

#endif // STRINGLISTPIN_H
