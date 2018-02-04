#include "firmataplugin.h"

#include <QtPlugin>

#include <QDebug>

#include <QTranslator>
#include <QApplication>

#include <fugio/core/uuid.h>

#include <fugio/firmata/uuid.h>

#include "firmatanode.h"

QList<QUuid>				fugio::NodeControlBase::PID_UUID;

ClassEntry		mNodeClasses[] =
{
	ClassEntry( "Firmata", "Firmata", NID_FIRMATA, &FirmataNode::staticMetaObject ),
	ClassEntry()
};

ClassEntry		mPinClasses[] =
{
	ClassEntry()
};

FirmataPlugin::FirmataPlugin() : mApp( 0 )
{
	//-------------------------------------------------------------------------
	// Install translator

	static QTranslator		Translator;

	if( Translator.load( QLocale(), QLatin1String( "translations" ), QLatin1String( "_" ), ":/" ) )
	{
		qApp->installTranslator( &Translator );
	}
}

PluginInterface::InitResult FirmataPlugin::initialise( fugio::GlobalInterface *pApp, bool pLastChance )
{
	Q_UNUSED( pLastChance )

	mApp = pApp;

	mApp->registerNodeClasses( mNodeClasses );

	mApp->registerPinClasses( mPinClasses );

	return( INIT_OK );
}

void FirmataPlugin::deinitialise()
{
	mApp->unregisterPinClasses( mPinClasses );

	mApp->unregisterNodeClasses( mNodeClasses );
}
