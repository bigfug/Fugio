#ifndef PAINTERPLUGIN_H
#define PAINTERPLUGIN_H

#if defined( LUA_SUPPORTED )
#include <lua.hpp>
#endif

#include <fugio/plugin_interface.h>
#include <fugio/lua/lua_interface.h>

using namespace fugio;

class LuaQtPlugin : public QObject, public PluginInterface
{
	Q_OBJECT
	Q_PLUGIN_METADATA( IID "com.bigfug.fugio.lua-qt.plugin" )
	Q_INTERFACES( fugio::PluginInterface )

public:
	Q_INVOKABLE explicit LuaQtPlugin( void );

	virtual ~LuaQtPlugin( void ) {}

	static LuaQtPlugin *instance( void )
	{
		return( mInstance );
	}

	static LuaInterface *lua( void );

	//-------------------------------------------------------------------------
	// fugio::PluginInterface

	virtual InitResult initialise( fugio::GlobalInterface *pApp, bool pLastChance ) Q_DECL_OVERRIDE;

	virtual void deinitialise( void ) Q_DECL_OVERRIDE;

private:
#if defined( LUA_SUPPORTED )
	static int luaOpen( lua_State *L );
#endif

private:
	static LuaQtPlugin			*mInstance;

	static const luaL_Reg		 mLuaFunctions[];
	static const luaL_Reg		 mLuaMethods[];

	static GlobalInterface		*mApp;
};


#endif // PAINTERPLUGIN_H
