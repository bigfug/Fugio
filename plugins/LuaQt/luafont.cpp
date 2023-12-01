#include "luafont.h"

#include "luaqtplugin.h"

const char *LuaFont::FontUserData::TypeName = "qt.font";

#if defined( LUA_SUPPORTED )

const luaL_Reg LuaFont::mLuaInstance[] =
{
	{ 0, 0 }
};

const luaL_Reg LuaFont::mLuaMethods[] =
{
	{ "__tostring",			LuaFont::luaToString },
	{ "__gc",				LuaFont::luaDelete },
	{ "family",				LuaFont::luaFamily },
	{ "setFamily",			LuaFont::luaSetFamily },
	{ "setPixelSize",		LuaFont::luaSetPixelSize },
	{ "setPointSize",		LuaFont::luaSetPointSize },
	{ 0, 0 }
};

void LuaFont::registerExtension( fugio::LuaInterface *LUA )
{
	LuaQtPlugin::addLuaFunction( "font", LuaFont::luaNew );

	LUA->luaRegisterExtension( LuaFont::luaOpen );
}

int LuaFont::luaOpen( lua_State *L )
{
	luaL_newmetatable( L, FontUserData::TypeName );

	lua_pushvalue( L, -1 );
	lua_setfield( L, -2, "__index" );

	luaL_setfuncs( L, mLuaMethods, 0 );

	luaL_newlib( L, mLuaInstance );

	return( 1 );
}

int LuaFont::luaNew(lua_State *L)
{
	const int LuaTop = lua_gettop( L );

	if( LuaTop == 0 )
	{
		pushfont( L, QFont() );
	}
	else
	{
		const char		*s = luaL_checkstring( L, 1 );
		QString			 FontFace = QString( s ? s : "" );
		int				 PointSize = -1;
		int				 Weight = -1;
		bool			 Italic = false;

		if( LuaTop >= 2 )
		{
			PointSize = luaL_checkinteger( L, 2 );
		}

		if( LuaTop >= 3 )
		{
			Weight = luaL_checkinteger( L, 3 );
		}

		if( LuaTop >= 3 )
		{
			Italic = lua_toboolean( L, 3 );
		}

		if( LuaTop > 3 )
		{
			luaL_argerror( L, 3, "Unknown additional arguments" );
		}

		pushfont( L, QFont( FontFace, PointSize, Weight, Italic ) );
	}

	return( 1 );
}

int LuaFont::luaToString( lua_State *L )
{
	FontUserData		*FontData = checkfontdata( L );

	lua_pushfstring( L, "%s", FontData->mFont->toString().toLatin1().constData() );

	return( 1 );
}

int LuaFont::luaDelete( lua_State *L )
{
	FontUserData		*FontData = checkfontdata( L );

	if( FontData )
	{
		if( FontData->mFont )
		{
			delete FontData->mFont;

			FontData->mFont = 0;
		}
	}

	return( 0 );
}

int LuaFont::luaFamily( lua_State *L )
{
	FontUserData		*FontData = checkfontdata( L );

	lua_pushfstring( L, "%s", FontData->mFont->family().constData()->toLatin1() );

	return( 1 );
}

int LuaFont::luaSetFamily( lua_State *L )
{
	FontUserData		*FontData = checkfontdata( L );
	const char			*Family = luaL_checkstring( L, 2 );

	FontData->mFont->setFamily( QString( Family ) );

	return( 0 );
}

int LuaFont::luaSetPixelSize( lua_State *L )
{
	FontUserData		*FontData = checkfontdata( L );
	int					 Size = luaL_checkinteger( L, 2 );

	FontData->mFont->setPixelSize( Size );

	return( 0 );
}

int LuaFont::luaSetPointSize( lua_State *L )
{
	FontUserData		*FontData = checkfontdata( L );
	int					 Size = luaL_checkinteger( L, 2 );

	FontData->mFont->setPointSize( Size );

	return( 0 );
}

#endif
