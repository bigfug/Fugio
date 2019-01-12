#include "matrixpin.h"

#include <QSettings>

#include <fugio/math/uuid.h>

MatrixPin::MatrixPin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin ), VariantHelper( QMetaType::Type( QMetaType::type( "Eigen::MatrixXd" ) ), PID_MATRIX )
{
}

QString MatrixPin::toString( void ) const
{
	return( QString() );
}

void MatrixPin::loadSettings( QSettings &pSettings )
{
	fugio::PinControlBase::loadSettings( pSettings );


}

void MatrixPin::saveSettings( QSettings &pSettings ) const
{
	fugio::PinControlBase::saveSettings( pSettings );

	pSettings.beginWriteArray( "values", mValues.size() );

	for( int i = 0 ; i < mValues.size() ; i++ )
	{
		pSettings.setArrayIndex( i );

		const Eigen::Index		rows = mValues.at( i ).rows();
		const Eigen::Index		cols = mValues.at( i ).cols();

		pSettings.setValue( "rows", rows );
		pSettings.setValue( "cols", cols );

		for( int r = 0 ; r < rows ; r++ )
		{
			for( int c = 0 ; c < cols ; c++ )
			{
			}
		}
	}

	pSettings.endArray();
}
