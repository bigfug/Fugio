#include "tuioplugin.h"

#include <QtPlugin>

#include <QDebug>

#include <fugio/core/uuid.h>
#include <fugio/tuio/uuid.h>

#include "tuioclientnode.h"

//#include "tuiocursorpin.h"

QList<QUuid>				fugio::NodeControlBase::PID_UUID;

ClassEntry		mNodeClasses[] =
{
	ClassEntry( "Client", "TUIO", NID_TUIO_CLIENT, &TuioClientNode::staticMetaObject ),
	ClassEntry()
};

ClassEntry		mPinClasses[] =
{
	ClassEntry()
};

TuioPlugin::TuioPlugin()
	: mApp( 0 )
{
}

TuioPlugin::~TuioPlugin( void )
{
}

PluginInterface::InitResult TuioPlugin::initialise( fugio::GlobalInterface *pApp, bool pLastChance )
{
	Q_UNUSED( pLastChance )

	mApp = pApp;

	mApp->registerNodeClasses( mNodeClasses );

	mApp->registerPinClasses( mPinClasses );

	return( INIT_OK );
}

void TuioPlugin::deinitialise()
{
	mApp->unregisterPinClasses( mPinClasses );

	mApp->unregisterNodeClasses( mNodeClasses );
}
