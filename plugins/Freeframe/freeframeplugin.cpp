#include "freeframeplugin.h"

#include <QDir>
#include <QFileInfoList>
#include <QFileInfo>
#include <QLibrary>
#include <QCryptographicHash>

#include <fugio/global.h>

#include <fugio/freeframe/uuid.h>

#include <FreeFrame.h>

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

PluginInterface::InitResult FreeframePlugin::initialise( fugio::GlobalInterface *pApp, bool pLastChance )
{
	Q_UNUSED( pLastChance )

	static QDir FFDir( "/Users/bigfug/dev/Freeframe/deploy-release-x86_64" );

	mApp = pApp;

	pluginDirScan( FFDir );

	mApp->registerNodeClasses( NodeClasses );

	mApp->registerNodeClasses( mNodeClasses );

	mApp->registerPinClasses( PinClasses );

	return( INIT_OK );
}

void FreeframePlugin::deinitialise( void )
{
	mApp->unregisterPinClasses( PinClasses );

	mApp->unregisterNodeClasses( mNodeClasses );

	mApp->unregisterNodeClasses( NodeClasses );

	mApp = 0;
}

void FreeframePlugin::pluginDirScan( QDir &pDir )
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

	PMU.UIntValue = FF_CAP_PROCESSOPENGL;

	PMU = InitModuleProcFunc( FF_GETPLUGINCAPS, PMU, NULL );

	if( PMU.UIntValue == FF_SUPPORTED )
	{
		CE.mMetaObject = &FFGLNode::staticMetaObject;
		CE.mGroup      = "FFGL";
	}
	else
	{
		CE.mMetaObject = &FF10Node::staticMetaObject;
		CE.mGroup      = "FreeFrame";
	}

	if( !CE.mUuid.isNull() )
	{
		QString		LibNam = pPlgLib.fileName();

		mPluginMap.insert( CE.mUuid, new FreeframeLibrary( LibNam ) );

		mNodeClasses.append( CE );
	}
}
