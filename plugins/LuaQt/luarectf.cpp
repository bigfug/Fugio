#include "luarectf.h"

#include "luapointf.h"
#include "luasizef.h"
#include "luapainter.h"

const char *LuaRectF::RectFUserData::TypeName = "qt.rectf";

const luaL_Reg LuaRectF::mLuaInstance[] =
{
	{ 0, 0 }
};

const luaL_Reg LuaRectF::mLuaMethods[] =
{
	//	{ "__tostring", LuaExPin::luaToString },
	{ "adjust",				LuaRectF::luaAdjust },
	{ "adjusted",			LuaRectF::luaAdjusted },
	{ "center",				LuaRectF::luaCenter },
	{ "height",				LuaRectF::luaHeight },
	{ "setHeight",			LuaRectF::luaSetHeight },
	{ "setWidth",			LuaRectF::luaSetWidth },
	{ "setX",				LuaRectF::luaSetX },
	{ "setY",				LuaRectF::luaSetY },
	{ "size",				LuaRectF::luaSize },
	{ "translate",			LuaRectF::luaTranslate },
	{ "width",				LuaRectF::luaWidth },
	{ "x",					LuaRectF::luaX },
	{ "y",					LuaRectF::luaY },
	{ 0, 0 }
};

int LuaRectF::luaOpen( lua_State *L )
{
	luaL_newmetatable( L, RectFUserData::TypeName );

	lua_pushvalue( L, -1 );
	lua_setfield( L, -2, "__index" );

	luaL_setfuncs( L, mLuaMethods, 0 );

	luaL_newlib( L, mLuaInstance );

	return( 1 );
}

int LuaRectF::luaNew( lua_State *L )
{
	int			T = 1;
	QRectF		R = LuaRectF::parseRectF( L, T );

	pushrectf( L, R );

	return( 1 );
}

QRectF LuaRectF::parseRectF( lua_State *L, int &pNewTop, bool *pRectOk )
{
	QRectF			R;
	const int		LuaTop = lua_gettop( L );

	if( LuaRectF::isRectF( L, pNewTop ) )
	{
		R = LuaRectF::checkrectf( L, pNewTop++ );
	}
	else if( LuaPointF::isPointF( L, pNewTop ) )
	{
		R.setTopLeft( LuaPointF::checkpointf( L, pNewTop++ ) );

		if( LuaTop - pNewTop == 0 )
		{
			if( LuaPointF::isPointF( L, pNewTop ) )
			{
				R.setBottomRight( LuaPointF::checkpointf( L, pNewTop++ ) );
			}
			else if( LuaSizeF::isSizeF( L, pNewTop ) )
			{
				R.setSize( LuaSizeF::checksizef( L, pNewTop++ ) );
			}
		}
		else if( LuaTop - pNewTop == 1 )
		{
			R.setWidth( luaL_checknumber( L, pNewTop++ ) );
			R.setHeight( luaL_checknumber( L, pNewTop++ ) );
		}
	}
	else if( LuaTop - pNewTop == 3 )
	{
		R.setX( luaL_checknumber( L, pNewTop++ ) );
		R.setY( luaL_checknumber( L, pNewTop++ ) );
		R.setWidth( luaL_checknumber( L, pNewTop++ ) );
		R.setHeight( luaL_checknumber( L, pNewTop++ ) );
	}

	if( pRectOk )
	{
		*pRectOk = R.isValid();
	}

	return( R );
}

int LuaRectF::luaAdjust( lua_State *L )
{
	RectFUserData		*RUD = checkrectfuserdata( L );
	qreal				 dx1 = luaL_checknumber( L, 2 );
	qreal				 dy1 = luaL_checknumber( L, 3 );
	qreal				 dx2 = luaL_checknumber( L, 4 );
	qreal				 dy2 = luaL_checknumber( L, 5 );

	QRectF				 R( *RUD );

	R.adjust( dx1, dy1, dx2, dy2 );

	*RUD = R;

	return( 0 );
}

int LuaRectF::luaAdjusted( lua_State *L )
{
	RectFUserData		*RUD = checkrectfuserdata( L );
	qreal				 dx1 = luaL_checknumber( L, 2 );
	qreal				 dy1 = luaL_checknumber( L, 3 );
	qreal				 dx2 = luaL_checknumber( L, 4 );
	qreal				 dy2 = luaL_checknumber( L, 5 );

	QRectF				 R( *RUD );

	R.adjust( dx1, dy1, dx2, dy2 );

	pushrectf( L, R );

	return( 1 );
}

int LuaRectF::luaCenter( lua_State *L )
{
	RectFUserData		*RUD = checkrectfuserdata( L );
	QRectF				 R( *RUD );

	LuaPointF::pushpointf( L, R.center() );

	return( 1 );
}

int LuaRectF::luaHeight( lua_State *L )
{
	RectFUserData		*RUD = checkrectfuserdata( L );

	lua_pushnumber( L, RUD->h );

	return( 1 );
}

int LuaRectF::luaSetHeight( lua_State *L )
{
	RectFUserData		*RUD = checkrectfuserdata( L );
	qreal				 i = luaL_checknumber( L, 2 );

	RUD->h = i;

	return( 0 );
}

int LuaRectF::luaSetWidth( lua_State *L )
{
	RectFUserData		*RUD = checkrectfuserdata( L );
	qreal				 i = luaL_checknumber( L, 2 );

	RUD->w = i;

	return( 0 );
}

int LuaRectF::luaSetX( lua_State *L )
{
	RectFUserData		*RUD = checkrectfuserdata( L );
	qreal				 i = luaL_checknumber( L, 2 );

	RUD->x = i;

	return( 0 );
}

int LuaRectF::luaSetY( lua_State *L )
{
	RectFUserData		*RUD = checkrectfuserdata( L );
	qreal				 i = luaL_checknumber( L, 2 );

	RUD->y = i;

	return( 0 );
}

int LuaRectF::luaSize(lua_State *L)
{
	RectFUserData		*RUD = checkrectfuserdata( L );
	QRectF				 R( *RUD );

	LuaSizeF::pushsizef( L, R.size() );

	return( 1 );
}

int LuaRectF::luaTranslate( lua_State *L )
{
	RectFUserData		*RUD = checkrectfuserdata( L );
	qreal				 dx = luaL_checknumber( L, 2 );
	qreal				 dy = luaL_checknumber( L, 3 );

	QRectF				 R( *RUD );

	R.translate( dx, dy );

	*RUD = R;

	return( 0 );
}

int LuaRectF::luaWidth( lua_State *L )
{
	RectFUserData		*RUD = checkrectfuserdata( L );

	lua_pushnumber( L, RUD->w );

	return( 1 );
}

int LuaRectF::luaX( lua_State *L )
{
	RectFUserData		*RUD = checkrectfuserdata( L );

	lua_pushnumber( L, RUD->x );

	return( 1 );
}

int LuaRectF::luaY( lua_State *L )
{
	RectFUserData		*RUD = checkrectfuserdata( L );

	lua_pushnumber( L, RUD->y );

	return( 1 );
}
