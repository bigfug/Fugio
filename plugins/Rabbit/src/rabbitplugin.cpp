#include "rabbitplugin.h"

#include <QTranslator>
#include <QApplication>

#include <fugio/global_signals.h>

#include <fugio/nodecontrolbase.h>

#include "fugio/rabbit/uuid.h"

#include "rabbitnode.h"

using namespace fugio;

QList<QUuid>	NodeControlBase::PID_UUID;

const ClassEntry	RabbitPlugin::mNodeClasses[] =
{
	ClassEntry( "Rabbit", NID_RABBIT, &RabbitNode::staticMetaObject ),
	ClassEntry()
};

const ClassEntry RabbitPlugin::mPinClasses[] =
{
	ClassEntry()
};

RabbitPlugin::RabbitPlugin()
{
	//-------------------------------------------------------------------------
	// Install translator

	static QTranslator		Translator;

	if( Translator.load( QLocale(), QLatin1String( "translations" ), QLatin1String( "_" ), ":/" ) )
	{
		qApp->installTranslator( &Translator );
	}
}

PluginInterface::InitResult RabbitPlugin::initialise( fugio::GlobalInterface *pApp, bool pLastChance )
{
	Q_UNUSED( pLastChance )

	mApp = pApp;

	mApp->registerNodeClasses( mNodeClasses );

	mApp->registerPinClasses( mPinClasses );

	return( INIT_OK );
}

void RabbitPlugin::deinitialise( void )
{
	mApp->unregisterPinClasses( mPinClasses );

	mApp->unregisterNodeClasses( mNodeClasses );

	mApp = Q_NULLPTR;
}
