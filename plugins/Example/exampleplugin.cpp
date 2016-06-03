#include "exampleplugin.h"

#include <QtPlugin>

#include <QDebug>

#include <fugio/core/uuid.h>
#include <fugio/example/uuid.h>

#include "example1node.h"

QList<QUuid>				fugio::NodeControlBase::PID_UUID;

ClassEntry		mNodeClasses[] =
{
	ClassEntry( "Example1", "Examples", NID_EXAMPLE1, &Example1Node::staticMetaObject ),
	ClassEntry()
};

ClassEntry		mPinClasses[] =
{
	ClassEntry()
};

ExamplePlugin::ExamplePlugin()
	: mApp( 0 )
{
}

ExamplePlugin::~ExamplePlugin( void )
{
}

PluginInterface::InitResult ExamplePlugin::initialise( fugio::GlobalInterface *pApp )
{
	mApp = pApp;

	mApp->registerNodeClasses( mNodeClasses );

	mApp->registerPinClasses( mPinClasses );

	return( INIT_OK );
}

void ExamplePlugin::deinitialise()
{
	mApp->unregisterPinClasses( mPinClasses );

	mApp->unregisterNodeClasses( mNodeClasses );
}
