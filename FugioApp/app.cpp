#include <QOpenGLContext>
#include <QDateTime>

#include <QDebug>

#include "app.h"
#include "mainwindow.h"
#include "pinprivate.h"

#include "nodeitem.h"
#include "pinitem.h"

App::App( int &argc, char **argv ) :
	QApplication( argc, argv ), mMainWindow( 0 ), mGlobal( 0 )
{
	mGlobal = qobject_cast<GlobalPrivate *>( fugio::fugio()->qobject() );
}

App::~App( void )
{
	if( mGlobal )
	{
		delete mGlobal;

		mGlobal = 0;
	}
}

void App::setMainWindow( MainWindow *pMainWindow )
{
	mMainWindow = pMainWindow;
}

MainWindow *App::mainWindow( void )
{
	return( mMainWindow );
}

void App::incrementStatistic( const QString &pName )
{
	QSettings		Settings;

	Settings.beginGroup( "statistics" );

	Settings.setValue( pName, Settings.value( pName, 0 ).toInt() + 1 );

	Settings.endGroup();
}
