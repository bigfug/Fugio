#ifndef TESTSPLUGIN_H
#define TESTSPLUGIN_H

#if defined( LUA_SUPPORTED )
#include <lua.hpp>
#endif

#include <fugio/plugin_interface.h>
#include <fugio/lua/lua_interface.h>

using namespace fugio;

class ARToolKitPlugin : public QObject, public PluginInterface
{
	Q_OBJECT
	Q_PLUGIN_METADATA( IID "com.bigfug.fugio.artoolkit.plugin" )
	Q_INTERFACES( fugio::PluginInterface )

public:
	Q_INVOKABLE explicit ARToolKitPlugin( void ) { mInstance = this; }

	virtual ~ARToolKitPlugin( void ) {}

	static ARToolKitPlugin *instance( void )
	{
		return( mInstance );
	}

	static LuaInterface *lua( void );

	//-------------------------------------------------------------------------
	// fugio::PluginInterface

	virtual InitResult initialise( fugio::GlobalInterface *pApp, bool pLastChance );

	virtual void deinitialise( void );

private:
#if defined( LUA_SUPPORTED )
	static int luaOpen( lua_State *L );

	static int luaParamLoad( lua_State *L );
#endif

private:
	static ARToolKitPlugin		*mInstance;

	static const luaL_Reg		 mLuaFunctions[];
	static const luaL_Reg		 mLuaMethods[];

	static GlobalInterface		*mApp;
};


#endif // TESTSPLUGIN_H
