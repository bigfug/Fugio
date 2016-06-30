#include "guiplugin.h"

#include <QtPlugin>

#include <QDebug>

#include "fugio/core/uuid.h"

#include "floatnode.h"
#include "buttonnode.h"
#include "integernode.h"
#include "lcdnumbernode.h"
#include "numbermonitornode.h"
#include "keyboardnode.h"
#include "lednode.h"
#include "slidernode.h"
#include "choicenode.h"
#include "mainwindownode.h"
#include "stringnode.h"

#include "keyboardpin.h"

QList<QUuid>				NodeControlBase::PID_UUID;

ClassEntry		GuiPlugin::mNodeClasses[] =
{
	ClassEntry( "Button", "GUI", NID_BUTTON, &ButtonNode::staticMetaObject ),
	ClassEntry( "Choice", "GUI", NID_CHOICE, &ChoiceNode::staticMetaObject ),
	ClassEntry( "Keyboard", "GUI", NID_KEYBOARD, &KeyboardNode::staticMetaObject ),
	ClassEntry( "LCD Number", "GUI", NID_LCD_NUMBER, &LcdNumberNode::staticMetaObject ),
	ClassEntry( "LED", "GUI", NID_LED, &LedNode::staticMetaObject ),
	ClassEntry( "Main Window", "GUI", NID_MAIN_WINDOW, &MainWindowNode::staticMetaObject ),
	ClassEntry( "Number (Float)", "GUI", NID_FLOAT, &FloatNode::staticMetaObject ),
	ClassEntry( "Number (Integer)", "GUI", NID_INTEGER, &IntegerNode::staticMetaObject ),
	ClassEntry( "Number Monitor", "GUI", NID_NUMBER_MONITOR, &NumberMonitorNode::staticMetaObject ),
	ClassEntry( "Slider", "GUI", NID_SLIDER, &SliderNode::staticMetaObject ),
	ClassEntry( "String", "GUI", NID_STRING, &StringNode::staticMetaObject ),
	ClassEntry()
};

ClassEntry		GuiPlugin::mPinClasses[] =
{
	ClassEntry( "Keyboard", PID_KEYBOARD, &KeyboardPin::staticMetaObject ),
	ClassEntry()
};

GuiPlugin::GuiPlugin( void )
	: mApp( 0 )
{
}

GuiPlugin::~GuiPlugin( void )
{
}

PluginInterface::InitResult GuiPlugin::initialise( fugio::GlobalInterface *pApp )
{
	mApp = pApp;

	//mApp->registerVideoOutputFactory( this );

	mApp->registerNodeClasses( mNodeClasses );

	mApp->registerPinClasses( mPinClasses );

	mApp->registerPinSplitter( PID_SIZE, NID_SPLIT_SIZE );
	//mApp->registerPinSplitter( PID_SIZE_3D, NID_SPLIT_SIZE );
	mApp->registerPinSplitter( PID_LIST, NID_SPLIT_LIST );

	mApp->registerPinJoiner( PID_SIZE, NID_JOIN_SIZE );
	//mApp->registerPinJoiner( PID_SIZE_3D, NID_JOIN_SIZE );

	return( INIT_OK );
}

void GuiPlugin::deinitialise()
{
	mApp->unregisterPinClasses( mPinClasses );

	mApp->unregisterNodeClasses( mNodeClasses );

	mApp = 0;
}
