#include "luapointf.h"

#include <fugio/lua/lua_interface.h>

#include <fugio/core/uuid.h>
#include <fugio/node_interface.h>
#include <fugio/node_control_interface.h>
#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>
#include <fugio/core/variant_interface.h>
#include <fugio/context_interface.h>

#include "luaqtplugin.h"

const char *LuaPointF::mTypeName = "qt.pointf";

#if defined( LUA_SUPPORTED )

const luaL_Reg LuaPointF::mLuaInstance[] =
{
	{ "dotProduct",		LuaPointF::luaDotProduct },
	{ 0, 0 }
};

const luaL_Reg LuaPointF::mLuaMethods[] =
{
	{ "__gc",				LuaPointF::luaDelete },
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

void LuaPointF::registerExtension( LuaInterface *LUA )
{
	LuaQtPlugin::addLuaFunction( "point", LuaPointF::luaNew );

	LUA->luaRegisterExtension( LuaPointF::luaOpen );

	LUA->luaAddPinGet( PID_POINT, LuaPointF::luaPinGet );

	LUA->luaAddPinSet( PID_POINT, LuaPointF::luaPinSet );

	LUA->luaAddPushVariantFunction( QMetaType::QPointF, LuaPointF::pushVariant );

	LUA->luaAddPopVariantFunction( LuaPointF::mTypeName, LuaPointF::popVariant );
}

int LuaPointF::luaOpen(lua_State *L)
{
	luaL_newmetatable( L, LuaPointF::mTypeName );

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
			for( int i = 1 ; i <= 2 ; i++ )
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

int LuaPointF::luaPinSet(const QUuid &pPinLocalId, lua_State *L, int pIndex)
{
	fugio::LuaInterface						*Lua  = LuaQtPlugin::lua();
	NodeInterface							*Node = Lua->node( L );
	QSharedPointer<fugio::PinInterface>		 Pin = Node->findPinByLocalId( pPinLocalId );
	UserData								*UD = checkuserdata( L, pIndex );

	if( !Pin )
	{
		return( luaL_error( L, "No destination pin" ) );
	}

	if( Pin->direction() != PIN_OUTPUT )
	{
		return( luaL_error( L, "No destination pin" ) );
	}

	if( !Pin->hasControl() )
	{
		return( luaL_error( L, "No quaternion pin" ) );
	}

	fugio::VariantInterface			*DstVar = qobject_cast<fugio::VariantInterface *>( Pin->control()->qobject() );

	if( !DstVar )
	{
		return( luaL_error( L, "Can't access quaternion" ) );
	}

	if( UD->mPoint != DstVar->variant().toPointF() )
	{
		DstVar->setVariant( UD->mPoint );

		Pin->node()->context()->pinUpdated( Pin );
	}

	return( 0 );
}

int LuaPointF::luaDelete(lua_State *L)
{
	UserData	*UD = checkuserdata( L );

	static_cast<UserData *>( UD )->mPoint.~QPointF();

	return( 0 );
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
	UserData		*PUD = checkuserdata( L );
	qreal				 i = luaL_checknumber( L, 2 );

	PUD->mPoint.setX( i );

	return( 0 );
}

int LuaPointF::luaSetY( lua_State *L )
{
	UserData		*PUD = checkuserdata( L );
	qreal				 i = luaL_checknumber( L, 2 );

	PUD->mPoint.setY( i );

	return( 0 );
}

int LuaPointF::luaToArray( lua_State *L )
{
	UserData		*PUD = checkuserdata( L );

	lua_newtable( L );

	lua_pushnumber( L, PUD->mPoint.x() );	lua_rawseti( L, -2, 1 );
	lua_pushnumber( L, PUD->mPoint.y() );	lua_rawseti( L, -2, 2 );

	return( 1 );
}

int LuaPointF::luaX( lua_State *L )
{
	UserData		*PUD = checkuserdata( L );

	lua_pushnumber( L, PUD->mPoint.x() );

	return( 1 );
}

int LuaPointF::luaY( lua_State *L )
{
	UserData		*PUD = checkuserdata( L );

	lua_pushnumber( L, PUD->mPoint.y() );

	return( 1 );
}

#endif
