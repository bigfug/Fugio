#include "kinectplugin.h"

#include <QtPlugin>

#include <QTranslator>
#include <QApplication>

#include "kinectnode.h"

#include <fugio/kinect/uuid.h>

QList<QUuid>				fugio::NodeControlBase::PID_UUID;

KinectPlugin::KinectPlugin( void )
	: mApp( 0 )
{
	//-------------------------------------------------------------------------
	// Install translator

	static QTranslator		Translator;

	if( Translator.load( QLocale(), QLatin1String( "translations" ), QLatin1String( "_" ), ":/" ) )
	{
		qApp->installTranslator( &Translator );
	}

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
