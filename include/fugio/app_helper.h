#ifndef APP_HELPER_H
#define APP_HELPER_H

#include <fugio/global.h>

#include <QCommandLineParser>
#include <QTranslator>
#include <QFileInfo>
#include <QCoreApplication>
#include <QSettings>
#include <QLibraryInfo>
#include <QDir>
#include <QSurfaceFormat>

#include "global_interface.h"

FUGIO_NAMESPACE_BEGIN

class AppHelper
{
public:
	AppHelper( void )
		: OptionHelp( "help" ),
		  OptionVersion( "version" ),
		  OptionClearSettings( "clear-settings", QCoreApplication::translate( "main", "Clear all settings (mainly for testing purposes)" ) ),
		  OptionOpenGL( "opengl", QCoreApplication::translate( "main", "Select OpenGL backend" ) ),
		  OptionGLES( "gles", QCoreApplication::translate( "main", "Select OpenGL ES backend" ) ),
		  OptionGLSW( "glsw", QCoreApplication::translate( "main", "Select OpenGL software backend" ) ),
		  OptionPluginPath( QStringList() << "pp" << "plugin-path", QCoreApplication::translate( "main", "Look for plugins in <path>" ), "path" ),
		  OptionEnablePlugin( QStringList() << "pe" << "enable-plugin", QCoreApplication::translate( "main", "Enable plugin <plugin>." ), "plugin" ),
		  OptionDisablePlugin( QStringList() << "pd" << "disable-plugin", QCoreApplication::translate( "main", "Disable plugin <plugin>." ), "plugin" ),
		  OptionLocale( QStringList() << "l" << "locale", QCoreApplication::translate( "main", "Set language locale to <locale>." ), QLocale().bcp47Name() ),
		  OptionDefine( QStringList() << "d" << "define", QCoreApplication::translate( "main", "Define a <variable>." ), "variable" )
	{
#if QT_VERSION >= QT_VERSION_CHECK( 5, 4, 0 )
		QCoreApplication::setAttribute( Qt::AA_ShareOpenGLContexts );
#endif

		//-------------------------------------------------------------------------
		// Command Line Parser

		CLP.setSingleDashWordOptionMode( QCommandLineParser::ParseAsLongOptions );

		CLP.setApplicationDescription( "Fugio" );

		OptionHelp    = CLP.addHelpOption();
		OptionVersion = CLP.addVersionOption();

		CLP.addPositionalArgument( "patches", QCoreApplication::translate( "main", "Patches to open (optional)."), "[patches...]" );

		CLP.addOption( OptionClearSettings );

		CLP.addOption( OptionOpenGL );
		CLP.addOption( OptionGLES );
		CLP.addOption( OptionGLSW );

		CLP.addOption( OptionPluginPath );
		CLP.addOption( OptionEnablePlugin );
		CLP.addOption( OptionDisablePlugin );

		CLP.addOption( OptionLocale );

		CLP.addOption( OptionDefine );
	}

	void processCommandLine( int argc, char **argv )
	{
		QStringList		ArgLst;

		for( int i = 0 ; i < argc ; i++ )
		{
			ArgLst << QString::fromLocal8Bit( argv[ i ] );
		}

		if( !CLP.parse( ArgLst ) )
		{
			fputs( qPrintable( CLP.errorText() ), stderr );

			exit( 1 );
		}

		if( CLP.isSet( OptionVersion ) )
		{
			CLP.showVersion();

			Q_UNREACHABLE();
		}

		QSurfaceFormat  SurfaceFormat;

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

		if( CLP.isSet( OptionLocale ) )
		{
			QLocale::setDefault( QLocale( CLP.value( OptionLocale ) ) );
		}
	}

	void checkForHelpOption( void )
	{
		if( CLP.isSet( OptionHelp ) )
		{
			CLP.showHelp();

			Q_UNREACHABLE();
		}
	}

	void initialiseTranslator( void )
	{
		//-------------------------------------------------------------------------
		// Install translator

		QLocale		SystemLocal;

		const QString		TranslatorSource = QDir::current().absoluteFilePath( "translations" );

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

		if( Translator.load( SystemLocal, QLatin1String( "translations" ), QLatin1String( "_" ), ":/" ) )
		{
			qApp->installTranslator( &Translator );
		}
	}

	//-------------------------------------------------------------------------
	// Set command line variables

	void setCommandLineVariables( void )
	{
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

			fugio::fugio()->setCommandLineValues( CommandLineVariables );
		}
	}

	void registerAndLoadPlugins( void )
	{
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

		GlobalInterface	*PBG = fugio();

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
	}

	void cleanup( void )
	{
		qApp->removeTranslator( &Translator );

		qApp->removeTranslator( &qtTranslator );

		if( CLP.isSet( OptionClearSettings ) )
		{
			qInfo() << "Settings cleared";

			QSettings().clear();
		}
	}

public:
	QCommandLineParser		CLP;

protected:
	QCommandLineOption		OptionHelp;
	QCommandLineOption		OptionVersion;

	QCommandLineOption		OptionClearSettings;
	QCommandLineOption		OptionOpenGL;
	QCommandLineOption		OptionGLES;
	QCommandLineOption		OptionGLSW;
	QCommandLineOption		OptionPluginPath;
	QCommandLineOption		OptionEnablePlugin;
	QCommandLineOption		OptionDisablePlugin;
	QCommandLineOption		OptionLocale;
	QCommandLineOption		OptionDefine;

	QTranslator				qtTranslator;
	QTranslator				Translator;
};

FUGIO_NAMESPACE_END

#endif // APP_HELPER_H
