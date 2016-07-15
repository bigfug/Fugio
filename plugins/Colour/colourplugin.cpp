#include "colourplugin.h"

#include <QtPlugin>

#include <QDebug>

#include <fugio/colour/uuid.h>
#include <fugio/nodecontrolbase.h>

#include "colourbuttonnode.h"
#include "splitcolourhslanode.h"
#include "splitcolourrgbanode.h"
#include "joincolourhslanode.h"
#include "joincolourrgbanode.h"
#include "colourpin.h"

QList<QUuid>				NodeControlBase::PID_UUID;

ClassEntry		ColourPlugin::mNodeClasses[] =
{
	ClassEntry( "Colour", "GUI", NID_COLOUR_BUTTON, &ColourButtonNode::staticMetaObject ),
	ClassEntry( "Split RGBA", "Colour", NID_SPLIT_COLOUR_RGBA, &SplitColourRGBANode::staticMetaObject ),
	ClassEntry( "Split HSLA", "Colour", NID_SPLIT_COLOUR_HSLA, &SplitColourHSLANode::staticMetaObject ),
	ClassEntry( "Join RGBA", "Colour", NID_JOIN_COLOUR_RGBA, &JoinColourRGBANode::staticMetaObject ),
	ClassEntry( "Join HSLA", "Colour", NID_JOIN_COLOUR_HSLA, &JoinColourHSLANode::staticMetaObject ),
	ClassEntry()
};

ClassEntry		ColourPlugin::mPinClasses[] =
{
	ClassEntry( "Colour", PID_COLOUR, &ColourPin::staticMetaObject ),
	ClassEntry()
};

ColourPlugin::ColourPlugin( void )
	: mApp( 0 )
{
}

ColourPlugin::~ColourPlugin( void )
{
}

PluginInterface::InitResult ColourPlugin::initialise( fugio::GlobalInterface *pApp )
{
	mApp = pApp;

	mApp->registerNodeClasses( mNodeClasses );

	mApp->registerPinClasses( mPinClasses );

	mApp->registerPinSplitter( PID_COLOUR, NID_SPLIT_COLOUR_HSLA );
	mApp->registerPinSplitter( PID_COLOUR, NID_SPLIT_COLOUR_RGBA );

	mApp->registerPinJoiner( PID_COLOUR, NID_JOIN_COLOUR_HSLA );
	mApp->registerPinJoiner( PID_COLOUR, NID_JOIN_COLOUR_RGBA );

	return( INIT_OK );
}

void ColourPlugin::deinitialise()
{
	mApp->unregisterPinClasses( mPinClasses );

	mApp->unregisterNodeClasses( mNodeClasses );

	mApp = 0;
}
