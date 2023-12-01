#ifndef GAMEPADPLUGIN_H
#define GAMEPADPLUGIN_H

#include <cstdlib>

#include <fugio/global_interface.h>
#include <fugio/plugin_interface.h>

class GamepadPlugin : public QObject, public fugio::PluginInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::PluginInterface )

	Q_PLUGIN_METADATA( IID "com.bigfug.fugio.gamepad.plugin" FILE "manifest.json" )

public:
	Q_INVOKABLE explicit GamepadPlugin( void );

	virtual ~GamepadPlugin( void ) {}

	//-------------------------------------------------------------------------
	// fugio::PluginInterface

	virtual InitResult initialise( fugio::GlobalInterface *pApp, bool pLastChance );

	virtual void deinitialise( void );

private:
	fugio::GlobalInterface			*mApp;

	static const fugio::ClassEntry	 mNodeClasses[];
	static const fugio::ClassEntry	 mPinClasses[];
};


#endif // GAMEPADPLUGIN_H
