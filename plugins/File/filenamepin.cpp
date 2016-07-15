#include "filenamepin.h"

#include <QSettings>
#include <QFileInfo>
#include <QDir>

FilenamePin::FilenamePin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin )
{

}

void FilenamePin::loadSettings( QSettings &pSettings )
{
	QString		FileName = pSettings.value( "filename", mFilename ).toString();

	if( !FileName.isEmpty() )
	{
		QFileInfo	FileInfo( pSettings.fileName() );
		QDir		FileDir( FileInfo.absolutePath() );
		QFileInfo	DestInfo( FileDir.absoluteFilePath( FileName ) );

		mFilename = DestInfo.exists() ? DestInfo.canonicalFilePath() : FileName;
	}
}

void FilenamePin::saveSettings( QSettings &pSettings ) const
{
	if( !mFilename.isEmpty() )
	{
		QFileInfo	FileInfo( pSettings.fileName() );
		QDir		FileDir( FileInfo.absolutePath() );
		QString		FileName = FileDir.relativeFilePath( mFilename );

		pSettings.setValue( "filename", FileName );
	}
}
