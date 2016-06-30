#ifndef LUAEXPIN_H
#define LUAEXPIN_H

#if defined( LUA_PLUGIN_SUPPORTED )
#include <lua.hpp>
#endif

#include <QVector>

#include <fugio/lua/lua_interface.h>

class LuaExPin : public fugio::LuaExtension
{
public:
	LuaExPin();

	static int luaPinGetName( lua_State *L );
	static int luaPinGetValue( lua_State *L );
	static int luaPinSetValue( lua_State *L );

	// LuaExtension interface
public:
	static int lua_openpin( lua_State *L );

private:
	static int luaGet( lua_State *L );
	static int luaToString( lua_State *L );
	static int luaUpdated( lua_State *L );
	static int luaIsUpdated( lua_State *L );

private:
	static const luaL_Reg					mLuaPinFunctions[];
	static const luaL_Reg					mLuaInstance[];
	static const luaL_Reg					mLuaPinMethods[];
};

#endif // LUAEXPIN_H
