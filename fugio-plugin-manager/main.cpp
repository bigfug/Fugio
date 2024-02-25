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

    if( !QSslSocket::supportsSsl() )
    {
        qWarning( "SSL support not detected - downloading from https sources won't work" );
    }

	//-------------------------------------------------------------------------
	// Variables

    // QSettings FugioGlobalSettings( QSettings::SystemScope, "Fugio", "Fugio" );

    PluginManager       PM;

	PluginCache			PC;

	//-------------------------------------------------------------------------
	// Set up the arguments for the command line parser.

	QCommandLineParser  CommandLine;

	CommandLine.setApplicationDescription("Command line control of Fugio plugins");
	CommandLine.addVersionOption();
    CommandLine.addHelpOption();

    QCommandLineOption  RepoAdd( "add", "Add plugin repository", "url|file" );
    QCommandLineOption  RepoRemove( "remove", "Remove a plugin repository", "name" );
    QCommandLineOption  PluginInstall( "install", "Install a plugin", "name" );
    QCommandLineOption  PluginUnInstall( "uninstall", "Uninstall a plugin", "name" );

    CommandLine.addOption( RepoAdd );
    CommandLine.addOption( RepoRemove );
    CommandLine.addOption( PluginInstall );
    CommandLine.addOption( PluginUnInstall );

    // Verbose
    QCommandLineOption verboseOption( "V", QCoreApplication::translate("main", "Verbose reporting"));

    CommandLine.addOption(verboseOption);

    // The directory where Fugio is installed

    QCommandLineOption PluginConfigOption(QStringList() << "c" << "plugin-config",
                                          QCoreApplication::translate( "main", "Filename of plugin config." ),
                                          QCoreApplication::translate( "main", "config"));

    CommandLine.addOption( PluginConfigOption );

    CommandLine.process( a );

    //-------------------------------------------------------------------------

    const bool verbose = CommandLine.isSet( verboseOption );

    if( verbose )
    {
        printf("%s %s\n", qPrintable(QCoreApplication::applicationName()),
               qPrintable(QCoreApplication::applicationVersion()));
    }

    if( verbose )
    {
        printf( "Global config filename: %s\n", qPrintable( PC.pluginConfigFilename() ) );
    }

    //-------------------------------------------------------------------------
    // Process the Fugio directory command line option

    QFileInfo PluginConfigFileInfo( "plugin-config.ini" );

    if( CommandLine.isSet( PluginConfigOption ) )
    {
        PluginConfigFileInfo = QFileInfo( CommandLine.value( PluginConfigOption ) );
    }

	QSettings	Settings( PluginConfigFileInfo.absoluteFilePath(), QSettings::IniFormat );

	PluginConfig Config( Settings );

    if( verbose )
    {
        printf( "Plugin config file: %s\n", qPrintable( PluginConfigFileInfo.absoluteFilePath() ) );
    }

    //-------------------------------------------------------------------------

	for( QString &ArgDat : CommandLine.values( RepoAdd ) )
    {
        QUrl            repoUrl;
        QString         repoManifestFilename;
        bool            repoManifestRemove = false;
        QDateTime       repoModified;

        // qDebug() << repoCommand << repoData << QFileInfo( repoData ).isFile();

        if( !QFileInfo( ArgDat ).isFile() )
        {
            repoUrl = QUrl( ArgDat );

            qDebug() << repoUrl;

            PluginActionDownload    RepoDown( repoUrl );

            RepoDown.setAutoRemove( false );

            if( RepoDown.action() )
            {
                repoManifestFilename = RepoDown.tempFileName();

                repoModified = RepoDown.modified();

                repoManifestRemove = true;
            }
        }
        else
        {
            QFileInfo repoFileInfo( ArgDat );

            repoUrl = QUrl::fromLocalFile( repoFileInfo.absoluteFilePath() );

            qDebug() << repoUrl;

            repoManifestFilename = repoFileInfo.absoluteFilePath();

            repoModified = repoFileInfo.lastModified().toUTC();
        }

        if( !repoManifestFilename.isEmpty() )
        {
            PluginRepoManifest      RepoManifest( repoManifestFilename, "win64" );

            RepoManifest.setModified( repoModified );

            PC.addRepoManifest( RepoManifest, repoUrl );

            if( repoManifestRemove )
            {
                QFile::remove( repoManifestFilename );
            }
        }
    }

	for( QString &ArgDat : CommandLine.values( RepoRemove ) )
    {
        PC.removeRepo( ArgDat );
    }

	for( QString &ArgDat : CommandLine.values( PluginInstall ) )
    {
        QVersionNumber      PluginVersion = Config.installedPluginVersion( ArgDat );

        if( !PluginVersion.isNull() )
        {
            continue;
        }

        PluginVersion = PC.latestPluginVersion( ArgDat );

        if( PluginVersion.isNull() )
        {
            continue;
        }

        QString     PluginArchive = PC.cachedPluginFilename( ArgDat, PluginVersion );

        if( PluginArchive.isEmpty() )
        {
            QString   pluginFilename;
            bool      pluginRemove = false;
            QUrl      PluginUrl = PC.pluginUrl( ArgDat, PluginVersion );

            qDebug() << PluginUrl;

            if( !PluginUrl.isLocalFile() )
            {
                PluginActionDownload    pluginDown( PluginUrl );

                pluginDown.setAutoRemove( false );

                if( pluginDown.action() )
                {
                    pluginFilename = pluginDown.tempFileName();

                    pluginRemove = true;
                }
            }
            else
            {
                pluginFilename = QDir::toNativeSeparators( PluginUrl.toLocalFile() );
            }

            PC.addPluginToCache( ArgDat, PluginVersion, pluginFilename );

            if( pluginRemove )
            {
                QFile::remove( pluginFilename );
            }
        }

        PluginArchive = PC.cachedPluginFilename( ArgDat, PluginVersion );

        if( !PluginArchive.isEmpty() )
        {
            PluginActionInstall     PluginInstall( PluginArchive, PM.pluginDirectory().absolutePath() );

            if( PluginInstall.action() )
            {
                Config.setInstalledPluginVersion( ArgDat, PluginVersion );
            }
        }
    }

	for( QString &ArgDat : CommandLine.values( PluginUnInstall ) )
    {
    }

    return( 0 );
}
