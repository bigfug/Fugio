#include "jsonplugin.h"

#include <QTranslator>
#include <QApplication>

#include <fugio/global_interface.h>
#include <fugio/global_signals.h>

#include <fugio/json/uuid.h>

#include <fugio/nodecontrolbase.h>

#include "jsonquerynode.h"
#include "parsejsonnode.h"

#include "jsonpin.h"

using namespace fugio;

QList<QUuid>	NodeControlBase::PID_UUID;

ClassEntry	NodeClasses[] =
{
	ClassEntry( "Query", "JSON", NID_JSON_QUERY, &JsonQueryNode::staticMetaObject ),
	ClassEntry( "Parse", "JSON", NID_PARSE_JSON, &ParseJsonNode::staticMetaObject ),
	ClassEntry()
};

ClassEntry PinClasses[] =
{
	ClassEntry( "JSON", PID_JSON, &JsonPin::staticMetaObject ),
	ClassEntry()
};

JsonPlugin::JsonPlugin()
{
	//-------------------------------------------------------------------------
	// Install translator

	static QTranslator		Translator;

	if( Translator.load( QLocale(), QLatin1String( "translations" ), QLatin1String( "_" ), ":/" ) )
	{
		qApp->installTranslator( &Translator );
	}
}

PluginInterface::InitResult JsonPlugin::initialise( fugio::GlobalInterface *pApp, bool pLastChance )
{
	Q_UNUSED( pLastChance )

	mApp = pApp;

	mApp->registerNodeClasses( NodeClasses );

	mApp->registerPinClasses( PinClasses );

	return( INIT_OK );
}

void JsonPlugin::deinitialise( void )
{
	mApp->unregisterPinClasses( PinClasses );

	mApp->unregisterNodeClasses( NodeClasses );

	mApp = 0;
}
