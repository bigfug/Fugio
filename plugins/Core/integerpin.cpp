#include "integerpin.h"

#include <QSettings>

#include <fugio/core/uuid.h>

IntegerPin::IntegerPin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin ), VariantHelper( QMetaType::Int, PID_INTEGER )
{
}

QString IntegerPin::toString() const
{
	QStringList		L;

	for( int v : mValues )
	{
		L << QString::number( v );
	}

	return( L.join( ',' ) );
}

void IntegerPin::loadSettings( QSettings &pSettings )
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

void IntegerPin::saveSettings( QSettings &pSettings ) const
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
