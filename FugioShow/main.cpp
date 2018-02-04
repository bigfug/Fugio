#include <QApplication>
#include <QSurfaceFormat>
#include <QLibraryInfo>
#include <QTranslator>

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

	//-------------------------------------------------------------------------
	// Install translator

	QLocale		SystemLocal;

	QTranslator qtTranslator;

	if( QFileInfo::exists( QLibraryInfo::location( QLibraryInfo::TranslationsPath ) ) )
	{
		qtTranslator.load( SystemLocal, QLatin1String( "qt" ), QLatin1String( "_" ), QLibraryInfo::location( QLibraryInfo::TranslationsPath ) );
	}

	if( !qtTranslator.isEmpty() )
	{
		qApp->installTranslator( &qtTranslator );
	}

	static QTranslator		Translator;

	if( Translator.load( QLocale(), QLatin1String( "translations" ), QLatin1String( "_" ), ":/" ) )
	{
		qApp->installTranslator( &Translator );
	}


	fugio::Show		 S;

	return( S.exec( A ) );
}
