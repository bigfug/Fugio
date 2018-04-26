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

#include <fugio/app_helper.h>

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

	//-------------------------------------------------------------------------

	fugio::AppHelper	HLP;

	HLP.processCommandLine( argc, argv );

	int		 RET = 0;
	App		*APP = new App( argc, argv );

	if( !APP )
	{
		return( -1 );
	}

	HLP.checkForHelpOption();

	qDebug() << QString( "%1 %2 - %3" ).arg( QApplication::applicationName() ).arg( QApplication::applicationVersion() ).arg( "started" );

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

		HLP.registerAndLoadPlugins();

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

		for( QString PatchName : HLP.CLP.positionalArguments() )
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

	HLP.cleanup();

	delete APP;

	qDebug() << QString( "%1 %2 - %3" ).arg( QApplication::applicationName() ).arg( QApplication::applicationVersion() ).arg( "finished" );

	log_file( "" );

	return( RET );
}
