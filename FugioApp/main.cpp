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

#include "contextprivate.h"
#include "contextsubwindow.h"
#include "contextwidgetprivate.h"

#include <fugio/plugin_interface.h>
#include <fugio/node_control_interface.h>

#if defined( Q_OS_RASPBERRY_PI )
#include <bcm_host.h>
#endif

QSplashScreen	*SplashScreen = Q_NULLPTR;

QString			LogNam;

void log_file( const QString &pLogDat )
{
	QFile		FH( LogNam );

	if( FH.open( QFile::Append ) )
	{
		QTextStream	TS( &FH );

		TS << pLogDat << "\n";

		FH.close();
	}
}

void logger_static( QtMsgType type, const QMessageLogContext &context, const QString &msg )
{
	Q_UNUSED( context )

	QByteArray localMsg = msg.toLocal8Bit();

	QString		LogDat;

	LogDat.append( QDateTime::currentDateTime().toString( Qt::ISODate ) );
	LogDat.append( ": " );
	LogDat.append( localMsg );

	switch (type)
	{
		case QtDebugMsg:
			fprintf(stderr, "DBUG: %s\n", localMsg.constData() );
			break;
#if ( QT_VERSION >= QT_VERSION_CHECK( 5, 5, 0 ) )
		case QtInfoMsg:
			fprintf(stderr, "INFO: %s\n", localMsg.constData() );
			break;
#endif
		case QtWarningMsg:
			fprintf(stderr, "WARN: %s\n", localMsg.constData() );
			break;
		case QtCriticalMsg:
			fprintf(stderr, "CRIT: %s\n", localMsg.constData() );
			break;
		case QtFatalMsg:
			fprintf(stderr, "FATL: %s\n", localMsg.constData() );
			break;
	}

	fflush( stderr );

	log_file( LogDat );

	if( type == QtFatalMsg )
	{
		abort();
	}
}

#if QT_VERSION < QT_VERSION_CHECK( 5, 5, 0 )
#define qInfo qDebug
#endif

// Little trick I picked up from StackExchange to add quotes to a DEFINE

#define Q(x) #x
#define QUOTE(x) Q(x)

//-------------------------------------------------------------------------

int main( int argc, char *argv[] )
{
#if defined( Q_OS_RASPBERRY_PI )
	bcm_host_init();
#endif

	// Translation Test

	//QLocale::setDefault( QLocale( QLocale::German, QLocale::Austria ) );

	qInstallMessageHandler( logger_static );

	QApplication::setApplicationName( "Fugio" );
	QApplication::setOrganizationDomain( "Fugio" );

#if QT_VERSION < QT_VERSION_CHECK( 5, 4, 0 )
	QApplication::setApplicationVersion( QUOTE( FUGIO_VERSION ) );
#else
	QApplication::setApplicationVersion( QString( "%1 (%2/%3)" ).arg( QUOTE( FUGIO_VERSION ) ).arg( QSysInfo::buildCpuArchitecture() ).arg( QSysInfo::currentCpuArchitecture() ) );
#endif

	const QString	CfgDir = QStandardPaths::writableLocation( QStandardPaths::DataLocation );

	LogNam = QDir( CfgDir ).absoluteFilePath( "fugio.log" );

	if( QDir( CfgDir ).mkpath( "dummy" ) )
	{
		QDir( CfgDir ).rmdir( "dummy" );
	}

	qDebug() << QString( "%1 %2 - %3" ).arg( QApplication::applicationName() ).arg( QApplication::applicationVersion() ).arg( "started" );

	//-------------------------------------------------------------------------
	// Command Line Parser

	QCommandLineParser		CLP;

	CLP.setSingleDashWordOptionMode( QCommandLineParser::ParseAsLongOptions );

	CLP.setApplicationDescription( "Fugio" );
	CLP.addHelpOption();
	CLP.addVersionOption();

	QCommandLineOption OptionClearSettings( "clear-settings", "Clear all settings (mainly for testing purposes)" );

	CLP.addOption( OptionClearSettings );

	QCommandLineOption OptionOpenGL( "opengl", QCoreApplication::translate( "main", "Select OpenGL backend" ) );
	QCommandLineOption OptionGLES( "gles", QCoreApplication::translate( "main", "Select OpenGL ES backend" ) );
	QCommandLineOption OptionGLSW( "glsw", QCoreApplication::translate( "main", "Select OpenGL software backend" ) );

	CLP.addOption( OptionOpenGL );
	CLP.addOption( OptionGLES );
	CLP.addOption( OptionGLSW );

	QCommandLineOption OptionPluginPath( QStringList() << "pp" << "plugin-path", "Look for plugins in <path>", "path" );
	QCommandLineOption OptionEnablePlugin( QStringList() << "pe" << "enable-plugin", "Enable plugin <plugin>.", "plugin" );
	QCommandLineOption OptionDisablePlugin( QStringList() << "pd" << "disable-plugin", "Disable plugin <plugin>.", "plugin" );

	CLP.addOption( OptionPluginPath );
	CLP.addOption( OptionEnablePlugin );
	CLP.addOption( OptionDisablePlugin );

	QCommandLineOption OptionLocale( QStringList() << "l" << "locale", "Set language locale to <locale>.", QLocale().bcp47Name() );

	CLP.addOption( OptionLocale );

	QCommandLineOption OptionDefine( QStringList() << "d" << "define", "Define a <variable>.", "variable" );

	CLP.addOption( OptionDefine );

	QStringList		Args;

	for( int i = 0 ; i < argc ; i++ )
	{
		Args << QString::fromLocal8Bit( argv[ i ] );
	}

	CLP.parse( Args );

	//-------------------------------------------------------------------------
	// Command Line OpenGL

#if QT_VERSION >= QT_VERSION_CHECK( 5, 4, 0 )
	QCoreApplication::setAttribute( Qt::AA_ShareOpenGLContexts );
#endif

	QSurfaceFormat	SurfaceFormat;

	if( CLP.isSet( OptionGLSW ) )
	{
#if QT_VERSION >= QT_VERSION_CHECK( 5, 4, 0 )
		QCoreApplication::setAttribute( Qt::AA_UseSoftwareOpenGL );
#endif
	}
	else if( CLP.isSet( OptionGLES ) )
	{
#if QT_VERSION >= QT_VERSION_CHECK( 5, 3, 0 )
		QCoreApplication::setAttribute( Qt::AA_UseOpenGLES );
#endif
	}
	else
	{
#if QT_VERSION >= QT_VERSION_CHECK( 5, 3, 0 )
		QCoreApplication::setAttribute( Qt::AA_UseDesktopOpenGL );
#endif

#if !defined( QT_OPENGL_ES_2 )
		SurfaceFormat.setDepthBufferSize( 24 );
		SurfaceFormat.setProfile( QSurfaceFormat::CoreProfile );
		SurfaceFormat.setSamples( 4 );
		SurfaceFormat.setVersion( 4, 5 );
#endif
	}

#if defined( QT_DEBUG )
	SurfaceFormat.setOption( QSurfaceFormat::DebugContext );
#endif

	QSurfaceFormat::setDefaultFormat( SurfaceFormat );

	//-------------------------------------------------------------------------
	// Command Line Locale

	if( CLP.isSet( OptionLocale ) )
	{
		const QString	Locale = CLP.value( OptionLocale );

		QLocale::setDefault( QLocale( Locale ) );
	}

	//-------------------------------------------------------------------------

	int		 RET = 0;
	App		*APP = new App( argc, argv );

	if( APP == 0 )
	{
		return( -1 );
	}

	QImage	SplashImage( ":/icons/fugio-splash.png" );

	if( true )
	{
		QPainter		Painter( &SplashImage );
		QFont			Font = Painter.font();

		Font.setPixelSize( SplashImage.height() / 12 );

		Painter.setFont( Font );

		QFontMetrics	FontMetrics( Font );
		QString			SplashText = QUOTE( FUGIO_VERSION );
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
	// Create QSettings

	QSettings		Settings;

#if defined( QT_DEBUG )
	qInfo() << Settings.fileName();
#endif

	//-------------------------------------------------------------------------
	// Ask the user if we can collect some anonymous data about how they use Fugio

//	if( Settings.value( "first-time", true ).toBool() )
//	{
//		if( !Settings.value( "asked-data-collection-permission", false ).toBool() )
//		{
//			if( QMessageBox::question( nullptr, "Help Fugio Improve", "To understand how users are using Fugio, we would like to collect some anonymous data that will be stored on our website.\n\nYou can opt in or out at any time.\n\nWould you allow Fugio to do this?", QMessageBox::Yes, QMessageBox::No ) == QMessageBox::Yes )
//			{
//				Settings.setValue( "data-collection-permission", true );
//			}
//			else
//			{
//				Settings.setValue( "data-collection-permission", false );
//			}

//			Settings.setValue( "asked-data-collection-permission", true );
//		}

//		Settings.setValue( "first-time", false );
//	}

	//-------------------------------------------------------------------------

	APP->incrementStatistic( "started" );

	//-------------------------------------------------------------------------
	// Install translator

	QLocale		SystemLocal;

	const QString		TranslatorSource = QDir::current().absoluteFilePath( "translations" );

	QTranslator qtTranslator;

	if( QFileInfo::exists( QLibraryInfo::location( QLibraryInfo::TranslationsPath ) ) )
	{
		qtTranslator.load( SystemLocal, QLatin1String( "qt" ), QLatin1String( "_" ), QLibraryInfo::location( QLibraryInfo::TranslationsPath ) );
	}
	else if( QFileInfo::exists( TranslatorSource ) )
	{
		qtTranslator.load( SystemLocal, QLatin1String( "qt" ), QLatin1String( "_" ), TranslatorSource, QLatin1String( ".qm" ) );
	}

	if( !qtTranslator.isEmpty() )
	{
		qApp->installTranslator( &qtTranslator );
	}

	QTranslator		Translator;

	if( Translator.load( SystemLocal, QLatin1String( "translations" ), QLatin1String( "_" ), ":/" ) )
	{
		qApp->installTranslator( &Translator );
	}

	//-------------------------------------------------------------------------
	// Set command line variables

	GlobalPrivate	*PBG = &APP->global();

	if( CLP.isSet( OptionDefine ) )
	{
		QMap<QString,QString>		CommandLineVariables;

		for( QString S : CLP.values( OptionDefine ) )
		{
			const int	SepPos = S.indexOf( ':' );

			if( SepPos > 0 )
			{
				QString		K = S.left( SepPos );
				QString		V = S.mid( SepPos + 1 );

				CommandLineVariables.insert( V, K );
			}
			else
			{
				CommandLineVariables.insert( S, "" );
			}
		}

		PBG->setCommandLineValues( CommandLineVariables );
	}

	//-------------------------------------------------------------------------
	// Register and load plugins

	QDir	PluginsDir = QDir( qApp->applicationDirPath() );

#if defined( Q_OS_LINUX ) && !defined( QT_DEBUG )
	PluginsDir.cdUp();
	PluginsDir.cd( "lib" );
	PluginsDir.cd( "fugio" );
#else

#if defined( Q_OS_MAC )
	PluginsDir.cdUp();
	PluginsDir.cdUp();
	PluginsDir.cdUp();
#endif

	while( !PluginsDir.isRoot() && PluginsDir.isReadable() && !PluginsDir.cd( "plugins" ) )
	{
		PluginsDir.cdUp();
	}
#endif

	if( !PluginsDir.isRoot() && PluginsDir.isReadable() )
	{
		qInfo() << "Plugin Directory:" << PluginsDir.absolutePath();
	}

	MainWindow	*WND = new MainWindow();

	if( WND )
	{
		APP->setMainWindow( WND );

		APP->processEvents();

		WND->initBegin();

		//-------------------------------------------------------------------------
		// Process command line enabled/disabled plugins

		if( CLP.isSet( OptionEnablePlugin ) )
		{
			PBG->setEnabledPlugins( CLP.values( OptionEnablePlugin ) );
		}

		if( CLP.isSet( OptionDisablePlugin ) )
		{
			PBG->setDisabledPlugins( CLP.values( OptionDisablePlugin ) );
		}

#if defined( Q_OS_MACX ) || defined( Q_OS_LINUX )
		if( !PluginsDir.isRoot() && PluginsDir.isReadable())
		{
			PBG->loadPlugins( PluginsDir );
		}
#else
		if( !PluginsDir.isRoot() && PluginsDir.isReadable() )
		{
			QString		CurDir = QDir::currentPath();
			QString		NxtDir = PluginsDir.absolutePath();

			QDir::setCurrent( NxtDir );

			PBG->loadPlugins( QDir::current() );

			QDir::setCurrent( CurDir );
		}
#endif

		//-------------------------------------------------------------------------
		// Load plugins from additional paths

		if( CLP.isSet( OptionPluginPath ) )
		{
			for( QString PluginPath : CLP.values( OptionPluginPath ) )
			{
				PBG->loadPlugins( QDir( PluginPath ) );
			}
		}

		PBG->initialisePlugins();

		//-------------------------------------------------------------------------

		APP->processEvents();

		WND->createDeviceMenu();

		const QString	CfgNam = QDir( CfgDir ).absoluteFilePath( "fugio.ini" );

		if( true )
		{
			QSettings					 CFG( CfgNam, QSettings::IniFormat );

			if( CFG.status() != QSettings::NoError )
			{
				qWarning() << CfgNam << "can't load";
			}

			if( CFG.format() != QSettings::IniFormat )
			{
				qWarning() << CfgNam << "bad format";
			}

			PBG->loadConfig( CFG );
		}

		WND->initEnd();

		WND->show();

		QTimer::singleShot( 500, WND, SLOT(stylesApply()) );

		SplashScreen->finish( WND );

		// Load patches that were specified on the command line

		for( QString PatchName : CLP.positionalArguments() )
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
			QSettings				 CFG( CfgNam, QSettings::IniFormat );

			CFG.clear();

			PBG->saveConfig( CFG );
		}

		PBG->clear();

		PBG->unloadPlugins();

		APP->setMainWindow( 0 );

		delete WND;
	}

	APP->incrementStatistic( "finished" );

	qApp->removeTranslator( &Translator );

	qApp->removeTranslator( &qtTranslator );

	if( CLP.isSet( OptionClearSettings ) )
	{
		qInfo() << "Settings cleared";

		Settings.clear();
	}

	delete APP;

	qDebug() << QString( "%1 %2 - %3" ).arg( QApplication::applicationName() ).arg( QApplication::applicationVersion() ).arg( "finished" );

	log_file( "" );

	return( RET );
}
