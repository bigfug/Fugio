#include "geometryplugin.h"

#include <QtPlugin>

#include <QDebug>

#include <QTranslator>
#include <QApplication>

#include <fugio/geometry/uuid.h>
#include <fugio/nodecontrolbase.h>

#include "polygonnode.h"
#include "trianglestripnode.h"

QList<QUuid>				NodeControlBase::PID_UUID;

ClassEntry		GeometryPlugin::mNodeClasses[] =
{
	ClassEntry( "Polygon",			"Geometry", NID_POLYGON, &PolygonNode::staticMetaObject ),
	ClassEntry( "Triangle Strip",	"Geometry", NID_TRIANGLE_STRIP, &TriangleStripNode::staticMetaObject ),
	ClassEntry()
};

ClassEntry		GeometryPlugin::mPinClasses[] =
{
	ClassEntry()
};

GeometryPlugin::GeometryPlugin( void )
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

PluginInterface::InitResult GeometryPlugin::initialise( fugio::GlobalInterface *pApp, bool pLastChance )
{
	Q_UNUSED( pLastChance )

	mApp = pApp;

	mApp->registerNodeClasses( mNodeClasses );

	mApp->registerPinClasses( mPinClasses );

	return( INIT_OK );
}

void GeometryPlugin::deinitialise()
{
	mApp->unregisterPinClasses( mPinClasses );

	mApp->unregisterNodeClasses( mNodeClasses );

	mApp = 0;
}
