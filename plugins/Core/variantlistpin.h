#ifndef VARIANTLISTPIN_H
#define VARIANTLISTPIN_H

#include <QObject>

#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>
#include <fugio/pincontrolbase.h>
#include <fugio/serialise_interface.h>

#include <fugio/core/variant_interface.h>
#include <fugio/core/list_interface.h>

class VariantListPin : public fugio::PinControlBase, public fugio::VariantInterface, public fugio::SerialiseInterface, public fugio::ListInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::VariantInterface fugio::SerialiseInterface fugio::ListInterface )
	//Q_PROPERTY( QVariantList value READ value WRITE setValue NOTIFY valueChanged )

public:
	Q_INVOKABLE explicit VariantListPin( QSharedPointer<fugio::PinInterface> pPin );

	virtual ~VariantListPin( void ) {}

	//-------------------------------------------------------------------------
	// Q_PROPERTY

	Q_INVOKABLE QVariant value( void ) const
	{
		return( mValue );
	}

	Q_INVOKABLE void setValue( QVariantList pValue )
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
		return( QString( "Size: %1" ).arg( mValue.size() ) );
	}

	virtual QString description( void ) const Q_DECL_OVERRIDE
	{
		return( "Variant List" );
	}

	//-------------------------------------------------------------------------
	// fugio::VariantInterface

	virtual void setVariant( const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		setValue( pValue.toList() );
	}

	virtual QVariant variant( void ) const Q_DECL_OVERRIDE
	{
		return( mValue );
	}

	virtual void setFromBaseVariant( const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		setVariant( pValue.toList() );
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
		QVariantList		NewVal;

		pDataStream >> NewVal;

		setValue( NewVal );
	}

	//-------------------------------------------------------------------------
	// fugio::ListInterface

	virtual int listSize() const Q_DECL_OVERRIDE;
	virtual QUuid listPinControl() const Q_DECL_OVERRIDE;
	virtual QVariant listIndex(int pIndex) const Q_DECL_OVERRIDE;
	virtual void listSetIndex(int pIndex, const QVariant &pValue) Q_DECL_OVERRIDE;
	virtual void listSetSize(int pSize) Q_DECL_OVERRIDE;
	virtual void listClear() Q_DECL_OVERRIDE;
	virtual void listAppend(const QVariant &pValue) Q_DECL_OVERRIDE;
	virtual bool listIsEmpty() const Q_DECL_OVERRIDE;

signals:
	void valueChanged( QVariantList pValue );

private:
	QVariantList		mValue;
};

#endif // VARIANTLISTPIN_H
