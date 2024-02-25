#include "mainwindow.h"
#include "app.h"
#include <QDir>
#include <QPluginLoader>
#include <QDebug>
#include <QSharedPointer>
#include <QDateTime>
#include <QStandardPaths>
#include <QLibraryInfo>
#include <QTranslator>
#include <QMessageBox>
#include <QSurfaceFormat>
#include <QSplashScreen>
#include <QCommandLineParser>
#include <QProcess>
#include <QProgressDialog>
#include <QPushButton>
#include <QFileDialog>
#include <QJsonArray>

#include <fugio/plugin_interface.h>
#include <fugio/node_control_interface.h>

#include <fugio/app_helper.h>

#if defined( Q_OS_RASPBERRY_PI )
#include <bcm_host.h>
#endif

#if QT_VERSION < QT_VERSION_CHECK( 5, 5, 0 )
#define qInfo qDebug
#endif

// Little trick I picked up from StackExchange to add quotes to a DEFINE

#define Q(x) #x
#define QUOTE(x) Q(x)

//-------------------------------------------------------------------------

int main( int argc, char *argv[] )
{
	QSplashScreen	*SplashScreen = Q_NULLPTR;

#if defined( Q_OS_RASPBERRY_PI )
	bcm_host_init();
#endif

	qInstallMessageHandler( App::logger_static );

	QApplication::setApplicationName( "Fugio" );
	QApplication::setOrganizationDomain( "Fugio" );

#if QT_VERSION < QT_VERSION_CHECK( 5, 4, 0 )
    QApplication::setApplicationVersion( QUOTE( FUGIO_APP_VERSION ) );
#else
    QApplication::setApplicationVersion( QString( "%1 (%2/%3)" ).arg( QUOTE( FUGIO_APP_VERSION ) ).arg( QSysInfo::buildCpuArchitecture() ).arg( QSysInfo::currentCpuArchitecture() ) );
#endif

	//-------------------------------------------------------------------------

	fugio::AppHelper	HLP;

	HLP.processCommandLine( argc, argv );

	HLP.checkForHelpOption();

	qInfo() << "Configuration Directory" << HLP.configDirectory().absolutePath();
	qInfo() << "Configuration File" << HLP.configFile();

	App::setDataDirectory( HLP.configDirectory() );

	const QString	CfgDir = App::dataDirectory().absolutePath();

	SettingsHelper::setSettingsFormat( HLP.configFile(), QSettings::IniFormat );

	//-------------------------------------------------------------------------

	App::setLogFileName( QDir( CfgDir ).absoluteFilePath( "fugio.log" ) );

	if( QDir( CfgDir ).mkpath( "dummy" ) )
	{
		QDir( CfgDir ).rmdir( "dummy" );
	}

	//-------------------------------------------------------------------------

	int		 RET = 0;
	App		*APP = new App( argc, argv );

	if( !APP )
	{
		return( -1 );
	}

	//-------------------------------------------------------------------------

	if( true )
	{
		SettingsHelper  Helper;
		PluginConfig    PC( Helper );
		PluginCache		Cache;
		QStringList		pluginList;

		if( PC.installedPlugins().isEmpty() )
		{
			QMessageBox		MsgBox;

			MsgBox.setIcon( QMessageBox::Question );

			MsgBox.setText( "Fugio Plugin Initialisation" );
			MsgBox.setInformativeText( "Fugio doesn't have any plugins installed yet.\n\nDo you want to install the default plugins? (recommended)" );

			QPushButton *buttonSelect = MsgBox.addButton( "Local...", QMessageBox::ActionRole );
			QPushButton *buttonYes = MsgBox.addButton( "Yes", QMessageBox::YesRole );

			MsgBox.addButton( "Skip", QMessageBox::NoRole );

			MsgBox.exec();

			QList<QUrl>		repoUrls;
			bool			repoAddAll = false;

			if( MsgBox.clickedButton() == buttonSelect )
			{
				QUrl repoUrl = QFileDialog::getOpenFileUrl( Q_NULLPTR, "Choose repository manifest", QDir::currentPath(), "Manifest (*.json)" );

				if( !repoUrl.isEmpty() )
				{
					repoUrls << repoUrl;

					repoAddAll = true;
				}
			}
			else if( MsgBox.clickedButton() == buttonYes )
			{
				PluginActionDownload    BootstrapDown( QUrl( "https://raw.githubusercontent.com/bigfug/Fugio/develop/fugio-bootstrap.json" ) );

				BootstrapDown.setAutoRemove( true );

				if( BootstrapDown.action() )
				{
					QFile   BootstrapFile( BootstrapDown.tempFileName() );

					if( BootstrapFile.open( QFile::ReadOnly ) )
					{
						QJsonParseError	JsonError;

						QJsonDocument BootstrapJson = QJsonDocument::fromJson( BootstrapFile.readAll(), &JsonError );

						if( !BootstrapJson.isNull() )
						{
							QJsonArray      RepoArray = BootstrapJson[ "repositories" ].toArray();
							QJsonArray      PluginArray = BootstrapJson[ "plugins" ].toArray();

							for( QJsonValueRef r : RepoArray )
							{
								repoUrls << QUrl( r.toString() );
							}

							for( QJsonValueRef r : PluginArray )
							{
								pluginList << r.toString();
							}
						}
					}
				}
			}

			for( QUrl &repoUrl : repoUrls )
			{
				QString         repoManifestFilename;
				bool            repoManifestRemove = false;
				QDateTime       repoModified;

				qInfo() << "Downloading plugin repository" << repoUrl.toDisplayString();

				if( !repoUrl.isLocalFile() )
				{
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
					QFileInfo repoFileInfo( repoUrl.toLocalFile() );

					repoManifestFilename = repoFileInfo.absoluteFilePath();

					repoModified = repoFileInfo.lastModified().toUTC();
				}

				if( !repoManifestFilename.isEmpty() )
				{
					PluginRepoManifest      RepoManifest( repoManifestFilename, "win64" );

					RepoManifest.setModified( repoModified );

					Cache.addRepoManifest( RepoManifest, repoUrl );

					if( repoAddAll )
					{
						pluginList.append( RepoManifest.pluginList() );
					}

					if( repoManifestRemove )
					{
						QFile::remove( repoManifestFilename );
					}
				}
			}
		}

		QString repoReload = HLP.reloadRepo();

		if( !repoReload.isEmpty() )
		{
			QString repoManifestFilename = Cache.repoCacheDirectory().filePath( Cache.repoManifestFilename( repoReload ) );

			PluginRepoManifest      RepoManifest( repoManifestFilename, "win64" );

			pluginList.append( RepoManifest.pluginList() );
		}

		if( !pluginList.isEmpty() )
		{
			QProgressDialog	Dialog;

			Dialog.setMinimumDuration( 0 );

			Dialog.setRange( 0, pluginList.count() );

			Helper.beginGroup( "plugin-update" );

			Helper.beginWriteArray( "install" );

			int ArrayIndex = 0;

			for( QString &PluginName : pluginList )
			{
				QString repoManifestFilename = Cache.repoFromPlugin( PluginName );

				if( repoManifestFilename.isEmpty() )
				{
					qWarning() << "Couldn't install plugin" << PluginName;

					continue;
				}

				repoManifestFilename = Cache.repoCacheDirectory().filePath( Cache.repoManifestFilename( repoManifestFilename ) );

				PluginRepoManifest      RepoManifest( repoManifestFilename, "win64" );

				QVersionNumber PluginVersion = RepoManifest.pluginLatestVersion( PluginName );

				Dialog.setLabelText( QCoreApplication::translate( "main", "Installing plugin: %1 (%2)" ).arg( PluginName, PluginVersion.toString() ) );

				Dialog.setValue( ArrayIndex );

				QString   pluginFilename;
				bool      pluginRemove = false;
				QUrl      PluginUrl = Cache.pluginUrl( PluginName, PluginVersion );

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

				if( !pluginFilename.isEmpty() )
				{
					if( Cache.addPluginToCache( PluginName, PluginVersion, pluginFilename ) )
					{
						Helper.setArrayIndex( ArrayIndex++ );

						Helper.setValue( "plugin", PluginName );
						Helper.setValue( "version", PluginVersion.toString() );
					}
				}

				if( pluginRemove )
				{
					QFile::remove( pluginFilename );
				}
			}

			Helper.endArray();

			Helper.endGroup();
		}
	}

	//-------------------------------------------------------------------------

	if( true )
	{
		SettingsHelper	Helper;
		PluginConfig	Config( Helper );
		PluginCache		Cache;

		// Read the plugin config changes before acting upon them

		QStringList		RemoveList;
		QMap<QString,QVersionNumber>	InstallList;

		Helper.beginGroup( "plugin-update" );

		int ArrayCount = Helper.beginReadArray( "remove" );

		for( int ArrayIndex = 0 ; ArrayIndex < ArrayCount ; ArrayIndex++ )
		{
			Helper.setArrayIndex( ArrayIndex );

			QString	  PluginName = Helper.value( "plugin" ).toString();

			RemoveList << PluginName;
		}

		Helper.endArray();

		ArrayCount = Helper.beginReadArray( "install" );

		for( int ArrayIndex = 0 ; ArrayIndex < ArrayCount ; ArrayIndex++ )
		{
			Helper.setArrayIndex( ArrayIndex );

			QString	  PluginName = Helper.value( "plugin" ).toString();
			QVersionNumber PluginVersion = QVersionNumber::fromString( Helper.value( "version" ).toString() );

			InstallList.insert( PluginName, PluginVersion );
		}

		Helper.endArray();

		Helper.endGroup();

		// Remove plugins

		if( !RemoveList.isEmpty() )
		{
			QProgressDialog	Dialog;

			Dialog.setMinimumDuration( 0 );

			Dialog.setRange( 0, sizeof( RemoveList ) );

			qInfo() << "Removing plugins";

			for( int i = 0 ; i < RemoveList.size() ; i++ )
			{
				const QString &PluginName = RemoveList.at( i );

				Dialog.setValue( i );

				Dialog.setLabelText( QCoreApplication::translate( "main", "Removing plugin: %1" ).arg( PluginName ) );

				QVersionNumber PluginVersion = Config.installedPluginVersion( PluginName );

				QString PluginArchive = Cache.cachedPluginFilename( PluginName, PluginVersion );

				PluginActionRemove		Action( PluginArchive, gApp->dataDirectory().absolutePath() );

				if( Action.action() )
				{
					Config.setInstalledPluginVersion( PluginName, QVersionNumber() );
				}

				Dialog.setValue( i + 1 );
			}
		}

		// Install plugins

		if( !InstallList.isEmpty() )
		{
			QProgressDialog	Dialog;

			Dialog.setMinimumDuration( 0 );

			Dialog.setRange( 0, InstallList.size() );

			int i = 0;

			for( auto it = InstallList.begin() ; it != InstallList.end() ; it++ )
			{
				QString	  PluginName = it.key();
				QVersionNumber	PluginVersion = InstallList.value( PluginName );

				Dialog.setLabelText( QCoreApplication::translate( "main", "Installing plugin: %1 (%2)" ).arg( PluginName, PluginVersion.toString() ) );

				Dialog.setValue( i );

				QString PluginArchive = Cache.cachedPluginFilename( PluginName, PluginVersion );

				if( !PluginArchive.isEmpty() )
				{
					PluginActionInstall     PluginInstall( PluginArchive, gApp->dataDirectory().absolutePath() );

					if( PluginInstall.action() )
					{
						Config.setInstalledPluginVersion( PluginName, PluginVersion );
					}
				}

				Dialog.setValue( ++i );
			}
		}

		Helper.remove( "plugin-update" );
	}

	//-------------------------------------------------------------------------

	// Save these in case we need to restart later

	QStringList		AppArgs = qApp->arguments();
	QString			AppExec = AppArgs.takeFirst();

	qDebug().noquote() << QString( "%1 %2 - %3" ).arg( QApplication::applicationName(), QApplication::applicationVersion(), "started" );

	HLP.initialiseTranslator();

	//-------------------------------------------------------------------------
	// SplashImage

	QImage	SplashImage( ":/icons/fugio-splash.png" );

	if( true )
	{
		QPainter		Painter( &SplashImage );
		QFont			Font = Painter.font();

		Font.setPixelSize( SplashImage.height() / 12 );

		Painter.setFont( Font );

		QFontMetrics	FontMetrics( Font );
        QString			SplashText = QUOTE( FUGIO_APP_VERSION );
		QSize			TextSize = FontMetrics.size( Qt::TextSingleLine, SplashText );

		Painter.setPen( Qt::white );

		Painter.drawText( ( SplashImage.rect().center() + QPoint( 0, SplashImage.height() / 3 ) ) - QPoint( TextSize.width() / 2, TextSize.height() / 2 ), SplashText );
	}

	QPixmap	SplashPixmap = QPixmap::fromImage( SplashImage );

	if( ( SplashScreen = new QSplashScreen( SplashPixmap, Qt::WindowStaysOnTopHint ) ) )
	{
		SplashScreen->show();

		APP->processEvents();
	}

	//-------------------------------------------------------------------------

	APP->incrementStatistic( "started" );

	//-------------------------------------------------------------------------
	// Set command line variables

	GlobalPrivate	*PBG = &APP->global();

	HLP.setCommandLineVariables();

	MainWindow	*WND = new MainWindow();

	if( WND )
	{
		APP->setMainWindow( WND );

		APP->processEvents();

		WND->initBegin();

		HLP.registerAndLoadPlugins( App::pluginsDirectory() );

		//-------------------------------------------------------------------------

		APP->processEvents();

		WND->createDeviceMenu();

		if( true )
		{
			SettingsHelper					 CFG;

			if( CFG.status() != QSettings::NoError )
			{
				qWarning() << CFG.fileName() << "can't load";
			}

			if( CFG.format() != QSettings::IniFormat )
			{
				qWarning() << CFG.fileName() << "bad format";
			}

			PBG->loadConfig( CFG );
		}

		WND->initEnd();

		WND->show();

		QTimer::singleShot( 500, WND, SLOT(stylesApply()) );

		SplashScreen->finish( WND );

		// Load patches that were specified on the command line

		for( QString &PatchName : HLP.CLP.positionalArguments() )
		{
			qDebug() << "Loading" << PatchName << "...";

			WND->loadPatch( PatchName );
		}

		// check for recovery files

		if( PBG->contexts().isEmpty() )
		{
			WND->checkRecoveryFiles();
		}

		// prompt user for patch

		if( PBG->contexts().isEmpty() )
		{
			WND->promptUserForPatch();
		}

		PBG->start();

		RET = APP->exec();

		PBG->stop();

		if( true )
		{
			SettingsHelper					 CFG;

			// CFG.clear();

			PBG->saveConfig( CFG );
		}

		PBG->clear();

		PBG->unloadPlugins();

		APP->setMainWindow( 0 );

		delete WND;
	}

	APP->incrementStatistic( "finished" );

	HLP.cleanup();

	if( APP->restartApp() )
	{
		RET = App::EXIT_RESTART;
	}

	delete APP;

	qDebug().noquote() << QString( "%1 %2 - %3" ).arg( QApplication::applicationName(), QApplication::applicationVersion(), "finished" );

	App::log_file( "" );

	if( RET == App::EXIT_RESTART )
	{
		QProcess::startDetached( AppExec, AppArgs ); //application restart

		RET = 0;
	}

	return( RET );
}
