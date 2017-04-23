#include "oculusriftplugin.h"

#include <QTranslator>
#include <QApplication>

#include <fugio/global_interface.h>
#include <fugio/global_signals.h>

#include <fugio/nodecontrolbase.h>

#include <fugio/oculusrift/uuid.h>

#include <fugio/opengl/uuid.h>

#include "oculusriftnode.h"

QList<QUuid>	NodeControlBase::PID_UUID;

OculusRiftPlugin	*OculusRiftPlugin::mInstance = nullptr;

ClassEntry	NodeClasses[] =
{
	ClassEntry( "Oculus Rift", "", NID_OCULUS_RIFT, &OculusRiftNode::staticMetaObject ),
	ClassEntry()
};

ClassEntry PinClasses[] =
{
	ClassEntry()
};

OculusRiftPlugin::OculusRiftPlugin( void )
	: mApp( 0 )
{
	mInstance = this;

	//-------------------------------------------------------------------------
	// Install translator

	static QTranslator		Translator;

	if( Translator.load( QLocale(), QLatin1String( "fugio_oculusrift" ), QLatin1String( "_" ), ":/translations" ) )
	{
		qApp->installTranslator( &Translator );
	}
}

bool OculusRiftPlugin::hasOpenGLContext()
{
	InterfaceOpenGL		*OGL = qobject_cast<InterfaceOpenGL *>( mApp->findInterface( IID_OPENGL ) );

	if( !OGL || !OGL->hasContext() )
	{
		return( false );
	}

#if defined( OCULUS_PLUGIN_SUPPORTED )
	if( glewExperimental == GL_FALSE )
	{
		glewExperimental = GL_TRUE;

		if( glewInit() != GLEW_OK )
		{
			return( false );
		}
	}
#endif

	return( true );
}

fugio::PluginInterface::InitResult OculusRiftPlugin::initialise( fugio::GlobalInterface *pApp, bool pLastChance )
{
	Q_UNUSED( pLastChance )

	//qDebug() << QString( aiGetLegalString() );

	mApp = pApp;

	mApp->registerNodeClasses( NodeClasses );

	mApp->registerPinClasses( PinClasses );

	//mApp->registerPinSplitter( PID_VECTOR3, NID_SPLIT_VECTOR3 );

#if defined( OCULUS_PLUGIN_SUPPORTED )
	///     - ovrError_Initialize: Generic initialization error.
	///     - ovrError_LibLoad: Couldn't load LibOVRRT.
	///     - ovrError_LibVersion: LibOVRRT version incompatibility.
	///     - ovrError_ServiceConnection: Couldn't connect to the OVR Service.
	///     - ovrError_ServiceVersion: OVR Service version incompatibility.
	///     - ovrError_IncompatibleOS: The operating system version is incompatible.
	///     - ovrError_DisplayInit: Unable to initialize the HMD display.
	///     - ovrError_ServerStart:  Unable to start the server. Is it already running?
	///     - ovrError_Reinitialization: Attempted to re-initialize with a different version.

	ovrResult result = ovr_Initialize( nullptr );

	switch( result )
	{
		case ovrError_Initialize:
			qWarning() << "Generic initialization error";
			return( INIT_OK );

		case ovrError_LibLoad:
			qWarning() << "Couldn't load LibOVRRT";
			return( INIT_OK );

		case ovrError_LibVersion:
			qWarning() << "LibOVRRT version incompatibility";
			return( INIT_OK );

		case ovrError_ServiceConnection:
			qWarning() << "Couldn't connect to the OVR Service";
			return( INIT_OK );

		case ovrError_ServiceVersion:
			qWarning() << "OVR Service version incompatibility";
			return( INIT_OK );

		case ovrError_IncompatibleOS:
			qWarning() << "The operating system version is incompatible";
			return( INIT_OK );

		case ovrError_DisplayInit:
			qWarning() << "Unable to initialize the HMD display";
			return( INIT_OK );

		case ovrError_ServerStart:
			qWarning() << "Unable to start the server. Is it already running?";
			return( INIT_OK );

		case ovrError_Reinitialization:
			qWarning() << "Attempted to re-initialize with a different version.";
			return( INIT_OK );

		case ovrSuccess:
			break;

		default:
			qWarning() << "Error:" << result;
			return( INIT_OK );
	}

	DeviceOculusRift::deviceInitialise();
#else
	qWarning() << "Oculus Rift plugin is not supported";
#endif

	connect( mApp->qobject(), SIGNAL(frameStart(qint64)), this, SLOT(globalFrameStart(qint64)) );

	return( INIT_OK );
}

void OculusRiftPlugin::deinitialise( void )
{
	mApp->unregisterPinClasses( PinClasses );

	mApp->unregisterNodeClasses( NodeClasses );

	DeviceOculusRift::deviceDeinitialise();

#if defined( OCULUS_PLUGIN_SUPPORTED )
	ovr_Shutdown();
#endif

	mApp = 0;
}

void OculusRiftPlugin::globalFrameStart( qint64 pTimeStamp )
{
	DeviceOculusRift::devicePacketStart( pTimeStamp );
}
