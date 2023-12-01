#include "luapen.h"

#include "luaqtplugin.h"

#include "luacolor.h"

#include "luabrush.h"

const char *LuaPen::PenUserData::TypeName = "qt.pen";

#if defined( LUA_SUPPORTED )
const luaL_Reg LuaPen::mLuaInstance[] =
{
	{ 0, 0 }
};

const luaL_Reg LuaPen::mLuaMethods[] =
{
	{ "__gc",				LuaPen::luaDelete },
	{ "color",				LuaPen::luaColor },
	{ "setBrush",			LuaPen::luaSetBrush },
	{ "setColor",			LuaPen::luaSetColor },
	{ "setWidth",			LuaPen::luaSetWidth },
	{ "width",				LuaPen::luaWidth },
	{ 0, 0 }
};

void LuaPen::registerExtension(fugio::LuaInterface *LUA)
{
	LuaQtPlugin::addLuaFunction( "pen", LuaPen::luaNew );

	LUA->luaRegisterExtension( LuaPen::luaOpen );
}

int LuaPen::luaOpen( lua_State *L )
{
	luaL_newmetatable( L, PenUserData::TypeName );

	lua_pushvalue( L, -1 );
	lua_setfield( L, -2, "__index" );

	luaL_setfuncs( L, mLuaMethods, 0 );

	luaL_newlib( L, mLuaInstance );

	return( 1 );
}

int LuaPen::luaNew( lua_State *L )
{
//	QPen(Qt::PenStyle style)
//	QPen(const QColor &color)
//	QPen(const QBrush &brush, qreal width, Qt::PenStyle style = Qt::SolidLine, Qt::PenCapStyle cap = Qt::SquareCap, Qt::PenJoinStyle join = Qt::BevelJoin)
//	QPen(const QPen &pen)

	pushpen( L, QPen() );

	return( 1 );
}

int LuaPen::luaDelete( lua_State *L )
{
	PenUserData		*UD = checkpendata( L );

	if( UD && UD->mPen )
	{
		delete UD->mPen;
	}

	return( 0 );
}

int LuaPen::luaColor( lua_State *L )
{
	QPen		*Pen = checkpen( L );

	LuaColor::pushcolor( L, Pen->color() );

	return( 1 );
}

int LuaPen::luaSetBrush(lua_State *L)
{
	QPen		*Pen = checkpen( L );
	QBrush		*Brush = LuaBrush::checkbrush( L, 2 );

	if( Pen && Brush )
	{
		Pen->setBrush( *Brush );
	}

	return( 0 );
}

int LuaPen::luaSetColor( lua_State *L )
{
	QPen		*Pen = checkpen( L );
	const int	 Top = lua_gettop( L );

	if( Top == 5 || Top == 4 )
	{
		QColor		 Color = LuaColor::popRGBA( L, 2 );

		Pen->setColor( Color );
	}
	else if( Top == 2 )
	{
		if( lua_type( L, 2 ) == LUA_TTABLE )
		{
			QColor		 Color = LuaColor::popTableRGBA( L, 2 );

			Pen->setColor( Color );
		}
		else
		{
			QColor		 Color = LuaColor::checkcolor( L, 2 );

			Pen->setColor( Color );
		}
	}
	else
	{
		luaL_error( L, "Invalid arguments to color" );
	}

	return( 0 );
}

int LuaPen::luaSetWidth( lua_State *L )
{
	QPen		*Pen = checkpen( L );
	qreal		 Wid = luaL_checknumber( L, 2 );

	if( Wid < 0 )
	{
		return( luaL_error( L, "Pen width cannout be less than zero" ) );
	}

	Pen->setWidthF( Wid );

	return( 0 );
}

int LuaPen::luaWidth( lua_State *L )
{
	QPen		*Pen = checkpen( L );

	lua_pushinteger( L, Pen->width() );

	return( 1 );
}

#endif
