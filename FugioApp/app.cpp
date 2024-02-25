#include <QOpenGLContext>
#include <QDateTime>

#include <QDebug>
#include <QNetworkAccessManager>
#include <QStandardPaths>

#include "app.h"
#include "mainwindow.h"

#include <fugio/utils.h>

QList<App::LogMessage>		 App::mLogMessages;
QString						 App::mLogFileName;
QDir						 App::mDataDirectory;
QString						 SettingsHelper::mSettingsFileName;
QSettings::Format			 SettingsHelper::mSettingsFormat;

App::App( int &argc, char **argv ) :
	QApplication( argc, argv ), mMainWindow( 0 ), mAppRestart( false )
{
	SettingsHelper		Settings;

	QDir			UsrDir = App::dataDirectory();

	mUserSnippetsDirectory = Settings.value( "snippets-directory", UsrDir.absoluteFilePath( "snippets" ) ).toString();

	if( !QDir( mUserSnippetsDirectory ).exists() )
	{
		if( !QDir( UsrDir.absoluteFilePath( "snippets" ) ).exists() )
		{
			UsrDir.mkdir( "snippets" );
		}

		mUserSnippetsDirectory = UsrDir.absoluteFilePath( "snippets" );
	}
}

App::~App( void )
{
}

void App::setMainWindow( MainWindow *pMainWindow )
{
	mMainWindow = pMainWindow;
}

MainWindow *App::mainWindow( void )
{
	return( mMainWindow );
}

void App::incrementStatistic( const QString &pName )
{
	SettingsHelper	Settings;

	Settings.beginGroup( "statistics" );

	int				Value =  Settings.value( pName, 0 ).toInt() + 1;

	Settings.setValue( pName, Value );

	Settings.endGroup();

	recordData( pName, QString::number( Value ) );
}

void App::recordData( const QString &pName, const QString &pValue )
{
	SettingsHelper		Settings;

	// Disabled this because it currently crashes on Qt 5.8 with known bug
	if( true ) //!Settings.value( "data-collection-permission", false ).toBool() )
	{
		return;
	}

	QUuid			Instance = Settings.value( "instance", QUuid() ).toUuid();

	if( Instance.isNull() )
	{
		Instance = QUuid::createUuid();

		Settings.setValue( "instance", Instance );
	}
}

QString App::userSnippetsDirectory() const
{
	return( mUserSnippetsDirectory );
}

void App::setUserSnippetsDirectory( const QString &pDirectory )
{
	if( mUserSnippetsDirectory != pDirectory )
	{
		SettingsHelper().setValue( "snippets-directory", pDirectory );

		mUserSnippetsDirectory = pDirectory;

		emit userSnippetsDirectoryChanged( mUserSnippetsDirectory );
	}
}
