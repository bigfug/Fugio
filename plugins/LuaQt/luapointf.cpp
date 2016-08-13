#include "luapointf.h"

#include <fugio/lua/lua_interface.h>

#include <fugio/node_interface.h>
#include <fugio/node_control_interface.h>
#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>
#include <fugio/core/variant_interface.h>
#include <fugio/context_interface.h>

#include "luaqtplugin.h"

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
	{ "toArray",			LuaPointF::luaToArray },
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

int LuaPointF::luaNew( lua_State *L )
{
	QPointF		P;

	if( lua_gettop( L ) == 1 )
	{
		if( lua_type( L, 1 ) == LUA_TTABLE )
		{
			for( int i = 1 ; i < 2 ; i++ )
			{
				lua_rawgeti( L, 1, i );

				if( lua_isnil( L, -1 ) )
				{
					lua_pop( L, 1 );

					break;
				}

				if( i == 1 ) P.setX( lua_tonumber( L, -1 ) );
				if( i == 2 ) P.setY( lua_tonumber( L, -1 ) );

				lua_pop( L, 1 );
			}
		}
	}
	else if( lua_gettop( L ) == 2 )
	{
		P.setX( luaL_checknumber( L, 1 ) );
		P.setY( luaL_checknumber( L, 2 ) );
	}

	pushpointf( L, P );

	return( 1 );
}

int LuaPointF::luaPinGet( const QUuid &pPinLocalId, lua_State *L )
{
	fugio::LuaInterface						*Lua  = LuaQtPlugin::lua();
	NodeInterface							*Node = Lua->node( L );
	QSharedPointer<fugio::PinInterface>		 Pin = Node->findPinByLocalId( pPinLocalId );
	QSharedPointer<fugio::PinInterface>		 PinSrc;

	if( !Pin )
	{
		return( luaL_error( L, "No source pin" ) );
	}

	if( Pin->direction() == PIN_OUTPUT )
	{
		PinSrc = Pin;
	}
	else
	{
		PinSrc = Pin->connectedPin();
	}

	if( !PinSrc || !PinSrc->hasControl() )
	{
		return( luaL_error( L, "No point pin" ) );
	}

	fugio::VariantInterface			*SrcVar = qobject_cast<fugio::VariantInterface *>( PinSrc->control()->qobject() );

	if( !SrcVar )
	{
		return( luaL_error( L, "Can't access point" ) );
	}

	return( pushpointf( L, SrcVar->variant().toPointF()) );
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

int LuaPointF::luaToArray( lua_State *L )
{
	PointFUserData		*PUD = checkpointfuserdata( L );

	lua_newtable( L );

	lua_pushnumber( L, PUD->x );	lua_rawseti( L, -2, 1 );
	lua_pushnumber( L, PUD->y );	lua_rawseti( L, -2, 2 );

	return( 1 );
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
