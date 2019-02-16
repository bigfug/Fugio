#include "pjlinkplugin.h"

#include <QCoreApplication>
#include <QNetworkInterface>
#include <QTranslator>

#include <fugio/global_signals.h>

#include <fugio/nodecontrolbase.h>

#include <fugio/pjlink/uuid.h>

#include "projectornode.h"

QList<QUuid>	NodeControlBase::PID_UUID;

PJLinkPlugin *PJLinkPlugin::mInstance = Q_NULLPTR;

ClassEntry	PJLinkPlugin::mNodeClasses[] =
{
	ClassEntry( "Projector", "PJLink", NID_PJLINK_PROJECTOR, &ProjectorNode::staticMetaObject ),
	ClassEntry()
};

ClassEntry PJLinkPlugin::mPinClasses[] =
{
	ClassEntry()
};

PJLinkPlugin::PJLinkPlugin()
	: mApp( Q_NULLPTR ), mNetworkAccessManager( Q_NULLPTR )
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

PluginInterface::InitResult PJLinkPlugin::initialise( fugio::GlobalInterface *pApp, bool pLastChance )
{
	Q_UNUSED( pLastChance )

	mNetworkAccessManager = new QNetworkAccessManager();

	mApp = pApp;

	mApp->registerNodeClasses( mNodeClasses );

	mApp->registerPinClasses( mPinClasses );

	mServer = new PJLinkServer( this );

	return( INIT_OK );
}

void PJLinkPlugin::deinitialise( void )
{
	mApp->unregisterPinClasses( mPinClasses );

	mApp->unregisterNodeClasses( mNodeClasses );

	mApp = Q_NULLPTR;

	if( mNetworkAccessManager )
	{
		delete mNetworkAccessManager;

		mNetworkAccessManager = Q_NULLPTR;
	}
}

