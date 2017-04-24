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

	mApp = pApp;

	mApp->registerNodeClasses( NodeClasses );

	mApp->registerPinClasses( PinClasses );

	DeviceOculusRift::deviceInitialise();

	connect( mApp->qobject(), SIGNAL(frameStart(qint64)), this, SLOT(globalFrameStart(qint64)) );

	return( INIT_OK );
}

void OculusRiftPlugin::deinitialise( void )
{
	mApp->unregisterPinClasses( PinClasses );

	mApp->unregisterNodeClasses( NodeClasses );

	DeviceOculusRift::deviceDeinitialise();

	mApp = 0;
}

void OculusRiftPlugin::globalFrameStart( qint64 pTimeStamp )
{
	DeviceOculusRift::devicePacketStart( pTimeStamp );
}
