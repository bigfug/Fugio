#ifndef MATRIXPIN_H
#define MATRIXPIN_H

#include <QObject>

#include <Eigen/Dense>

#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>

#include <fugio/core/variant_helper.h>

#include <fugio/pincontrolbase.h>

class MatrixPin : public fugio::PinControlBase, public fugio::VariantHelper<Eigen::MatrixXd>
{
	Q_OBJECT
	Q_INTERFACES( fugio::VariantInterface )

public:
	Q_INVOKABLE explicit MatrixPin( QSharedPointer<fugio::PinInterface> pPin );

	virtual ~MatrixPin( void ) Q_DECL_OVERRIDE {}

	//-------------------------------------------------------------------------
	// fugio::PinControlInterface

	virtual QString toString( void ) const Q_DECL_OVERRIDE;

	virtual QString description( void ) const Q_DECL_OVERRIDE
	{
		return( "Matrix" );
	}

	virtual void loadSettings( QSettings &pSettings ) Q_DECL_OVERRIDE;

	virtual void saveSettings( QSettings &pSettings ) const Q_DECL_OVERRIDE;
};

Q_DECLARE_METATYPE( Eigen::MatrixXd )

#endif // MATRIXPIN_H
