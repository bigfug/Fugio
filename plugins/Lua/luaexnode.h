#ifndef LUAEXNODE_H
#define LUAEXNODE_H

#if defined( LUA_SUPPORTED )
#include <lua.hpp>
#endif

#include <QVector>

#include <fugio/lua/lua_interface.h>

class LuaExNode
{
public:
	LuaExNode() {}

#if defined( LUA_SUPPORTED )
	static int luaGetName( lua_State *L );

	// LuaExtension interface
public:
	static int lua_opennode( lua_State *L );

private:
	static const luaL_Reg					mLuaFunctions[];
	static const luaL_Reg					mLuaMethods[];
#endif
};

#endif // LUAEXNODE_H
