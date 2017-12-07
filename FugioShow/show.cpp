#include "show.h"

fugio::Show::Show()
{

}

fugio::Show::~Show()
{

}

int fugio::Show::exec( QApplication &pApp )
{
	GlobalPrivate			*G = qobject_cast<GlobalPrivate *>( fugio::fugio()->qobject() );

	if( G == 0 )
	{
		return( -2 );
	}

	QDir	PluginsDir = QDir( qApp->applicationDirPath() );

#if defined( Q_OS_WIN )
	if( PluginsDir.dirName().toLower() == "debug" || PluginsDir.dirName().toLower() == "release" )
	{
		PluginsDir.cdUp();
	}
#elif defined( Q_OS_MAC )
	PluginsDir.cdUp();
	PluginsDir.cdUp();
	PluginsDir.cdUp();

	qDebug() << PluginsDir.absolutePath();
#endif

	PluginsDir.cd( "plugins" );

	int				 R = 0;

	G->loadPlugins( PluginsDir );

	G->initialisePlugins();

	if( !parseCommandLine( G, pApp ) )
	{
		return( -1 );
	}

	QSharedPointer<ContextInterface>	C;

	C = G->newContext();

	if( C )
	{
		if( C->load( mPatchFileName ) )
		{
			G->start();

			R = pApp.exec();

			G->stop();
		}
	}

	G->clear();

	G->unloadPlugins();

	delete G;

	return( R );
}

bool fugio::Show::parseCommandLine( GlobalPrivate *G, QCoreApplication &pApp)
{
	QCommandLineParser		&CLP = G->commandLineParser();

	CLP.addHelpOption();
	CLP.addVersionOption();

	CLP.addPositionalArgument( "patch", "Patch" );

	CLP.process( pApp );

	QStringList args = CLP.positionalArguments();

	if( args.size() > 0 )
	{
		mPatchFileName = args.takeFirst();
	}

	return( !mPatchFileName.isEmpty() );
}


