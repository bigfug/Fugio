#include <QCoreApplication>
#include <QLocale>
#include <QTranslator>
#include <QCommandLineParser>
#include <QDir>
#include <QUrl>
#include <QFileInfo>
#include <QJsonDocument>
#include <QSettings>
#include <QStandardPaths>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QTemporaryFile>

#include "QtCore/qglobal.h"
#include "pluginmanager.h"

#include "../libs/zip/zip.h"

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

    QNetworkAccessManager   Manager;

    //-------------------------------------------------------------------------

    QCoreApplication::setApplicationName( "fugio-plugin-manager" );
	QCoreApplication::setApplicationVersion( "1.0.0" );

	//-------------------------------------------------------------------------
	// Variables

    QFileInfo PluginConfigFileInfo( "plugin-config.ini" );

    QSettings FugioGlobalSettings( QSettings::SystemScope, "Fugio", "Fugio" );

    PluginManager       PM;

    qDebug() << PM.pluginConfigFilename();

    QSettings FugioPluginSettings( PM.pluginConfigFilename(), QSettings::IniFormat );

    qDebug() << FugioPluginSettings.fileName();

    FugioPluginSettings.setValue( "test", "test" );

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

    QCommandLineOption PluginConfigOption(QStringList() << "c" << "plugin-config",
                                             QCoreApplication::translate( "main", "Filename of plugin config." ),
                                             QCoreApplication::translate( "main", "config"));

    CommandLine.addOption( PluginConfigOption );

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

    if( CommandLine.isSet( PluginConfigOption ) )
	{
        PluginConfigFileInfo = QFileInfo( CommandLine.value( PluginConfigOption ) );
	}

    QSettings   PluginSettings( PluginConfigFileInfo.absoluteFilePath(), QSettings::IniFormat );

    if( !PluginSettings.isWritable() )
    {
        fprintf( stderr, "Plugin config is not writable - %s\n", qPrintable( PluginConfigFileInfo.absoluteFilePath() ) );

        return( 1 );
    }

    if( verbose )
	{
        printf( "Plugin config file: %s\n", qPrintable( PluginConfigFileInfo.absoluteFilePath() ) );
	}

    PluginSettings.setValue( "test", "test" );

	//-------------------------------------------------------------------------

//    GlobalPrivate   *G = qobject_cast<GlobalPrivate *>( fugio::fugio()->qobject() );

//    PluginManager   &PM = G->pluginManager();

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

        qDebug() << Url;
        qDebug() << QSslSocket::supportsSsl();
        qDebug() << QSslSocket::sslLibraryVersionString();

        QNetworkReply       *NetRep = Manager.get( QNetworkRequest( Url ) );

        if( !NetRep )
        {
            exit( 1 );
        }

        qDebug() << NetRep->isRunning() << NetRep->errorString();

        if( !NetRep->isRunning() )
        {
            exit( 1 );
        }

        QObject::connect( NetRep, &QIODevice::readyRead, [&]( void )
        {
            qDebug() << "readyRead";
        } );

        QObject::connect( &Manager, &QNetworkAccessManager::finished, [&]( QNetworkReply *pNetRep )
        {
            QTemporaryFile      TempFile( "fugio-plugin-download-" );

            if( TempFile.open() )
            {
                qDebug() << TempFile.fileName();
            }

            qApp->exit();
        });

        QObject::connect( NetRep, &QNetworkReply::downloadProgress, [&]( qint64 bytesReceived, qint64 bytesTotal )
        {
            qDebug() << bytesReceived << bytesTotal;
        } );

        QObject::connect( NetRep, &QNetworkReply::errorOccurred, [&]( QNetworkReply::NetworkError pNetworkError )
                         {
                             qDebug() << pNetworkError;

            exit( 1 );
                         } );

        QObject::connect( NetRep, &QNetworkReply::sslErrors, [&]( const QList<QSslError> &pErrors )
        {

            exit( 1 );
        } );

        // TODO: if it's a remote file, copy to a plugin cache location

#if 0
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

            QStringList FP = FI.filePath().split( '/', Qt::SkipEmptyParts );
            QString FD = FP.takeFirst();

//            qDebug() << FI.filePath() << FI.isDir() << FI.isFile() << FP << FD;

			if( zip_entry_isdir( zfh ) )
			{
                //printf( "%s (dir)\n", zip_entry_name( zfh ) );
			}
			else
			{
                printf( "%s (file)\n", zip_entry_name( zfh ) );

                if( FD == "plugin" )
                {
                    qDebug() << "plugin" << FP.join( '/' );
                }
                else if( FD == "examples" )
                {
                    qDebug() << "examples" << FP.join( '/' );
                }
                else if( FD == "include" )
                {
                    qDebug() << "include" << FP.join( '/' );
                }
                else if( FD == "libs" )
                {
                    qDebug() << "libs" << FP.join( '/' );
                }
                else
                {
                    qDebug() << "unknown" << FD << FP.join( '/' );
                }

                FI.baseName();
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
#endif
	}

    return( a.exec() );
}
