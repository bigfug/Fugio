#include <QApplication>
#include <QSurfaceFormat>
#include <QLibraryInfo>

#include <fugio/app_helper.h>
#include <fugio/global_interface.h>
#include <fugio/context_interface.h>

#if defined( Q_OS_RASPBERRY_PI )
#include <bcm_host.h>
#endif

class ShowApp : public QApplication, public fugio::AppHelper
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

#if QT_VERSION >= QT_VERSION_CHECK( 5, 4, 0 )
	QCoreApplication::setAttribute( Qt::AA_ShareOpenGLContexts );
#endif

	QApplication::setAttribute( Qt::AA_UseDesktopOpenGL );

	QSurfaceFormat	SurfaceFormat;

	SurfaceFormat.setDepthBufferSize( 24 );
	SurfaceFormat.setProfile( QSurfaceFormat::CoreProfile );
	SurfaceFormat.setSamples( 4 );
	SurfaceFormat.setVersion( 4, 5 );

	QSurfaceFormat::setDefaultFormat( SurfaceFormat );

	ShowApp		 A( argc, argv );

	A.processCommandLine();

	A.initialiseTranslator();

	A.setCommandLineVariables();

	A.registerAndLoadPlugins();

	fugio::GlobalInterface	*G = fugio::fugio();

	for( QString PatchName : A.CLP.positionalArguments() )
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

	A.cleanup();

	return( R );
}
