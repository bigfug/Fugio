#include "xinputplugin.h"

#include "xinputnode.h"

QList<QUuid>	NodeControlBase::PID_UUID;

ClassEntry	NodeClasses[] =
{
	ClassEntry( "XInput", NID_XINPUT, &XInputNode::staticMetaObject ),
	ClassEntry()
};

XInputPlugin::XInputPlugin()
{

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
