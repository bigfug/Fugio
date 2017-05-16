#include "isfplugin.h"

#include <QtPlugin>

#include <QDebug>

#include <QTranslator>
#include <QApplication>

#include <fugio/isf/uuid.h>
#include <fugio/nodecontrolbase.h>

#include <fugio/opengl/uuid.h>

#include "isfnode.h"

QList<QUuid>				NodeControlBase::PID_UUID;

ISFPlugin					*ISFPlugin::mInstance = nullptr;

ClassEntry		ISFPlugin::mNodeClasses[] =
{
	ClassEntry( "ISF", NID_ISF, &ISFNode::staticMetaObject ),
	ClassEntry()
};

ClassEntry		ISFPlugin::mPinClasses[] =
{
	ClassEntry()
};

ISFPlugin::ISFPlugin( void )
	: mApp( 0 )
{
	//-------------------------------------------------------------------------
	// Install translator

	static QTranslator		Translator;

	if( Translator.load( QLocale(), QLatin1String( "fugio_isf" ), QLatin1String( "_" ), ":/translations" ) )
	{
		qApp->installTranslator( &Translator );
	}
}

bool ISFPlugin::hasContext()
{
	InterfaceOpenGL		*OGL = qobject_cast<InterfaceOpenGL *>( mApp->findInterface( IID_OPENGL ) );

	if( !OGL || !OGL->hasContext() )
	{
		return( false );
	}

#if defined( GLEW_SUPPORTED )
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

bool ISFPlugin::hasContextStatic()
{
	return( instance()->hasContext() );
}

PluginInterface::InitResult ISFPlugin::initialise( fugio::GlobalInterface *pApp, bool pLastChance )
{
	if( !pApp->findInterface( IID_OPENGL ) )
	{
		return( pLastChance ? INIT_FAILED : INIT_DEFER );
	}

	mApp = pApp;

	mApp->registerNodeClasses( mNodeClasses );

	mApp->registerPinClasses( mPinClasses );

	return( INIT_OK );
}

void ISFPlugin::deinitialise()
{
	mApp->unregisterPinClasses( mPinClasses );

	mApp->unregisterNodeClasses( mNodeClasses );

	mApp = 0;
}
