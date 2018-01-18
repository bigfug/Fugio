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
	QString		FileName = pSettings.value( "filename", mValues.first() ).toString();

	if( !FileName.isEmpty() )
	{
		QFileInfo	FileInfo( pSettings.fileName() );
		QDir		FileDir( FileInfo.absolutePath() );
		QFileInfo	DestInfo( FileDir.absoluteFilePath( FileName ) );

		mValues[ 0 ] = DestInfo.exists() ? DestInfo.canonicalFilePath() : FileName;
	}
}

void FilenamePin::saveSettings( QSettings &pSettings ) const
{
	if( !mValues.first().isEmpty() )
	{
		QFileInfo	FileInfo( pSettings.fileName() );
		QDir		FileDir( FileInfo.absolutePath() );
		QString		FileName = FileDir.relativeFilePath( mValues.first() );

		pSettings.setValue( "filename", FileName );
	}
}
