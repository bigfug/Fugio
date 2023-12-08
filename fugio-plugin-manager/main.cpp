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
#include <iostream>

#include "QtCore/qglobal.h"
#include "pluginmanager.h"

void progressBar( float pProgress )
{
    int barWidth = 70;

    std::cout << "[";
    int pos = barWidth * pProgress;
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "] " << int(pProgress * 100.0) << " %\r";
    std::cout.flush();

}

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

	PluginCache			PC;

    qDebug() << PC.pluginConfigFilename();

	//-------------------------------------------------------------------------
	// Set up the arguments for the command line parser.

	QCommandLineParser  CommandLine;

	CommandLine.setOptionsAfterPositionalArgumentsMode( QCommandLineParser::ParseAsOptions );
	CommandLine.setApplicationDescription("Command line control of Fugio plugins");
	CommandLine.addVersionOption();
	CommandLine.addHelpOption();

	// Add our own help option (instead of addHelpOption()) so we can offer help for each command
	// QCommandLineOption helpOption(QStringList() << "h" << "help",
	// 							   QCoreApplication::translate("main", "Help"));
	// CommandLine.addOption(helpOption);

	// Verbose
	QCommandLineOption verboseOption( "V", QCoreApplication::translate("main", "Verbose reporting"));

	CommandLine.addOption(verboseOption);

	// The directory where Fugio is installed

    QCommandLineOption PluginConfigOption(QStringList() << "c" << "plugin-config",
                                             QCoreApplication::translate( "main", "Filename of plugin config." ),
                                             QCoreApplication::translate( "main", "config"));

    CommandLine.addOption( PluginConfigOption );

	//-------------------------------------------------------------------------
	// The command we have to execute

	CommandLine.addPositionalArgument( "command", "repo add\nrepo rem\nrepo list", "<command> [args]" );


	//-------------------------------------------------------------------------
	// Process the command line

	QTextStream cout(stdout);
	QTextStream cerr(stderr);

	CommandLine.process( a.arguments() );

	QStringList args = CommandLine.positionalArguments();
	if (args.isEmpty()) {
		qDebug() << args;
		cerr << "Missing subcommand!\n";
		CommandLine.showHelp( 1 );
		return 1;
	}

	const QString subCommand = args.first();
	if (subCommand == "repo") {
		CommandLine.addPositionalArgument("command", "The repo command");
	} else if (subCommand == "pull") {
		CommandLine.addPositionalArgument("branch", "The remote branch to pull from.");
	} else if (subCommand == "push") {
		CommandLine.addPositionalArgument("branch", "The remote branch to push to.");
	} else {
		cerr << "Invalid subcommand!" << endl;
		cerr << CommandLine.helpText();
	}

	CommandLine.parse(a.arguments());
	CommandLine.clearPositionalArguments();
	args = CommandLine.positionalArguments();

	if (args.size() < 2) {
		cerr << "Missing command line parameter" << endl;
		CommandLine.showHelp( 1 );
	}

	qDebug() << args;

	// qDebug() << CommandLine.positionalArguments();

	fputs( qPrintable( CommandLine.helpText().toLatin1() ), stdout );

	//CommandLine.process( QCoreApplication::arguments() );

#if 0
	//CommandLine.addPositionalArgument( "command", QCoreApplication::translate( "main", "The command to execute." ), "<install|info>" );
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
	else if( command == "repo" )
	{
		CommandLine.clearPositionalArguments();
		CommandLine.addPositionalArgument( "repo", "Repository command", "repo [command]");
		CommandLine.process(a);
	}
	else if( command == "repo-add" )
	{
		CommandLine.clearPositionalArguments();
		CommandLine.addPositionalArgument( "repo-add", "Add a plugin repository from a URL", "repo-add [url]");
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

    PluginRepoManifest PRM( "D:/fugio-manifest.json", "win64" );

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

        if( !QFile::exists( UrlTxt ) )
        {
            QUrl	Url( UrlTxt );

            if( Url.scheme().isEmpty() )
            {
                Url = QUrl::fromLocalFile( UrlTxt );
            }

            if( !Url.isLocalFile() )
            {
                qDebug() << Url;
                qDebug() << QSslSocket::supportsSsl();
                qDebug() << QSslSocket::sslLibraryVersionString();

                PluginActionDownload    Action( Url, "test-download.png" );

                QObject::connect( &Action, &PluginActionDownload::status, [&]( const QString &pStatus )
                                 {

                                 });

                QObject::connect( &Action, &PluginActionDownload::downloadProgress, [&]( qint64 bytesReceived, qint64 bytesTotal )
                                 {
                                     progressBar( float( bytesReceived ) / float( bytesTotal ) );
                                 });

                Action.action();

                std::cout << std::endl;
            }
        }

        QFileInfo   LocalFile( UrlTxt );

        if( !LocalFile.exists() )
        {
            exit( 1 );
        }

        PluginActionInstall     ActionInstall( UrlTxt, "D:\\dev\\projects\\Fugio\\Fugio-4.0.0-win64" );

        QObject::connect( &ActionInstall, &PluginActionInstall::status, [&]( const QString &pStatus )
                         {
            qInfo() << pStatus;
        });

        ActionInstall.action();

        // TODO: if it's a remote file, copy to a plugin cache location

#if 0

		const QString authorName = JSON[ "authorName" ].toString();

		qDebug() << authorName;
#endif
	}
#endif

    return( 0 ) ;//a.exec() );
}
