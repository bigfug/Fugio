#include <QApplication>
#include <QSurfaceFormat>
#include <QLibraryInfo>

#include <fugio/app_helper.h>
#include <fugio/global_interface.h>
#include <fugio/context_interface.h>

#if defined( Q_OS_RASPBERRY_PI )
#include <bcm_host.h>
#endif

class ShowApp : public QApplication
{
public:
	ShowApp( int &argc, char **argv )
		: QApplication( argc, argv )
	{

	}
};

int main( int argc, char *argv[] )
{
#if defined( Q_OS_RASPBERRY_PI )
	bcm_host_init();
#endif

	fugio::AppHelper	H;

	H.processCommandLine( argc, argv );

	ShowApp		 A( argc, argv );

	H.checkForHelpOption();

	H.initialiseTranslator();

	H.setCommandLineVariables();

	QDir	AppDir = QDir( qApp->applicationDirPath() );

#if defined( Q_OS_LINUX ) && !defined( QT_DEBUG )
	AppDir.cdUp();
	AppDir.cd( "lib" );
	AppDir.cd( "fugio" );
#endif

#if defined( Q_OS_MAC )
		qDebug() << "App Binary Directory:" << AppDir.absolutePath();

		AppDir.cdUp();
		AppDir.cdUp();
		AppDir.cdUp();

		qDebug() << "App Directory:" << AppDir.absolutePath();
#endif

	H.registerAndLoadPlugins( AppDir );

	fugio::GlobalInterface	*G = fugio::fugio();

	for( QString PatchName : H.CLP.positionalArguments() )
	{
		QSharedPointer<fugio::ContextInterface>	C;

		C = G->newContext();

		if( C )
		{
			if( !C->load( PatchName ) )
			{
				qWarning() << QCoreApplication::tr( "Couldn't load patch %1" ).arg( PatchName );
			}
		}
	}

	G->start();

	int R = A.exec();

	G->stop();

	G->clear();

	G->unloadPlugins();

	H.cleanup();

	return( R );
}
