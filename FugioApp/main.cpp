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

#include "contextprivate.h"
#include "contextsubwindow.h"
#include "contextwidgetprivate.h"

#include <fugio/plugin_interface.h>
#include <fugio/node_control_interface.h>

QString LogNam;

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

int main(int argc, char *argv[])
{
#if defined( QT_DEBUG )
	//QLocale::setDefault( QLocale( QLocale::French, QLocale::France ) );
#endif

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

	QCoreApplication::setAttribute( Qt::AA_UseDesktopOpenGL );

#if QT_VERSION >= QT_VERSION_CHECK( 5, 4, 0 )
	QCoreApplication::setAttribute( Qt::AA_ShareOpenGLContexts );
#endif

	//-------------------------------------------------------------------------

	int		 RET = 0;
	App		*APP = new App( argc, argv );

	if( APP == 0 )
	{
		return( -1 );
	}

	//-------------------------------------------------------------------------
	// Create QSettings

	QSettings		Settings;

#if defined( QT_DEBUG )
	qInfo() << Settings.fileName();
#endif

	//-------------------------------------------------------------------------
	// Ask the user if we can collect some anonymous data about how they use Fugio

	if( Settings.value( "first-time", true ).toBool() )
	{
		if( !Settings.value( "asked-data-collection-permission", false ).toBool() )
		{
			if( QMessageBox::question( nullptr, "Help Fugio Improve", "To understand how users are using Fugio, we would like to collect some anonymous data that will be stored on our website.\n\nYou can opt in or out at any time.\n\nWould you allow Fugio to do this?", QMessageBox::Yes, QMessageBox::No ) == QMessageBox::Yes )
			{
				Settings.setValue( "data-collection-permission", true );
			}
			else
			{
				Settings.setValue( "data-collection-permission", false );
			}

			Settings.setValue( "asked-data-collection-permission", true );
		}

		Settings.setValue( "first-time", false );
	}

	//-------------------------------------------------------------------------

	APP->incrementStatistic( "started" );

	//-------------------------------------------------------------------------
	// Install translator

	QLocale		SystemLocal;

	QTranslator qtTranslator;

	if( QFileInfo::exists( QLibraryInfo::location( QLibraryInfo::TranslationsPath ) ) )
	{
		qtTranslator.load( SystemLocal, "qt_", QString(), QLibraryInfo::location( QLibraryInfo::TranslationsPath ) );
	}
	else if( QFileInfo::exists( "translations" ) )
	{
		qtTranslator.load( SystemLocal, "qt_", "translations" );
	}

	if( !qtTranslator.isEmpty() )
	{
		qApp->installTranslator(&qtTranslator);
	}

	QTranslator		Translator;

	if( !Translator.load( SystemLocal, "fugioapp_", QString(), "translations" ) )
	{
		qWarning() << "Can't load FugioApp translator for" << SystemLocal.name();
	}
	else
	{
		qApp->installTranslator( &Translator );
	}

	//-------------------------------------------------------------------------
	// Initialise Command Line Parser

	QCommandLineParser		&CLP = APP->global().commandLineParser();

	CLP.setApplicationDescription( "Fugio Editor" );

	QCommandLineOption		ClearSettingsOption( "clear-settings", "Clear all settings (mainly for testing purposes)" );

	CLP.addOption( ClearSettingsOption );

	//-------------------------------------------------------------------------
	// Register and load plugins

	GlobalPrivate	*PBG = &APP->global();

	QDir	PluginsDir = QDir( qApp->applicationDirPath() );

#if defined( Q_OS_WIN )
//	if( PluginsDir.dirName().toLower() == "debug" || PluginsDir.dirName().toLower() == "release" )
//	{
//		PluginsDir.cdUp();
//	}
#elif defined( Q_OS_MAC )
	PluginsDir.cdUp();
	PluginsDir.cdUp();
	PluginsDir.cdUp();
#endif

	PluginsDir.cd( "plugins" );

	qInfo() << "Plugin Directory:" << PluginsDir.absolutePath();

	MainWindow	*WND = new MainWindow();

	if( WND )
	{
		APP->setMainWindow( WND );

		PBG->setMainWindow( WND );

		APP->processEvents();

		WND->initBegin();

#if defined( Q_OS_MACX )
		PBG->loadPlugins( PluginsDir );

		PBG->initialisePlugins();
#else
		if( true )
		{
			QString		CurDir = QDir::currentPath();
			QString		NxtDir = PluginsDir.absolutePath();

			QDir::setCurrent( NxtDir );

			PBG->loadPlugins( QDir::current() );

			QDir::setCurrent( CurDir );

			PBG->initialisePlugins();
		}
#endif

		APP->processEvents();

		CLP.process( *APP );

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

		PBG->setMainWindow( 0 );

		APP->setMainWindow( 0 );

		delete WND;
	}

	APP->incrementStatistic( "finished" );

	qApp->removeTranslator( &Translator );

	qApp->removeTranslator( &qtTranslator );

	if( CLP.isSet( ClearSettingsOption ) )
	{
		qInfo() << "Settings cleared";

		Settings.clear();
	}

	delete APP;

	qDebug() << QString( "%1 %2 - %3" ).arg( QApplication::applicationName() ).arg( QApplication::applicationVersion() ).arg( "finished" );

	log_file( "" );

	return( RET );
}
