#include "timeplugin.h"

#include <QTranslator>
#include <QCoreApplication>

#include <ctime>

#include <fugio/global_interface.h>
#include <fugio/global_signals.h>

#include <fugio/nodecontrolbase.h>

#include <fugio/time/uuid.h>

#include "clocknode.h"
#include "cronnode.h"
#include "delaynode.h"
#include "everynode.h"
#include "beattapnode.h"
#include "playheadnode.h"
#include "playheadcontrolnode.h"
#include "timenode.h"
#include "datenode.h"
#include "playheadframesnode.h"
#include "universetimenode.h"
#include "datetimetostringnode.h"
#include "localtimenode.h"
#include "splitdatetimenode.h"
#include "utctimenode.h"

#include "inertianode.h"

#include "timepin.h"
#include "datepin.h"
#include "datetimepin.h"

TimePlugin		*TimePlugin::mInstance = nullptr;

QList<QUuid>	NodeControlBase::PID_UUID;

ClassEntry	NodeClasses[] =
{
	ClassEntry( "Beat Tap",			"Time", NID_BEAT_TAP, &BeatTapNode::staticMetaObject ),
	ClassEntry( "Clock",			"Time", NID_CLOCK, &ClockNode::staticMetaObject ),
	ClassEntry( "Cron",				"Time", NID_CRON, &CronNode::staticMetaObject ),
	ClassEntry( "Date",				"Time", NID_DATE, &DateNode::staticMetaObject ),
	ClassEntry( "DateTime To String",	"Time", NID_DATETIMETOSTRING, &DateTimeToStringNode::staticMetaObject ),
	ClassEntry( "Delay",			"Time", NID_TIME_DELAY, &DelayNode::staticMetaObject ),
	ClassEntry( "Every",			"Time", NID_EVERY, &EveryNode::staticMetaObject ),
	ClassEntry( "Inertia",			"Time", NID_INERTIA, &InertiaNode::staticMetaObject ),
	ClassEntry( "Local Time",		"Time", NID_LOCALTIME, &LocalTimeNode::staticMetaObject ),
	ClassEntry( "Playhead",			"Time", NID_PLAYHEAD, &PlayheadNode::staticMetaObject ),
	ClassEntry( "Playhead Control",	"Time", NID_PLAYHEAD_CONTROL, &PlayheadControlNode::staticMetaObject ),
	ClassEntry( "Playhead Frames",	"Time", NID_PLAYHEAD_FRAMES, &PlayheadFramesNode::staticMetaObject ),
	ClassEntry( "Split DateTime",	"Time", NID_SPLIT_DATETIME, &SplitDateTimeNode::staticMetaObject ),
	ClassEntry( "Time",				"Time", NID_TIME, &TimeNode::staticMetaObject ),
	ClassEntry( "Universe Time",	"Time", NID_UNIVERSE_TIME, &UniverseTimeNode::staticMetaObject ),
	ClassEntry( "UTC Time",			"Time", NID_UTCTIME, &UTCTimeNode::staticMetaObject ),
	ClassEntry()
};

ClassEntry PinClasses[] =
{
	ClassEntry( "Date",			"Time", PID_DATE, &DatePin::staticMetaObject ),
	ClassEntry( "DateTime",		"Time", PID_DATETIME, &DateTimePin::staticMetaObject ),
	ClassEntry( "Time",			"Time", PID_TIME, &TimePin::staticMetaObject ),
	ClassEntry()
};

TimePlugin::TimePlugin() : mApp( 0 )
{
	mInstance = this;

	//-------------------------------------------------------------------------
	// Install translator

	static QTranslator		Translator;

	if( Translator.load( QLocale(), QLatin1String( "translations" ), QLatin1String( "_" ), ":/" ) )
	{
		qApp->installTranslator( &Translator );
	}
}

PluginInterface::InitResult TimePlugin::initialise( fugio::GlobalInterface *pApp, bool pLastChance )
{
	Q_UNUSED( pLastChance )

	mApp = pApp;

	mApp->registerNodeClasses( NodeClasses );

	mApp->registerPinClasses( PinClasses );

	mApp->registerPinSplitter( PID_DATETIME, NID_SPLIT_DATETIME );

	mApp->registerPinForMetaType( PID_DATE, QMetaType::QDate );
	mApp->registerPinForMetaType( PID_DATETIME, QMetaType::QDateTime );
	mApp->registerPinForMetaType( PID_TIME, QMetaType::QTime );

	return( INIT_OK );
}

void TimePlugin::deinitialise( void )
{
	mApp->unregisterPinClasses( PinClasses );

	mApp->unregisterNodeClasses( NodeClasses );

	mApp = 0;
}
