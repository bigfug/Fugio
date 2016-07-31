#include "luasizef.h"

const char *LuaSizeF::SizeFUserData::TypeName = "qt.sizef";

#if defined( LUA_SUPPORTED )

const luaL_Reg LuaSizeF::mLuaInstance[] =
{
	{ 0, 0 }
};

const luaL_Reg LuaSizeF::mLuaMethods[] =
{
	{ "__tostring",			LuaSizeF::luaToString },
	{ "setWidth",			LuaSizeF::luaSetWidth },
	{ "setHeight",			LuaSizeF::luaSetHeight },
	{ "toArray",			LuaSizeF::luaToArray },
	{ "width",				LuaSizeF::luaWidth },
	{ "height",				LuaSizeF::luaHeight },
	{ 0, 0 }
};

int LuaSizeF::luaOpen(lua_State *L)
{
	luaL_newmetatable( L, SizeFUserData::TypeName );

	lua_pushvalue( L, -1 );
	lua_setfield( L, -2, "__index" );

	luaL_setfuncs( L, mLuaMethods, 0 );

	luaL_newlib( L, mLuaInstance );

	return( 1 );
}

int LuaSizeF::luaNew( lua_State *L )
{
	if( lua_gettop( L ) == 2 )
	{
		float		w = luaL_checknumber( L, 1 );
		float		h = luaL_checknumber( L, 2 );

		pushsizef( L, QSizeF( w, h ) );
	}
	else
	{
		pushsizef( L, QSizeF() );
	}

	return( 1 );
}

int LuaSizeF::luaToString( lua_State *L )
{
	SizeFUserData		*SUD = checksizefuserdata( L );

	lua_pushfstring( L, "%f,%f", SUD->w, SUD->h );

	return( 1 );
}

int LuaSizeF::luaSetWidth( lua_State *L )
{
	SizeFUserData		*SUD = checksizefuserdata( L );
	qreal				 i = luaL_checknumber( L, 2 );

	SUD->w = i;

	return( 0 );
}

int LuaSizeF::luaSetHeight( lua_State *L )
{
	SizeFUserData		*SUD = checksizefuserdata( L );
	qreal				 i = luaL_checknumber( L, 2 );

	SUD->h = i;

	return( 0 );
}

int LuaSizeF::luaWidth( lua_State *L )
{
	SizeFUserData		*SUD = checksizefuserdata( L );

	lua_pushnumber( L, SUD->w );

	return( 1 );
}

int LuaSizeF::luaHeight( lua_State *L )
{
	SizeFUserData		*SUD = checksizefuserdata( L );

	lua_pushnumber( L, SUD->h );

	return( 1 );
}

int LuaSizeF::luaToArray( lua_State *L )
{
	SizeFUserData		*SUD = checksizefuserdata( L );

	lua_newtable( L );

	lua_pushnumber( L, SUD->w );	lua_rawseti( L, -2, 1 );
	lua_pushnumber( L, SUD->h );	lua_rawseti( L, -2, 2 );

	return( 1 );
}

#endif
