#include "painterplugin.h"

#include <QtPlugin>

#include <QDebug>

#include <QTranslator>
#include <QApplication>

#include <fugio/core/uuid.h>
#include <fugio/painter/uuid.h>

#include "painternode.h"
#include "circlenode.h"
#include "clearnode.h"
#include "drawimagenode.h"
#include "rectnode.h"
#include "pennode.h"
#include "brushnode.h"
#include "textnode.h"
#include "switchnode.h"
#include "fontmetricsnode.h"
#include "fontnode.h"

#include "fontpin.h"

QList<QUuid>				fugio::NodeControlBase::PID_UUID;

ClassEntry		mNodeClasses[] =
{
	ClassEntry( "Brush", "Painter", NID_PAINTER_BRUSH, &BrushNode::staticMetaObject ),
	ClassEntry( "Draw Circle", "Painter", NID_CIRCLE, &CircleNode::staticMetaObject ),
	ClassEntry( "Draw Image", "Painter", NID_PAINTER_DRAW_IMAGE, &DrawImageNode::staticMetaObject ),
	ClassEntry( "Draw Rect", "Painter", NID_PAINTER_RECT, &RectNode::staticMetaObject ),
	ClassEntry( "Clear", "Painter", NID_PAINTER_CLEAR, &ClearNode::staticMetaObject ),
	ClassEntry( "Font", "GUI", NID_FONT, &FontNode::staticMetaObject ),
	ClassEntry( "Font Metrics", "Painter", NID_FONT_METRICS, &FontMetricsNode::staticMetaObject ),
	ClassEntry( "Painter", "Painter", NID_PAINTER, &PainterNode::staticMetaObject ),
	ClassEntry( "Pen", "Painter", NID_PAINTER_PEN, &PenNode::staticMetaObject ),
	ClassEntry( "Switch", "Painter", NID_PAINTER_SWITCH, &SwitchNode::staticMetaObject ),
	ClassEntry( "Text", "Painter", NID_PAINTER_TEXT, &TextNode::staticMetaObject ),
	ClassEntry()
};

ClassEntry		mPinClasses[] =
{
	ClassEntry( "Font", "Painter", PID_FONT, &FontPin::staticMetaObject ),
	ClassEntry()
};

PainterPlugin::PainterPlugin()
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

PainterPlugin::~PainterPlugin( void )
{
}

PluginInterface::InitResult PainterPlugin::initialise( fugio::GlobalInterface *pApp, bool pLastChance )
{
	Q_UNUSED( pLastChance )

	mApp = pApp;

	mApp->registerNodeClasses( mNodeClasses );

	mApp->registerPinClasses( mPinClasses );

	mApp->registerPinForMetaType( PID_PEN, QMetaType::QPen );
	mApp->registerPinForMetaType( PID_BRUSH, QMetaType::QBrush );
	mApp->registerPinForMetaType( PID_FONT, QMetaType::QFont );

	return( INIT_OK );
}

void PainterPlugin::deinitialise()
{
	mApp->unregisterPinClasses( mPinClasses );

	mApp->unregisterNodeClasses( mNodeClasses );
}
