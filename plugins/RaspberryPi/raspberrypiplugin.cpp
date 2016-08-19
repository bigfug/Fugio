#include "raspberrypiplugin.h"

#include <fugio/global_interface.h>
#include <fugio/global_signals.h>

#include <fugio/nodecontrolbase.h>

#include <fugio/raspberrypi/uuid.h>

#if defined( Q_OS_RASPBERRY_PI )
#include <bcm_host.h>
#endif

QList<QUuid>	NodeControlBase::PID_UUID;

ClassEntry	NodeClasses[] =
{
	ClassEntry()
};

ClassEntry PinClasses[] =
{
	ClassEntry()
};

PluginInterface::InitResult RasperryPiPlugin::initialise( fugio::GlobalInterface *pApp, bool pLastChance )
{
	Q_UNUSED( pLastChance )

	//qDebug() << QString( aiGetLegalString() );

	mApp = pApp;

	mApp->registerNodeClasses( NodeClasses );

	mApp->registerPinClasses( PinClasses );

	//-------------------------------------------------------------------------

	return( INIT_OK );
}

void RasperryPiPlugin::deinitialise( void )
{
	mApp->unregisterPinClasses( PinClasses );

	mApp->unregisterNodeClasses( NodeClasses );

	mApp = 0;
}
