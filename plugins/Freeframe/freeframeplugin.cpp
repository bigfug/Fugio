#include "freeframeplugin.h"

#include <QDir>
#include <QFileInfoList>
#include <QFileInfo>
#include <QLibrary>
#include <QCryptographicHash>
#include <QSettings>

#include <fugio/global.h>

#include <fugio/freeframe/uuid.h>

#include <FreeFrame.h>

#include "settingswidget.h"

#include "ff10node.h"
#include "ffglnode.h"

QList<QUuid>							fugio::NodeControlBase::PID_UUID;
QMap<QUuid,FreeframeLibrary*>			FreeframePlugin::mPluginMap;

fugio::ClassEntry	NodeClasses[] =
{
	ClassEntry()
};

fugio::ClassEntry PinClasses[] =
{
	ClassEntry()
};

FreeframeLibrary *FreeframePlugin::findPluginInfo( const QUuid &pUuid )
{
	QMap<QUuid,FreeframeLibrary*>::iterator	it = mPluginMap.find( pUuid );

	if( it == mPluginMap.end() )
	{
		return( nullptr );
	}

	return( it.value() );
}

void FreeframePlugin::loadPluginPaths()
{
	QSettings			 Settings;

	Settings.beginGroup( "freeframe" );

	int		PathCount = Settings.beginReadArray( "paths" );

	mPluginPaths.clear();

	for( int i = 0 ; i < PathCount ; i++ )
	{
		Settings.setArrayIndex( i );

		mPluginPaths << Settings.value( "path" ).toString();
	}

	Settings.endArray();

	Settings.endGroup();
}

PluginInterface::InitResult FreeframePlugin::initialise( fugio::GlobalInterface *pApp, bool pLastChance )
{
	Q_UNUSED( pLastChance )

	mApp = pApp;

	fugio::EditorInterface	*EI = qobject_cast<fugio::EditorInterface *>( mApp->findInterface( IID_EDITOR ) );

	if( EI )
	{
		EI->registerSettings( this );
	}

	mApp->registerNodeClasses( NodeClasses );

	mApp->registerPinClasses( PinClasses );

	loadPluginPaths();

	reloadPlugins();

	return( INIT_OK );
}

void FreeframePlugin::deinitialise( void )
{
	fugio::EditorInterface	*EI = qobject_cast<fugio::EditorInterface *>( mApp->findInterface( IID_EDITOR ) );

	if( EI )
	{
		EI->unregisterSettings( this );
	}

	mApp->unregisterPinClasses( PinClasses );

	mApp->unregisterNodeClasses( NodeClasses );

	mApp = 0;
}

void FreeframePlugin::pluginDirScan( QDir pDir )
{
	QFileInfoList		InfLst = pDir.entryInfoList( QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks, QDir::DirsFirst | QDir::Name );

	for( QFileInfo &InfDat : InfLst )
	{
		const QString	FilePath = pDir.absoluteFilePath( InfDat.fileName() );

		if( InfDat.isDir() )
		{
			if( pDir.cd( InfDat.fileName() ) )
			{
				pluginDirScan( pDir );

				pDir.cdUp();
			}

			continue;
		}

		QLibrary		PlgLib( FilePath );

		if( PlgLib.load() )
		{
			pluginProcess( PlgLib );
		}
	}
}

void FreeframePlugin::pluginProcess( QLibrary &pPlgLib )
{
	FF_Main_FuncPtr InitModuleProcFunc = (FF_Main_FuncPtr)pPlgLib.resolve( "plugMain" );

	if( !InitModuleProcFunc )
	{
		return;
	}

	qInfo() << "FreeFrame:" << pPlgLib.fileName();

	FFMixed		PMU;

	PMU.UIntValue = 0;

	PMU = InitModuleProcFunc( FF_GETINFO, PMU, 0 );

	if( PMU.UIntValue == FF_FAIL )
	{
		return;
	}

	PluginInfoStruct	*PluginInfo = static_cast<PluginInfoStruct *>( PMU.PointerValue );

	if( !PluginInfo )
	{
		return;
	}

	QByteArray		IDHash = QCryptographicHash::hash( QByteArray( (const char *)&PluginInfo->PluginUniqueID, 4 ), QCryptographicHash::Md5 );

	ClassEntry		CE;

	CE.mName = QString::fromLatin1( QByteArray( (const char *)PluginInfo->PluginName, 16 ) );
	CE.mUuid = QUuid::fromRfc4122( IDHash );

	if( FreeframeLibrary::testCapability( InitModuleProcFunc, FF_CAP_16BITVIDEO ) ||
		FreeframeLibrary::testCapability( InitModuleProcFunc, FF_CAP_24BITVIDEO ) ||
		FreeframeLibrary::testCapability( InitModuleProcFunc, FF_CAP_32BITVIDEO ) )
	{
		CE.mMetaObject = &FF10Node::staticMetaObject;
		CE.mGroup      = "FreeFrame";
	}
	else
	{
		CE.mMetaObject = &FFGLNode::staticMetaObject;
		CE.mGroup      = "FFGL";
	}

	if( !CE.mUuid.isNull() )
	{
		QString		LibNam = pPlgLib.fileName();

		mPluginMap.insert( CE.mUuid, new FreeframeLibrary( LibNam ) );

		mPluginClasses.append( CE );
	}
}

void FreeframePlugin::reloadPlugins()
{
	mApp->unregisterNodeClasses( mPluginClasses );

	mPluginClasses.clear();

	for( QString Path : mPluginPaths )
	{
		pluginDirScan( QDir( Path ) );
	}

	mApp->registerNodeClasses( mPluginClasses );
}

QWidget *FreeframePlugin::settingsWidget()
{
	SettingsWidget		*SW = new SettingsWidget();

	if( !SW )
	{
		return( nullptr );
	}

	SW->setPluginPaths( mPluginPaths );

	return( SW );
}

void FreeframePlugin::settingsAccept( QWidget *pWidget )
{
	SettingsWidget		*SW = qobject_cast<SettingsWidget *>( pWidget );

	if( !SW )
	{
		return;
	}

	mPluginPaths = SW->pluginPaths();

	QSettings			 Settings;

	Settings.beginGroup( "freeframe" );

	Settings.beginWriteArray( "paths", mPluginPaths.size() );

	for( int i = 0 ; i < mPluginPaths.size() ; i++ )
	{
		Settings.setArrayIndex( i );

		Settings.setValue( "path", mPluginPaths[ i ] );
	}

	Settings.endArray();

	Settings.endGroup();
}
