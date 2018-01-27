#ifndef MATRIX4PIN_H
#define MATRIX4PIN_H

#include <QObject>

#include <QMatrix4x4>

#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>

#include <fugio/core/variant_helper.h>

#include <fugio/pincontrolbase.h>

class Matrix4Pin : public fugio::PinControlBase, public fugio::VariantHelper<QMatrix4x4>
{
	Q_OBJECT
	Q_INTERFACES( fugio::VariantInterface )

public:
	Q_INVOKABLE explicit Matrix4Pin( QSharedPointer<fugio::PinInterface> pPin );

	virtual ~Matrix4Pin( void ) {}

	//-------------------------------------------------------------------------
	// fugio::PinControlInterface

	virtual QString toString( void ) const Q_DECL_OVERRIDE;

	virtual QString description( void ) const Q_DECL_OVERRIDE
	{
		return( "Matrix4" );
	}

	virtual void loadSettings( QSettings &pSettings ) Q_DECL_OVERRIDE;

	virtual void saveSettings( QSettings &pSettings ) const Q_DECL_OVERRIDE;
};

#endif // MATRIX4PIN_H
