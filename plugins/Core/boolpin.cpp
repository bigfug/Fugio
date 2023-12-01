#include "boolpin.h"

#include <QSettings>

#include <fugio/core/uuid.h>

BoolPin::BoolPin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin ), VariantHelper( QMetaType::Bool, PID_BOOL )
{

}

QString BoolPin::toString() const
{
	QStringList		L;

	for( bool v : mValues )
	{
		L << ( v ? "1" : "0" );
	}

	return( L.join( ',' ) );
}

void BoolPin::loadSettings( QSettings &pSettings )
{
	fugio::PinControlBase::loadSettings( pSettings );

	if( pSettings.childGroups().contains( "values" ) )
	{
		int	Count = pSettings.beginReadArray( "values" );

		mValues.resize( Count );

		for( int i = 0 ; i < mValues.size() ; i++ )
		{
			pSettings.setArrayIndex( i );

			mValues[ i ] = pSettings.value( "i", mValues[ i ] ).toBool();
		}

		pSettings.endArray();
	}
}

void BoolPin::saveSettings( QSettings &pSettings ) const
{
	fugio::PinControlBase::saveSettings( pSettings );

	pSettings.beginWriteArray( "values", mValues.size() );

	for( int i = 0 ; i < mValues.size() ; i++ )
	{
		pSettings.setArrayIndex( i );

		pSettings.setValue( "i", mValues.at( i ) );
	}

	pSettings.endArray();
}
