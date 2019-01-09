#include "lualine.h"

#include <fugio/core/uuid.h>
#include <fugio/node_interface.h>
#include <fugio/node_control_interface.h>
#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>
#include <fugio/context_interface.h>
#include <fugio/lua/lua_interface.h>
#include <fugio/core/variant_interface.h>

#include "luaqtplugin.h"

#include "luapointf.h"

const char *LuaLine::mTypeName = "qt.line";

#if defined( LUA_SUPPORTED )

const luaL_Reg LuaLine::mLuaMetaMethods[] =
{
	{ "__index",			LuaLine::luaIndex },
	{ "__newindex",			LuaLine::luaNewIndex },
	{ 0, 0 }
};

const luaL_Reg LuaLine::mLuaMethods[] =
{
	{ "angle",			LuaLine::luaAngle },
	{ "isNull",			LuaLine::luaIsNull },
	{ "length",			LuaLine::luaLength },
	{ "p1",				LuaLine::luaP1 },
	{ "p2",				LuaLine::luaP2 },
	{ "setAngle",		LuaLine::luaSetAngle },
	{ "setLine",		LuaLine::luaSetLine },
	{ "setP1",			LuaLine::luaSetP1 },
	{ "setP2",			LuaLine::luaSetP2 },
	{ "translate",		LuaLine::luaTranslate },
	{ "translated",		LuaLine::luaTranslated },
	{ 0, 0 }
};

void LuaLine::registerExtension( fugio::LuaInterface *LUA )
{
	LuaQtPlugin::addLuaFunction( "line", LuaLine::luaNew );

	LUA->luaRegisterExtension( LuaLine::luaOpen );

	LUA->luaAddPinGet( PID_LINE, LuaLine::luaPinGet );

	LUA->luaAddPinSet( PID_LINE, LuaLine::luaPinSet );

	LUA->luaAddPushVariantFunction( QMetaType::QLineF, LuaLine::pushVariant );

	LUA->luaAddPopVariantFunction( LuaLine::mTypeName, LuaLine::popVariant );
}

int LuaLine::luaOpen (lua_State *L )
{
	luaL_newmetatable( L, mTypeName );

	lua_pushvalue( L, -1 );
	lua_setfield( L, -2, "__index" );

	luaL_setfuncs( L, mLuaMethods, 0 );

	return( 1 );
}

int LuaLine::luaNew( lua_State *L )
{
	if( lua_gettop( L ) <= 0 )
	{
		pushline( L, QLineF() );

		return( 1 );
	}

	if( lua_gettop( L ) <= 2 )
	{
		QPointF		p1 = LuaPointF::checkpointf( L, 1 );
		QPointF		p2 = LuaPointF::checkpointf( L, 2 );

		pushline( L, QLineF( p1, p2 ) );

		return( 1 );
	}

	if( lua_gettop( L ) <= 4 )
	{
		qreal			 x1 = luaL_checknumber( L, 1 );
		qreal			 y1 = luaL_checknumber( L, 2 );
		qreal			 x2 = luaL_checknumber( L, 3 );
		qreal			 y2 = luaL_checknumber( L, 4 );

		pushline( L, QLineF( x1, y1, x2, y2 ) );

		return( 1 );
	}

	return( 0 );
}

int LuaLine::luaNewQt( lua_State *L )
{
	return( luaNew( L ) );
}

int LuaLine::luaIndex( lua_State *L )
{
	const char	*S = luaL_checkstring( L, 2 );

	for( const luaL_Reg *R = mLuaMethods ; R->name ; R++ )
	{
		if( !strcmp( R->name, S ) )
		{
			lua_pushcfunction( L, R->func );

			return( 1 );
		}
	}

	return( luaL_error( L, "unknown field" ) );
}

int LuaLine::luaNewIndex( lua_State *L )
{
//	UserData	*UD = checkuserdata( L );
//	int			 Index  = luaL_checkinteger( L, 2 );

//	luaL_argcheck( L, Index >= 1 && Index <= 16, 2, "index must be 1-16" );

//	lua_Number	 V = luaL_checknumber( L, 3 );

//	Index = Index - 1;

//	UD->mMatrix( Index / 4, Index % 4 ) = V;

	return( 0 );
}

int LuaLine::luaPinGet( const QUuid &pPinLocalId, lua_State *L )
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
		return( luaL_error( L, "No colour pin" ) );
	}

	fugio::VariantInterface			*SrcVar = qobject_cast<fugio::VariantInterface *>( PinSrc->control()->qobject() );

	if( !SrcVar )
	{
		return( luaL_error( L, "Can't access matrix" ) );
	}

	return( pushline( L, SrcVar->variant().value<QLineF>() ) );
}

int LuaLine::luaPinSet( const QUuid &pPinLocalId, lua_State *L, int pIndex )
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
		return( luaL_error( L, "No line pin" ) );
	}

	fugio::VariantInterface			*DstVar = qobject_cast<fugio::VariantInterface *>( Pin->control()->qobject() );

	if( !DstVar )
	{
		return( luaL_error( L, "Can't access quaternion" ) );
	}

	if( UD->mLine != DstVar->variant().value<QLineF>() )
	{
		DstVar->setVariant( UD->mLine );

		Pin->node()->context()->pinUpdated( Pin );
	}

	return( 0 );
}

int LuaLine::luaAngle( lua_State *L )
{
	UserData		*UD = checkuserdata( L );

	lua_pushnumber( L, UD->mLine.angle() );

	return( 1 );
}

int LuaLine::luaIsNull(lua_State *L)
{
	UserData		*UD = checkuserdata( L );

	lua_pushboolean( L, UD->mLine.isNull() );

	return( 1 );
}

int LuaLine::luaLength( lua_State *L )
{
	UserData		*UD = checkuserdata( L );

	lua_pushnumber( L, UD->mLine.length() );

	return( 1 );
}

int LuaLine::luaP1( lua_State *L )
{
	UserData		*UD = checkuserdata( L );

	return( LuaPointF::pushpointf( L, UD->mLine.p1() ) );
}

int LuaLine::luaP2( lua_State *L )
{
	UserData		*UD = checkuserdata( L );

	return( LuaPointF::pushpointf( L, UD->mLine.p2() ) );
}

int LuaLine::luaSetAngle(lua_State *L)
{
	UserData		*UD = checkuserdata( L );

	qreal			 a = luaL_checknumber( L, 2 );

	UD->mLine.setAngle( a );

	return( 0 );
}

int LuaLine::luaSetLine( lua_State *L )
{
	UserData		*UD = checkuserdata( L );
	qreal			 x1 = luaL_checknumber( L, 2 );
	qreal			 y1 = luaL_checknumber( L, 3 );
	qreal			 x2 = luaL_checknumber( L, 4 );
	qreal			 y2 = luaL_checknumber( L, 5 );

	UD->mLine.setLine( x1, y1, x2, y2 );

	return( 0 );
}

int LuaLine::luaSetP1( lua_State *L )
{
	UserData		*UD = checkuserdata( L );

	if( LuaPointF::isPointF( L, 2 ) )
	{
		UD->mLine.setP1( LuaPointF::checkpointf( L, 2 ) );

		return( 0 );
	}

	qreal			 x = luaL_checknumber( L, 2 );
	qreal			 y = luaL_checknumber( L, 3 );

	UD->mLine.setP1( QPointF( x, y ) );

	return( 0 );
}

int LuaLine::luaSetP2( lua_State *L )
{
	UserData		*UD = checkuserdata( L );

	if( LuaPointF::isPointF( L, 2 ) )
	{
		UD->mLine.setP2( LuaPointF::checkpointf( L, 2 ) );

		return( 0 );
	}

	qreal			 x = luaL_checknumber( L, 2 );
	qreal			 y = luaL_checknumber( L, 3 );

	UD->mLine.setP2( QPointF( x, y ) );

	return( 0 );
}

int LuaLine::luaTranslate( lua_State *L )
{
	UserData		*UD = checkuserdata( L );

	if( LuaPointF::isPointF( L, 2 ) )
	{
		UD->mLine.translate( LuaPointF::checkpointf( L, 2 ) );

		return( 0 );
	}

	qreal			 x = luaL_checknumber( L, 2 );
	qreal			 y = luaL_checknumber( L, 3 );

	UD->mLine.translate( QPointF( x, y ) );

	return( 0 );
}

int LuaLine::luaTranslated( lua_State *L )
{
	UserData		*UD = checkuserdata( L );

	if( LuaPointF::isPointF( L, 2 ) )
	{
		pushline( L, UD->mLine.translated( LuaPointF::checkpointf( L, 2 ) ) );

		return( 1 );
	}

	qreal			 x = luaL_checknumber( L, 2 );
	qreal			 y = luaL_checknumber( L, 3 );

	pushline( L, UD->mLine.translated( QPointF( x, y ) ) );

	return( 1 );
}

#endif
