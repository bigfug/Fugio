#include "gamepadplugin.h"

#include <QTranslator>
#include <QApplication>

#include <fugio/global_signals.h>

#include <fugio/nodecontrolbase.h>

#include <fugio/gamepad/gamepad.h>

#include "gamepadnode.h"

using namespace fugio;

QList<QUuid>	NodeControlBase::PID_UUID;

const ClassEntry	GamepadPlugin::mNodeClasses[] =
{
	ClassEntry( "Gamepad", "Gamepad", NID_GAMEPAD_GAMEPAD, &GamepadNode::staticMetaObject ),
	ClassEntry()
};

const ClassEntry GamepadPlugin::mPinClasses[] =
{
	ClassEntry()
};

GamepadPlugin::GamepadPlugin()
{
	//-------------------------------------------------------------------------
	// Install translator

	static QTranslator		Translator;

	if( Translator.load( QLocale(), QLatin1String( "translations" ), QLatin1String( "_" ), ":/" ) )
	{
		qApp->installTranslator( &Translator );
	}
}

PluginInterface::InitResult GamepadPlugin::initialise( fugio::GlobalInterface *pApp, bool pLastChance )
{
	Q_UNUSED( pLastChance )

	mApp = pApp;

	mApp->registerNodeClasses( mNodeClasses );

	mApp->registerPinClasses( mPinClasses );

	return( INIT_OK );
}

void GamepadPlugin::deinitialise( void )
{
	mApp->unregisterPinClasses( mPinClasses );

	mApp->unregisterNodeClasses( mNodeClasses );

	mApp = Q_NULLPTR;
}
