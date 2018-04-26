#include "dlibplugin.h"

#include <QTranslator>
#include <QApplication>

#include <fugio/global_interface.h>
#include <fugio/global_signals.h>

#include <fugio/dlib/uuid.h>

#include <fugio/nodecontrolbase.h>

#if defined( DLIB_SUPPORTED )
#endif

#include "facefeaturesnode.h"

QList<QUuid>	NodeControlBase::PID_UUID;

ClassEntry	NodeClasses[] =
{
	ClassEntry( "Face Features", "dlib", NID_DLIB_FACE_FEATURES, &FaceFeaturesNode::staticMetaObject ),
	ClassEntry()
};

ClassEntry PinClasses[] =
{
	ClassEntry()
};

DlibPlugin::DlibPlugin()
{
	//-------------------------------------------------------------------------
	// Install translator

	static QTranslator		Translator;

	if( Translator.load( QLocale(), QLatin1String( "translations" ), QLatin1String( "_" ), ":/" ) )
	{
		qApp->installTranslator( &Translator );
	}
}

PluginInterface::InitResult DlibPlugin::initialise( fugio::GlobalInterface *pApp, bool pLastChance )
{
	Q_UNUSED( pLastChance )

	mApp = pApp;

	mApp->registerNodeClasses( NodeClasses );

	mApp->registerPinClasses( PinClasses );

	return( INIT_OK );
}

void DlibPlugin::deinitialise( void )
{
	mApp->unregisterPinClasses( PinClasses );

	mApp->unregisterNodeClasses( NodeClasses );

	mApp = 0;
}
