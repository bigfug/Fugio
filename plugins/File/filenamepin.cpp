#include "filenamepin.h"

#include <QSettings>
#include <QFileInfo>
#include <QDir>

FilenamePin::FilenamePin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin ), VariantHelper( QMetaType::QString, PID_STRING )
{

}

void FilenamePin::loadSettings( QSettings &pSettings )
{
	QString		FileName = ( mPin->direction() == PIN_INPUT ? mPin->value().toString() : mValues.first() );

	FileName = pSettings.value( "filename", FileName ).toString();

	if( !FileName.isEmpty() )
	{
		QFileInfo	FileInfo( pSettings.fileName() );
		QDir		FileDir( FileInfo.absolutePath() );
		QFileInfo	DestInfo( FileDir.absoluteFilePath( FileName ) );

		FileName = DestInfo.exists() ? DestInfo.canonicalFilePath() : FileName;

		if( mPin->direction() == PIN_INPUT )
		{
			mPin->setValue( FileName );
		}
		else
		{
			mValues[ 0 ] = FileName;
		}
	}
}

void FilenamePin::saveSettings( QSettings &pSettings ) const
{
	QString		FileName = ( mPin->direction() == PIN_INPUT ? mPin->value().toString() : mValues.first() );

	if( !FileName.isEmpty() )
	{
		QFileInfo	FileInfo( pSettings.fileName() );
		QDir		FileDir( FileInfo.absolutePath() );

		FileName = FileDir.relativeFilePath( FileName );

		pSettings.setValue( "filename", FileName );
	}
}
