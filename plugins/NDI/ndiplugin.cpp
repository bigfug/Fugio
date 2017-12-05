#include "ndiplugin.h"

#include <fugio/global_interface.h>
#include <fugio/global_signals.h>

#include <fugio/ndi/uuid.h>

#include <fugio/nodecontrolbase.h>

#if defined( NDI_SUPPORTED )
#if defined( Q_OS_WIN )
#include <windows.h>
#else
#include <Processing.NDI.compat.h>
#endif

#include <Processing.NDI.Lib.h>
#endif

#include "ndireceivenode.h"
#include "ndisendnode.h"

QList<QUuid>	NodeControlBase::PID_UUID;

ClassEntry	NodeClasses[] =
{
	ClassEntry( "NDI Receive", "NDI", NID_NDI_RECEIVE, &NDIReceiveNode::staticMetaObject ),
	ClassEntry( "NDI Send",    "NDI", NID_NDI_SEND, &NDISendNode::staticMetaObject ),
	ClassEntry()
};

ClassEntry PinClasses[] =
{
	ClassEntry()
};

PluginInterface::InitResult NDIPlugin::initialise( fugio::GlobalInterface *pApp, bool pLastChance )
{
	mApp = pApp;

	mApp->registerNodeClasses( NodeClasses );

	mApp->registerPinClasses( PinClasses );

#if defined( NDI_SUPPORTED )
	if( !NDIlib_initialize() )
	{
		return( INIT_FAILED );
	}
#endif

	return( INIT_OK );
}

void NDIPlugin::deinitialise( void )
{
	mApp->unregisterPinClasses( PinClasses );

	mApp->unregisterNodeClasses( NodeClasses );

	mApp = 0;

#if defined( NDI_SUPPORTED )
	NDIlib_destroy();
#endif
}
