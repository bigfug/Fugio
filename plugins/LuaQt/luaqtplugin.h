#ifndef PAINTERPLUGIN_H
#define PAINTERPLUGIN_H

#include <QVector>

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

	virtual ~LuaQtPlugin( void ) Q_DECL_OVERRIDE {}

	static LuaQtPlugin *instance( void )
	{
		return( mInstance );
	}

	static LuaInterface *lua( void );

#if defined( LUA_SUPPORTED )
	static void addLuaFunction( const char *pName, lua_CFunction pFunction );

	static void addLuaFunction( luaL_Reg R );
#endif

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

	static QVector<luaL_Reg>	 mLuaFunctions;
	static QVector<luaL_Reg>	 mLuaMethods;

	static GlobalInterface		*mApp;
};


#endif // PAINTERPLUGIN_H
