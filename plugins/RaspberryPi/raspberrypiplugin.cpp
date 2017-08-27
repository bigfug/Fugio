#include "raspberrypiplugin.h"

#include <QTranslator>
#include <QCoreApplication>

#include <fugio/global_interface.h>
#include <fugio/global_signals.h>

#include <fugio/nodecontrolbase.h>

#include <fugio/raspberrypi/uuid.h>

#if defined( Q_OS_RASPBERRY_PI )
#include <bcm_host.h>
#include <unistd.h>

#include <IL/OMX_Core.h>
#endif

#if defined( PIGPIO_SUPPORTED )
#include <pigpiod_if2.h>
#endif

#include "sourcenode.h"

QList<QUuid>	NodeControlBase::PID_UUID;

ClassEntry	NodeClasses[] =
{
	ClassEntry( "Source", "OMX", NID_OMX_SOURCE, &SourceNode::staticMetaObject ),
	ClassEntry()
};

ClassEntry PinClasses[] =
{
	ClassEntry()
};

RasperryPiPlugin::RasperryPiPlugin() : mApp( 0 )
{
	//-------------------------------------------------------------------------
	// Install translator

	static QTranslator		Translator;

	if( Translator.load( QLocale(), QLatin1String( "fugio_raspberrypi" ), QLatin1String( "_" ), ":/translations" ) )
	{
		qApp->installTranslator( &Translator );
	}
}

PluginInterface::InitResult RasperryPiPlugin::initialise( fugio::GlobalInterface *pApp, bool pLastChance )
{
	Q_UNUSED( pLastChance )

	//qDebug() << QString( aiGetLegalString() );

	mApp = pApp;

	mApp->registerNodeClasses( NodeClasses );

	mApp->registerPinClasses( PinClasses );

	//-------------------------------------------------------------------------

#if defined( Q_OS_RASPBERRY_PI )
	if( OMX_Init() != OMX_ErrorNone )
	{
		qWarning() << "OMX_Init failed";
	}

#if defined( QT_DEBUG )
	OMX_ERRORTYPE	OMXErr = OMX_ErrorNone;
	char			OMXStr[ 256 ];

	for( int i = 0 ; OMXErr == OMX_ErrorNone ; i++ )
	{
		OMXErr = OMX_ComponentNameEnum( OMXStr, sizeof( OMXStr ), i );

		if( OMXErr == OMX_ErrorNone )
		{
			qDebug() << QString::fromLatin1( OMXStr );
		}
	}
#endif

#endif

#if defined( PIGPIO_SUPPORTED )
	mPigPioInit = pigpio_start( NULL, NULL );

	if( mPigPioInit >= 0 )
	{
#if defined( QT_DEBUG )
		for( unsigned p = 0 ; p <= 53 ; p++ )
		{
			qDebug() << p << get_mode( mPigPioInit, p );
		}
#endif
	}
	else
	{
		qWarning() << "pigpio didn't start - is the daemon running?";
	}
#endif

	return( INIT_OK );
}

void RasperryPiPlugin::deinitialise( void )
{
#if defined( PIGPIO_SUPPORTED )
	if( mPigPioInit >= 0 )
	{
		pigpio_stop( mPigPioInit );
	}
#endif

#if defined( Q_OS_RASPBERRY_PI )
	OMX_Deinit();
#endif

	mApp->unregisterPinClasses( PinClasses );

	mApp->unregisterNodeClasses( NodeClasses );

	mApp = 0;
}
