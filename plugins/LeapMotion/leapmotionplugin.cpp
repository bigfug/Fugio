#include "leapmotionplugin.h"

#include <QtPlugin>

#include <QDebug>

#include <QTranslator>
#include <QApplication>

#include <fugio/core/uuid.h>

#if defined( LEAP_PLUGIN_SUPPORTED )

#include <Leap.h>

using namespace Leap;

#endif

#include "leapnode.h"
#include "leaprectifynode.h"
#include "leaphandnode.h"

#include "leaphandpin.h"

QList<QUuid>				fugio::NodeControlBase::PID_UUID;

static QString				GroupName = "Leap Motion";

fugio::ClassEntry		LeapMotionPlugin::mNodeClasses[] =
{
	fugio::ClassEntry( "Image Rectify", GroupName, NID_LEAP_RECTIFY, &LeapRectifyNode::staticMetaObject ),
	fugio::ClassEntry( "Hand", GroupName, NID_LEAP_HAND, &LeapHandNode::staticMetaObject ),
	fugio::ClassEntry( "Device", GroupName, NID_LEAP, &LeapNode::staticMetaObject ),
	fugio::ClassEntry()
};

fugio::ClassEntry		LeapMotionPlugin::mPinClasses[] =
{
	fugio::ClassEntry( "Hand", "Leap Motion", PID_LEAP_HAND, &LeapHandPin::staticMetaObject ),
	fugio::ClassEntry()
};

LeapMotionPlugin::LeapMotionPlugin( void )
	: mApp( 0 )
{
	//-------------------------------------------------------------------------
	// Install translator

	static QTranslator		Translator;

	if( Translator.load( QLocale(), QLatin1String( "translations" ), QLatin1String( "_" ), ":/" ) )
	{
		qApp->installTranslator( &Translator );
	}
}

LeapMotionPlugin::~LeapMotionPlugin( void )
{

}

#if defined( LEAP_PLUGIN_SUPPORTED )
QVector3D LeapMotionPlugin::leapToVec3(const Vector &pSrcVec)
{
	return( QVector3D( pSrcVec.x, pSrcVec.y, pSrcVec.z ) );
}
#endif

fugio::PluginInterface::InitResult LeapMotionPlugin::initialise( fugio::GlobalInterface *pApp, bool pLastChance )
{
	Q_UNUSED( pLastChance )

	mApp = pApp;

	mApp->registerNodeClasses( mNodeClasses );

	mApp->registerPinClasses( mPinClasses );

	return( INIT_OK );
}

void LeapMotionPlugin::deinitialise()
{
	mApp->unregisterPinClasses( mPinClasses );

	mApp->unregisterNodeClasses( mNodeClasses );
}
