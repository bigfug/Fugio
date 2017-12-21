#include <QApplication>
#include <QSurfaceFormat>

#include "show.h"

int main( int argc, char *argv[] )
{
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

	QApplication	 A( argc, argv );

	fugio::Show		 S;

	return( S.exec( A ) );
}
