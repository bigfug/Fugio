#include "syphonplugin.h"

#include <QTimer>

#include <fugio/global_interface.h>
#include <fugio/global_signals.h>

#include <fugio/nodecontrolbase.h>

#include <fugio/syphon/uuid.h>
#include <fugio/opengl/uuid.h>

#include "syphonreceivernode.h"
#include "syphonsendernode.h"
#include "syphonrendernode.h"

#include "syphonpin.h"

SyphonPlugin	*SyphonPlugin::mInstance = 0;

QList<QUuid>	NodeControlBase::PID_UUID;

// Syphon Sender doesn't work!

ClassEntry	NodeClasses[] =
{
	ClassEntry( "Syphon Receiver", "Syphon", NID_SYPHON_RECEIVER, &SyphonReceiverNode::staticMetaObject ),
//	ClassEntry( "Syphon Sender", "Syphon", NID_SYPHON_SENDER, &SyphonSenderNode::staticMetaObject ),
	ClassEntry( "Syphon Render", "Syphon", NID_SYPHON_RENDER, &SyphonRenderNode::staticMetaObject ),
	ClassEntry()
};

ClassEntry PinClasses[] =
{
	ClassEntry( "Syphon", PID_SYPHON, &SyphonPin::staticMetaObject ),
	ClassEntry()
};

SyphonPlugin::SyphonPlugin() : mApp( 0 )
{
	mInstance = this;

#if defined( SYPHON_SUPPORTED )
	glewExperimental = GL_FALSE;
#endif
}

fugio::PluginInterface::InitResult SyphonPlugin::initialise( fugio::GlobalInterface *pApp, bool pLastChance )
{
	Q_UNUSED( pLastChance )

	mApp = pApp;

	mApp->registerNodeClasses( NodeClasses );

	mApp->registerPinClasses( PinClasses );

	return( INIT_OK );
}

void SyphonPlugin::deinitialise( void )
{
	mApp->unregisterPinClasses( PinClasses );

	mApp->unregisterNodeClasses( NodeClasses );

	mApp = 0;
}

bool SyphonPlugin::hasOpenGLContext()
{
	InterfaceOpenGL		*OGL = qobject_cast<InterfaceOpenGL *>( mApp->findInterface( IID_OPENGL ) );

	if( !OGL || !OGL->hasContext() )
	{
		return( false );
	}

#if !defined( Q_OS_RASPBERRY_PI )
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

