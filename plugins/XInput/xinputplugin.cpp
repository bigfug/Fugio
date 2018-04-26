#include "xinputplugin.h"

#include <QTranslator>
#include <QCoreApplication>

#include "xinputnode.h"

QList<QUuid>	NodeControlBase::PID_UUID;

ClassEntry	NodeClasses[] =
{
	ClassEntry( "XInput", NID_XINPUT, &XInputNode::staticMetaObject ),
	ClassEntry()
};

XInputPlugin::XInputPlugin()
{
	//-------------------------------------------------------------------------
	// Install translator

	static QTranslator		Translator;

	if( Translator.load( QLocale(), QLatin1String( "translations" ), QLatin1String( "_" ), ":/" ) )
	{
		qApp->installTranslator( &Translator );
	}
}

XInputPlugin::~XInputPlugin()
{

}

PluginInterface::InitResult XInputPlugin::initialise( GlobalInterface *pApp, bool pLastChance )
{
	Q_UNUSED( pLastChance )

	mApp = pApp;

	mApp->registerNodeClasses( NodeClasses );

	//connect( mApp->global(), SIGNAL(frameStart(qint64)), this, SLOT(onGlobalFrameStart(qint64)) );

	return( INIT_OK );
}

void XInputPlugin::deinitialise()
{
	mApp->unregisterNodeClasses( NodeClasses );

	mApp = 0;
}

void XInputPlugin::onGlobalFrameStart( qint64 pTimeStamp )
{
	Q_UNUSED( pTimeStamp )


}
