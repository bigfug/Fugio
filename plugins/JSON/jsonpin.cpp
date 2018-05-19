#include "jsonpin.h"

#include <QSettings>

#include <fugio/json/uuid.h>

JsonPin::JsonPin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin ), VariantHelper( QMetaType::QJsonDocument, PID_JSON )
{

}

QString JsonPin::toString() const
{
	QStringList		L;

	for( QJsonDocument v : mValues )
	{
		L << v.toJson( QJsonDocument::Indented );
	}

	return( L.join( ',' ) );
}

void JsonPin::loadSettings( QSettings &pSettings )
{
	fugio::PinControlBase::loadSettings( pSettings );

	if( pSettings.childGroups().contains( "values" ) )
	{
		int	Count = pSettings.beginReadArray( "values" );

		mValues.resize( Count );

		for( int i = 0 ; i < mValues.size() ; i++ )
		{
			pSettings.setArrayIndex( i );

			mValues[ i ] = QJsonDocument::fromJson( pSettings.value( "i", mValues[ i ] ).toString().toLatin1() );
		}

		pSettings.endArray();
	}
}

void JsonPin::saveSettings( QSettings &pSettings ) const
{
	fugio::PinControlBase::saveSettings( pSettings );

	pSettings.beginWriteArray( "values", mValues.size() );

	for( int i = 0 ; i < mValues.size() ; i++ )
	{
		pSettings.setArrayIndex( i );

		pSettings.setValue( "i", mValues.at( i ).toJson( QJsonDocument::Compact ) );
	}

	pSettings.endArray();
}
