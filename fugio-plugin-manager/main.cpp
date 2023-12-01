#include <QCoreApplication>
#include <QLocale>
#include <QTranslator>
#include <QCommandLineParser>
#include <QDir>
#include <QUrl>
#include <QFileInfo>
#include <QJsonDocument>

#include "QtCore/qglobal.h"
#include "globalprivate.h"
#include "pluginmanager.h"

#include "../libs/zip/zip.h"

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	QCoreApplication::setApplicationName( "fugio-plugin-manager" );
	QCoreApplication::setApplicationVersion( "1.0.0" );

	//-------------------------------------------------------------------------
	// Variables

	QDir FugioDirectory;

	//-------------------------------------------------------------------------
	// Set up the arguments for the command line parser.

	QCommandLineParser  CommandLine;

	CommandLine.setApplicationDescription("Command line control of Fugio plugins");
	CommandLine.addVersionOption();

	// Add our own help option (instead of addHelpOption()) so we can offer help for each command
	QCommandLineOption helpOption(QStringList() << "h" << "help",
								   QCoreApplication::translate("main", "Help"));
	CommandLine.addOption(helpOption);

	// Verbose
	QCommandLineOption verboseOption( "V", QCoreApplication::translate("main", "Verbose reporting"));

	CommandLine.addOption(verboseOption);

	// The command we have to execute

	CommandLine.addPositionalArgument( "command", QCoreApplication::translate( "main", "The command to execute." ), "<install|info>" );

	// The directory where Fugio is installed

	QCommandLineOption FugioDirectoryOption(QStringList() << "f" << "fugio-directory",
											 QCoreApplication::translate( "main", "Location of Fugio install <directory>." ),
											 QCoreApplication::translate( "main", "directory"));

	CommandLine.addOption( FugioDirectoryOption );

	//-------------------------------------------------------------------------
	// Process the command line

	CommandLine.process( QCoreApplication::arguments() );

	// Did the user request help?

	bool help = CommandLine.isSet(helpOption);
	const bool verbose = CommandLine.isSet( verboseOption );

	// look for the command option. if we find one, parse the arguments again with the options for that command

	const QStringList args = CommandLine.positionalArguments();
	const QString command = args.isEmpty() ? QString() : args.first();

	if( command == "install" )
	{
		CommandLine.clearPositionalArguments();
		CommandLine.addPositionalArgument("install", "install a new package", "install [url]");
		CommandLine.process(a);
	}
	else if( command == "info" )
	{
		CommandLine.clearPositionalArguments();
		CommandLine.addPositionalArgument( "info", "show info from a package", "info [url]");
		CommandLine.process(a);
	}
	else
	{
		// we haven't found a known command, set the help flag

		help = true;
	}

	// Show the user the help text. This will exit the app.

	if( help )
	{
		CommandLine.showHelp();
	}

	//-------------------------------------------------------------------------

	if( verbose )
	{
		printf("%s %s\n", qPrintable(QCoreApplication::applicationName()),
			   qPrintable(QCoreApplication::applicationVersion()));
	}

	//-------------------------------------------------------------------------
	// Process the Fugio directory command line option

	if( CommandLine.isSet( FugioDirectoryOption ) )
	{
		FugioDirectory = QDir( CommandLine.value( FugioDirectoryOption ) );

		if( !FugioDirectory.exists() )
		{
			fputs( "Fugio Directory isn't valid\n", stderr );

			return( 1 );
		}
	}

	if( verbose )
	{
		printf( "Fugio directory: %s\n", qPrintable( FugioDirectory.absolutePath() ) );
	}

	//-------------------------------------------------------------------------

    GlobalPrivate   *G = qobject_cast<GlobalPrivate *>( fugio::fugio()->qobject() );

    PluginManager   &PM = G->pluginManager();

	if( command == "info" )
	{
		QString UrlTxt = CommandLine.positionalArguments().at( 1 );

		if( verbose )
		{
			printf( "Performing 'info' on '%s'...\n", qPrintable( UrlTxt ) );
		}

		QUrl	Url;

		QFileInfo FileInfo( UrlTxt );

		if( FileInfo.exists() && FileInfo.isFile() )
		{
			Url = QUrl::fromLocalFile( UrlTxt );
		}
		else
		{
			Url = QUrl( UrlTxt );
		}

		// TODO: if it's a remote file, copy to a plugin cache location

		struct zip_t *zfh = zip_open( qPrintable( UrlTxt ), 0, 'r' );

		if( !zfh )
		{
			fputs( "Couldn't open zip file", stderr );

			exit( 1 );
		}

		const int PluginEntries = zip_total_entries( zfh );

		if( verbose )
		{
			printf( "Files in plugin zip archive: %d\n", PluginEntries );
		}

		if( zip_entry_open( zfh, "manifest.json" ) )
		{
			fputs( "error finding manifest.json in archive", stderr );

			exit( 1 );
		}

		const int ManifestSize = zip_entry_size( zfh );

		if( verbose )
		{
			printf( "manifest.json size is %d bytes\n", ManifestSize );
		}

		QByteArray		ManifestData;

		ManifestData.resize( ManifestSize );

		const int ManifestRead = zip_entry_noallocread( zfh, ManifestData.data(), ManifestData.size() );

		if( ManifestRead != ManifestSize )
		{
			fprintf( stderr, "Manifest read error: got %d - wanted %d bytes\n", ManifestRead, ManifestSize );

			exit( 1 );
		}

		if( verbose )
		{
			fwrite( ManifestData.constData(), ManifestRead, 1, stdout );
		}

		zip_entry_close( zfh );

		for( int i = 0 ; i < PluginEntries ; i++ )
		{
			if( zip_entry_openbyindex( zfh, i ) != 0 )
			{
				exit( 1 );
			}

			QFileInfo	FI( zip_entry_name( zfh ) );

			qDebug() << FI.filePath() << FI.isDir() << FI.isFile();

			if( zip_entry_isdir( zfh ) )
			{
				printf( "%s (dir)\n", zip_entry_name( zfh ) );
			}
			else
			{
				printf( "%s (file)\n", zip_entry_name( zfh ) );
			}

			zip_entry_close( zfh );
		}

		zip_close( zfh );

		// we have the raw manifest file in memory - parse it

		QJsonParseError	JsonError;

		QJsonDocument	JSON = QJsonDocument::fromJson( ManifestData, &JsonError );

		if( JSON.isNull() )
		{
			fprintf( stderr, "%s\n", qPrintable( JsonError.errorString() ) );

			exit( 1 );
		}

		const QString authorName = JSON[ "authorName" ].toString();

		qDebug() << authorName;
	}

    return( 0 );
}
