#include "kinectplugin.h"

#include <QtPlugin>

#include "kinectnode.h"

#include <fugio/kinect/uuid.h>

QList<QUuid>				fugio::NodeControlBase::PID_UUID;

KinectPlugin::KinectPlugin( void )
	: mApp( 0 )
{
	mNodeEntries.append( fugio::ClassEntry( "Kinect", "Kinect", NID_KINECT, &KinectNode::staticMetaObject ) );
}

KinectPlugin::~KinectPlugin()
{
}

fugio::PluginInterface::InitResult KinectPlugin::initialise( fugio::GlobalInterface *pApp, bool pLastChance )
{
	Q_UNUSED( pLastChance )

	mApp = pApp;

	mApp->registerNodeClasses( mNodeEntries );

	mApp->registerPinClasses( mPinEntries );

	return( INIT_OK );
}

void KinectPlugin::deinitialise()
{
	mApp->unregisterNodeClasses( mNodeEntries );

	mApp->unregisterPinClasses( mPinEntries );
}
