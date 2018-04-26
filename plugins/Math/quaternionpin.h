#ifndef QUATERNIONPIN_H
#define QUATERNIONPIN_H

#include <QObject>

#include <QQuaternion>

#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>

#include <fugio/core/variant_helper.h>

#include <fugio/pincontrolbase.h>

class QuaternionPin : public fugio::PinControlBase, public fugio::VariantHelper<QQuaternion>
{
	Q_OBJECT
	Q_INTERFACES( fugio::VariantInterface )

public:
	Q_INVOKABLE explicit QuaternionPin( QSharedPointer<fugio::PinInterface> pPin );

	virtual ~QuaternionPin( void );

	//-------------------------------------------------------------------------
	// fugio::PinControlInterface

	virtual QString toString( void ) const Q_DECL_OVERRIDE;

	virtual QString description( void ) const Q_DECL_OVERRIDE
	{
		return( "Quaternion" );
	}

	virtual void loadSettings( QSettings &pSettings ) Q_DECL_OVERRIDE;

	virtual void saveSettings( QSettings &pSettings ) const Q_DECL_OVERRIDE;

};

#endif // QUATERNIONPIN_H
