#include <QOpenGLContext>
#include <QDateTime>

#include <QDebug>
#include <QNetworkAccessManager>

#include "app.h"
#include "mainwindow.h"
#include "pinprivate.h"

#include "nodeitem.h"
#include "pinitem.h"

#include <fugio/utils.h>

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

	int				Value =  Settings.value( pName, 0 ).toInt() + 1;

	Settings.setValue( pName, Value );

	Settings.endGroup();

	recordData( pName, QString::number( Value ) );
}

void App::recordData( const QString &pName, const QString &pValue )
{
	QSettings		Settings;

	if( !Settings.value( "data-collection-permission", false ).toBool() )
	{
		return;
	}

	QUuid			Instance = Settings.value( "instance", QUuid() ).toUuid();

	if( Instance.isNull() )
	{
		Instance = QUuid::createUuid();

		Settings.setValue( "instance", Instance );
	}

	QString			Url = QString( "http://stats.bigfug.com/fugio-stats.php?i=%1&%2=%3" ).arg( fugio::utils::uuid2string( Instance ) ).arg( pName ).arg( pValue );

	if( mNetworkAccessManager.networkAccessible() == QNetworkAccessManager::Accessible )
	{
		mNetworkAccessManager.get( QNetworkRequest( Url ) );
	}
}
