#include "luaexnode.h"

#include <fugio/global_interface.h>
#include <fugio/core/variant_interface.h>
#include <fugio/lua/lua_interface.h>
#include <fugio/context_interface.h>
#include <fugio/global_signals.h>
#include <fugio/node_interface.h>
#include <fugio/node_signals.h>

#include "luaplugin.h"

using namespace fugio;

#if defined( LUA_SUPPORTED )

const luaL_Reg LuaExNode::mLuaFunctions[] =
{
	{ 0, 0 }
};

const luaL_Reg LuaExNode::mLuaMethods[] =
{
	{ "name", LuaExNode::luaGetName },
	{ 0, 0 }
};

int LuaExNode::luaGetName(lua_State *L)
{
	LuaInterface					*LUA = qobject_cast<LuaInterface *>( LuaPlugin::instance()->app()->findInterface( IID_LUA ) );
	NodeInterface					*N = LUA->node( L );

	lua_pushfstring( L, "%s", N->name().toUtf8().data() );

	return( 1 );
}

//-------------------------------------------------------------------------
// Register fugio.pin

int LuaExNode::lua_opennode( lua_State *L )
{
	luaL_newmetatable( L, "fugio.node" );

	lua_pushvalue( L, -1 );
	lua_setfield( L, -2, "__index" );

	luaL_setfuncs( L, mLuaMethods, 0 );

	luaL_newlib( L, mLuaFunctions );

	return( 1 );
}

#endif
