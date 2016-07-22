#include "luapointf.h"

const char *LuaPointF::PointFUserData::TypeName = "qt.pointf";

#if defined( LUA_SUPPORTED )

const luaL_Reg LuaPointF::mLuaInstance[] =
{
	{ "dotProduct",		LuaPointF::luaDotProduct },
	{ 0, 0 }
};

const luaL_Reg LuaPointF::mLuaMethods[] =
{
	{ "__add",				LuaPointF::luaAdd },
	{ "__div",				LuaPointF::luaDiv },
	{ "__eq",				LuaPointF::luaEq },
	{ "__mul",				LuaPointF::luaMul },
	{ "__sub",				LuaPointF::luaSub },
	{ "isNull",				LuaPointF::luaIsNull },
	{ "manhattanLength",	LuaPointF::luaManhattanLength },
	{ "setX",				LuaPointF::luaSetX },
	{ "setY",				LuaPointF::luaSetY },
	{ "x",					LuaPointF::luaX },
	{ "y",					LuaPointF::luaY },
	{ 0, 0 }
};

int LuaPointF::luaOpen(lua_State *L)
{
	luaL_newmetatable( L, PointFUserData::TypeName );

	lua_pushvalue( L, -1 );
	lua_setfield( L, -2, "__index" );

	luaL_setfuncs( L, mLuaMethods, 0 );

	luaL_newlib( L, mLuaInstance );

	return( 1 );
}

int LuaPointF::luaNew(lua_State *L)
{
	if( lua_gettop( L ) == 2 )
	{
		float		x = luaL_checknumber( L, 1 );
		float		y = luaL_checknumber( L, 2 );

		pushpointf( L, QPointF( x, y ) );
	}
	else
	{
		pushpointf( L, QPointF() );
	}

	return( 1 );
}

int LuaPointF::luaDotProduct( lua_State *L )
{
	QPointF		p1 = checkpointf( L, 1 );
	QPointF		p2 = checkpointf( L, 2 );

	lua_pushnumber( L, QPointF::dotProduct( p1, p2 ) );

	return( 1 );
}

int LuaPointF::luaAdd( lua_State *L )
{
	QPointF		P1 = checkpointf( L, 1 );
	QPointF		P2 = checkpointf( L, 2 );

	pushpointf( L, P1 + P2 );

	return( 1 );
}

int LuaPointF::luaDiv(lua_State *L)
{
	QPointF		P = checkpointf( L, 1 );
	qreal		R = luaL_checknumber( L, 2 );

	pushpointf( L, P / R );

	return( 1 );
}

int LuaPointF::luaEq( lua_State *L )
{
	QPointF		P1 = checkpointf( L, 1 );
	QPointF		P2 = checkpointf( L, 2 );

	lua_pushboolean( L, P1 == P2 );

	return( 1 );
}

int LuaPointF::luaMul(lua_State *L)
{
	QPointF		P = checkpointf( L, 1 );
	qreal		R = luaL_checknumber( L, 2 );

	pushpointf( L, P * R );

	return( 1 );
}

int LuaPointF::luaSub( lua_State *L )
{
	QPointF		P1 = checkpointf( L, 1 );
	QPointF		P2 = checkpointf( L, 2 );

	pushpointf( L, P1 - P2 );

	return( 1 );
}

int LuaPointF::luaIsNull( lua_State *L )
{
	QPointF		P = checkpointf( L );

	lua_pushboolean( L, P.isNull() );

	return( 1 );
}

int LuaPointF::luaManhattanLength(lua_State *L)
{
	QPointF		P = checkpointf( L );

	lua_pushnumber( L, P.manhattanLength() );

	return( 1 );
}

int LuaPointF::luaSetX( lua_State *L )
{
	PointFUserData		*PUD = checkpointfuserdata( L );
	qreal				 i = luaL_checknumber( L, 2 );

	PUD->x = i;

	return( 0 );
}

int LuaPointF::luaSetY( lua_State *L )
{
	PointFUserData		*PUD = checkpointfuserdata( L );
	qreal				 i = luaL_checknumber( L, 2 );

	PUD->y = i;

	return( 0 );
}

int LuaPointF::luaX( lua_State *L )
{
	PointFUserData		*PUD = checkpointfuserdata( L );

	lua_pushnumber( L, PUD->x );

	return( 1 );
}

int LuaPointF::luaY( lua_State *L )
{
	PointFUserData		*PUD = checkpointfuserdata( L );

	lua_pushnumber( L, PUD->y );

	return( 1 );
}

#endif
