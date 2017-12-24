#include "filenamepin.h"

#include <QSettings>
#include <QFileInfo>
#include <QDir>

FilenamePin::FilenamePin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin ), mFilenames( 1 )
{

}

void FilenamePin::loadSettings( QSettings &pSettings )
{
	QString		FileName = pSettings.value( "filename", mFilenames.first() ).toString();

	if( !FileName.isEmpty() )
	{
		QFileInfo	FileInfo( pSettings.fileName() );
		QDir		FileDir( FileInfo.absolutePath() );
		QFileInfo	DestInfo( FileDir.absoluteFilePath( FileName ) );

		mFilenames[ 0 ] = DestInfo.exists() ? DestInfo.canonicalFilePath() : FileName;
	}
}

void FilenamePin::saveSettings( QSettings &pSettings ) const
{
	if( !mFilenames.first().isEmpty() )
	{
		QFileInfo	FileInfo( pSettings.fileName() );
		QDir		FileDir( FileInfo.absolutePath() );
		QString		FileName = FileDir.relativeFilePath( mFilenames.first() );

		pSettings.setValue( "filename", FileName );
	}
}

void FilenamePin::setVariant( const QVariant &pValue )
{
	setVariant( 0, pValue );
}

void FilenamePin::setVariant( int pIndex, const QVariant &pValue )
{
	mFilenames[ pIndex ] = pValue.toString();
}

QVariant FilenamePin::variant( int pIndex ) const
{
	return( mFilenames[ pIndex ] );
}
