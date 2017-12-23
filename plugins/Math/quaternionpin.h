#ifndef QUATERNIONPIN_H
#define QUATERNIONPIN_H

#include <QObject>

#include <QQuaternion>

#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>

#include <fugio/core/list_interface.h>
#include <fugio/core/variant_interface.h>

#include <fugio/pincontrolbase.h>

class QuaternionPin : public fugio::PinControlBase, public fugio::VariantInterface, public fugio::ListInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::VariantInterface fugio::ListInterface )

public:
	Q_INVOKABLE explicit QuaternionPin( QSharedPointer<fugio::PinInterface> pPin );

	virtual ~QuaternionPin( void );

	//-------------------------------------------------------------------------
	// fugio::PinControlInterface

	virtual QString toString( void ) const Q_DECL_OVERRIDE;

	virtual QString description( void ) const Q_DECL_OVERRIDE
	{
		return( "Matrix4" );
	}

	virtual void loadSettings( QSettings &pSettings ) Q_DECL_OVERRIDE;

	virtual void saveSettings( QSettings &pSettings ) const Q_DECL_OVERRIDE;

	//-------------------------------------------------------------------------
	// fugio::VariantInterface

	virtual void setVariant( const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		mValue = pValue.value<QQuaternion>();
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
	// ListInterface interface
public:
	virtual int listSize() const Q_DECL_OVERRIDE;
	virtual QUuid listPinControl() const Q_DECL_OVERRIDE;
	virtual QVariant listIndex(int pIndex) const Q_DECL_OVERRIDE;
	virtual void listSetIndex(int pIndex, const QVariant &pValue) Q_DECL_OVERRIDE;
	virtual void listSetSize(int pSize) Q_DECL_OVERRIDE;
	virtual void listClear() Q_DECL_OVERRIDE;
	virtual void listAppend(const QVariant &pValue) Q_DECL_OVERRIDE;
	virtual bool listIsEmpty() const Q_DECL_OVERRIDE;

	virtual QMetaType::Type listType( void ) const Q_DECL_OVERRIDE
	{
		return( QMetaType::QQuaternion );
	}

private:
	QQuaternion			mValue;
};

#endif // QUATERNIONPIN_H
