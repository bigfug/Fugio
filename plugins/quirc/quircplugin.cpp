#include "quircplugin.h"

#include <QtPlugin>

#include <QDebug>

#include <QTranslator>
#include <QApplication>

#include <fugio/core/uuid.h>
#include <fugio/quirc/uuid.h>

#include "qrscannernode.h"

QList<QUuid>				fugio::NodeControlBase::PID_UUID;

ClassEntry		mNodeClasses[] =
{
	ClassEntry( "QR Scanner", "quirc", NID_QUIRC_SCANNER, &QRScannerNode::staticMetaObject ),
	ClassEntry()
};

ClassEntry		mPinClasses[] =
{
	ClassEntry()
};

QuircPlugin::QuircPlugin()
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

QuircPlugin::~QuircPlugin( void )
{
}

PluginInterface::InitResult QuircPlugin::initialise( fugio::GlobalInterface *pApp, bool pLastChance )
{
	Q_UNUSED( pLastChance )

	mApp = pApp;

	mApp->registerNodeClasses( mNodeClasses );

	mApp->registerPinClasses( mPinClasses );

	return( INIT_OK );
}

void QuircPlugin::deinitialise()
{
	mApp->unregisterPinClasses( mPinClasses );

	mApp->unregisterNodeClasses( mNodeClasses );
}
