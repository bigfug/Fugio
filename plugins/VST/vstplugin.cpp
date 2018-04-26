#include "vstplugin.h"

#include <QTranslator>
#include <QCoreApplication>

#include <QLocale>

#include <QStandardPaths>

#include <fugio/core/uuid.h>
#include <fugio/vst/uuid.h>

#include "vst3node.h"
//#include "vst2node.h"

#if defined( VST_SUPPORTED )

//#include <pluginterfaces/vst2.x/aeffect.h>
//#include <pluginterfaces/vst2.x/aeffectx.h>

#if defined( Q_OS_WIN )
#include <windows.h>
#include <conio.h>
extern "C"
{
	typedef bool (PLUGIN_API *InitModuleProc) ();
	typedef bool (PLUGIN_API *ExitModuleProc) ();

//	typedef AEffect *(PLUGIN_API *VSTPluginMain)( audioMasterCallback audioMaster );
}

#elif defined( Q_OS_MAC )
#include <CoreFoundation/CoreFoundation.h>
#include <curses.h>
typedef bool (*bundleEntryPtr)(CFBundleRef);
typedef bool (*bundleExitPtr)(void);

extern "C"
{
	typedef bool (PLUGIN_API *InitModuleProc) ();
	typedef bool (PLUGIN_API *ExitModuleProc) ();

//	typedef AEffect *(PLUGIN_API *VSTPluginMain)( audioMasterCallback audioMaster );
}

#endif

//using namespace Steinberg;

#if defined( VST_SUPPORTED )


#if defined( Q_OS_WIN )
#include <windows.h>
#include <conio.h>
#endif

extern "C"
{
//VstIntPtr audioMaster( AEffect*, VstInt32, VstInt32, VstIntPtr, void*, float )
//{
//	return( 0 );
//}
}
#endif

#endif // defined( VST_SUPPORTED )

QList<QUuid>				 fugio::NodeControlBase::PID_UUID;
QStringList					 VSTPlugin::mLibraryList;
QMap<QUuid,QPair<int,int>>	 VSTPlugin::mPluginMap;

using namespace fugio;

ClassEntry		NodeClasses[] =
{
	ClassEntry()
};

VSTPlugin::VSTPlugin( void )
	: mApp( 0 )
{
	//-------------------------------------------------------------------------
	// Install translator

	static QTranslator		Translator;

	if( Translator.load( QLocale(), QLatin1String( "translations" ), QLatin1String( "_" ), ":/" ) )
	{
		qApp->installTranslator( &Translator );
	}
}

VSTPlugin::~VSTPlugin( void )
{
}

bool VSTPlugin::findPluginInfo( const QUuid &pUuid, QString &pLibNam, int &pClsIdx )
{
	QMap<QUuid,QPair<int,int>>::iterator	it = mPluginMap.find( pUuid );

	if( it == mPluginMap.end() )
	{
		return( false );
	}

	pLibNam = mLibraryList.at( it.value().first );
	pClsIdx = it.value().second;

	return( true );
}

PluginInterface::InitResult VSTPlugin::initialise( GlobalInterface *pApp, bool pLastChance )
{
	Q_UNUSED( pLastChance )

	mApp = pApp;

#if defined( VST_SUPPORTED )

#if defined( Q_OS_WIN )
	QString		ProgramFiles64Path( getenv( "PROGRAMFILES" ) );
	QString		ProgramFiles32Path( getenv( "PROGRAMW6432" ) );

	if( !ProgramFiles64Path.isEmpty() )
	{
		pluginDirScan( QDir( ProgramFiles64Path ).absoluteFilePath( "Common Files/VST3" ) );				// VST3
		//pluginDirScan( QDir( ProgramFiles64Path ).absoluteFilePath( "Steinberg/VstPlugins" ) );			// VST2
	}

	if( !ProgramFiles32Path.isEmpty() && ProgramFiles32Path != ProgramFiles64Path )
	{
		pluginDirScan( QDir( ProgramFiles32Path ).absoluteFilePath( "Common Files/VST3" ) );				// VST3
		//pluginDirScan( QDir( ProgramFiles32Path ).absoluteFilePath( "Steinberg/VstPlugins" ) );			// VST2
	}
#elif defined( Q_OS_MACX )
//	pluginDirScan( QDir( "/Library/Audio/Plug-Ins/VST" ) );
	pluginDirScan( QDir( "/Library/Audio/Plug-Ins/VST3" ) );
	pluginDirScan( QDir( "/Network/Library/Audio/Plug-ins/VST3" ) );
#endif

#endif	// VST_SUPPORTED

	mApp->registerNodeClasses( mNodeClasses );

	return( INIT_OK );
}

void VSTPlugin::deinitialise()
{
	mApp->unregisterNodeClasses( mNodeClasses );
}

void VSTPlugin::pluginDirScan( QDir pDir )
{
	QFileInfoList		InfLst = pDir.entryInfoList( QDir::NoDotAndDotDot | QDir::NoSymLinks | QDir::Dirs | QDir::Files, QDir::DirsFirst | QDir::Name );

	for( QFileInfo &InfDat : InfLst )
	{
		if( InfDat.isDir() )
		{
#if defined( Q_OS_MACX )
			if( InfDat.suffix().toLower() == "vst3" )
			{
				QString		FN = pDir.absoluteFilePath( QString( "%1/Contents/MacOS/%2" ).arg( InfDat.fileName() ).arg( InfDat.baseName() ) );

				QLibrary		PlgLib( FN );

				if( PlgLib.load() )
				{
					pluginProcess3( PlgLib );
				}

				continue;
			}

//			if( InfDat.suffix().toLower() == "vst" )
//			{
//				QString		FN = pDir.absoluteFilePath( QString( "%1/Contents/MacOS/%2" ).arg( InfDat.fileName() ).arg( InfDat.baseName() ) );

//				QLibrary		PlgLib( FN );

//				if( PlgLib.load() )
//				{
//					pluginProcess2( PlgLib );
//				}

//				continue;
//			}
#endif

			if( pDir.cd( InfDat.fileName() ) )
			{
				pluginDirScan( pDir );

				pDir.cdUp();
			}

			continue;
		}

#if 0 //defined( Q_OS_WIN )
		if( InfDat.suffix().toLower() == "vst3" )
#else
		if( InfDat.suffix().toLower() == "vst3" || QLibrary::isLibrary( pDir.absoluteFilePath( InfDat.fileName() ) ) )
#endif
		{
			QLibrary		PlgLib( pDir.absoluteFilePath( InfDat.fileName() ) );

			if( PlgLib.load() )
			{
				qDebug() << pDir.absoluteFilePath( InfDat.fileName() );

				if( !pluginProcess3( PlgLib ) )
				{
					//pluginProcess2( PlgLib );
				}
			}
		}
	}
}

#if 0
bool VSTPlugin::pluginProcess2( QLibrary &pPlgLib )
{
#if !defined( VST_SUPPORTED )
	Q_UNUSED( pPlgLib )
#else
	VSTPluginMain	PluginMain = (VSTPluginMain)pPlgLib.resolve( "VSTPluginMain" );

	if( !PluginMain )
	{
		return( false );
	}

	AEffect			*Effect = PluginMain( &VST2Node::audioMaster );

	if( !Effect )
	{
		return( false );
	}

	if( Effect->magic != kEffectMagic )
	{
		return( false );
	}

	QFileInfo		FI( pPlgLib.fileName() );

	qDebug() << FI.baseName() << Effect->version << Effect->numInputs << Effect->numOutputs;

	static QUuid	VST2_UUID = QUuid( "{0fbd04b3-bbe6-481b-aee3-b93f84416955}" );

	ClassEntry		CE;

	CE.mMetaObject = &VST2Node::staticMetaObject;
	CE.mName       = FI.baseName();
	CE.mGroup      = "VST2";
	CE.mUuid       = VST2_UUID;
	CE.mFlags      = ClassEntry::None;

	CE.mUuid.data1 = Effect->uniqueID;

	if( !CE.mUuid.isNull() )
	{
		QString		LibNam = pPlgLib.fileName();

		if( !mLibraryList.contains( LibNam ) )
		{
			mLibraryList.append( LibNam );
		}

		mPluginMap.insert( CE.mUuid, QPair<int,int>( mLibraryList.indexOf( LibNam ), 0 ) );

		mNodeClasses.append( CE );
	}

	return( true );
#endif

	return( false );
}

#endif

bool VSTPlugin::pluginProcess3( QLibrary &pPlgLib )
{
#if defined( VST_SUPPORTED )
#if defined( Q_OS_WIN )
	InitModuleProc InitModuleProcFunc = (InitModuleProc)pPlgLib.resolve( "InitDll" );

	if( InitModuleProcFunc && !InitModuleProcFunc() )
	{
		return( false );
	}
#endif

	GetFactoryProc GetFactoryProcFunc = (GetFactoryProc)pPlgLib.resolve( "GetPluginFactory" );

	if( !GetFactoryProcFunc )
	{
		return( false );
	}

	Steinberg::IPluginFactory				*mPluginFactory;

	if( !( mPluginFactory = GetFactoryProcFunc() ) )
	{
		return( false );
	}

	Steinberg::PFactoryInfo					 mPluginFactoryInfo;

	mPluginFactory->getFactoryInfo( &mPluginFactoryInfo );

	for( int i = 0 ; i < mPluginFactory->countClasses() ; i++ )
	{
		Steinberg::PClassInfo	ClassInfo;

		if( mPluginFactory->getClassInfo( i, &ClassInfo ) != Steinberg::kResultOk )
		{
			continue;
		}

		if( strcmp( ClassInfo.category, kVstAudioEffectClass ) != 0 )
		{
			continue;
		}

		Steinberg::char8 cidString[50];
		Steinberg::FUID (ClassInfo.cid).toRegistryString (cidString);

		//qDebug() << ClassInfo.category << ClassInfo.name << cidString;

		ClassEntry		CE;

		CE.mMetaObject = &VST3Node::staticMetaObject;
		CE.mName       = QString( ClassInfo.name );
		CE.mGroup      = "VST3";
		CE.mUuid       = QUuid( cidString );
		CE.mFlags      = ClassEntry::None;

		if( !CE.mUuid.isNull() )
		{
			QString		LibNam = pPlgLib.fileName();

			if( !mLibraryList.contains( LibNam ) )
			{
				mLibraryList.append( LibNam );
			}

			mPluginMap.insert( CE.mUuid, QPair<int,int>( mLibraryList.indexOf( LibNam ), i ) );

			mNodeClasses.append( CE );
		}
	}

#if defined( Q_OS_WIN )
	ExitModuleProc ExitModuleProcFunc = (ExitModuleProc)pPlgLib.resolve( "ExitDll" );

	if( ExitModuleProcFunc )
	{
		ExitModuleProcFunc();
	}
#endif
	return( true );
#else
	return( false );
#endif
}
