#include "stringpin.h"
#include <QSettings>

#include <fugio/core/uuid.h>

StringPin::StringPin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin ), VariantHelper( QMetaType::QString, PID_STRING )
{
}


QString StringPin::toString() const
{
	QStringList		L;

	for( QString v : mValues )
	{
		L << v;
	}

	return( L.join( ',' ) );
}

void StringPin::loadSettings( QSettings &pSettings )
{
	fugio::PinControlBase::loadSettings( pSettings );

	if( pSettings.childGroups().contains( "values" ) )
	{
		int	Count = pSettings.beginReadArray( "values" );

		mValues.resize( Count );

		for( int i = 0 ; i < mValues.size() ; i++ )
		{
			pSettings.setArrayIndex( i );

			mValues[ i ] = pSettings.value( "i", mValues[ i ] ).toString();
		}

		pSettings.endArray();
	}
}

void StringPin::saveSettings( QSettings &pSettings ) const
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
