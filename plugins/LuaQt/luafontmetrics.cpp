#include "luafontmetrics.h"

#include "luafont.h"
#include "luarectf.h"

#include "luaqtplugin.h"

const char *LuaFontMetrics::FontMetricsUserData::TypeName = "qt.fontmetrics";

#if defined( LUA_SUPPORTED )

const luaL_Reg LuaFontMetrics::mLuaInstance[] =
{
	{ 0, 0 }
};

const luaL_Reg LuaFontMetrics::mLuaMethods[] =
{
	{ "__gc",				LuaFontMetrics::luaDelete },
	{ "boundingRect",		LuaFontMetrics::luaBoundingRect },
	{ 0, 0 }
};

void LuaFontMetrics::registerExtension( fugio::LuaInterface *LUA )
{
	LuaQtPlugin::addLuaFunction( "fontmetrics", luaNew );

	LUA->luaRegisterExtension( luaOpen );
}

int LuaFontMetrics::luaOpen( lua_State *L )
{
	luaL_newmetatable( L, FontMetricsUserData::TypeName );

	lua_pushvalue( L, -1 );
	lua_setfield( L, -2, "__index" );

	luaL_setfuncs( L, mLuaMethods, 0 );

	luaL_newlib( L, mLuaInstance );

	return( 1 );
}

int LuaFontMetrics::luaNew( lua_State *L )
{
	const int LuaTop = lua_gettop( L );

	if( LuaTop != 1 )
	{
		luaL_error( L, "Wrong number of arguments" );
	}

	if( !LuaFont::isFont( L, 1 ) )
	{
		luaL_argerror( L, 1, "Font expected" );
	}

	QFont	*Font = LuaFont::checkfont( L, 1 );

	pushfontmetrics( L, *Font );

	return( 1 );
}

int LuaFontMetrics::luaDelete( lua_State *L )
{
	FontMetricsUserData		*MetricsData = checkfontmetricsdata( L );

	if( MetricsData )
	{
		if( MetricsData->mMetrics )
		{
			delete MetricsData->mMetrics;

			MetricsData->mMetrics = 0;
		}

		if( MetricsData->mFont )
		{
			delete MetricsData->mFont;

			MetricsData->mFont = 0;
		}
	}

	return( 0 );
}

int LuaFontMetrics::luaBoundingRect( lua_State *L )
{
	FontMetricsUserData		*MetricsData = checkfontmetricsdata( L );

	const char				*s = luaL_checkstring( L, 2 );
	QString					 S( s ? s : "" );

	QRectF					 R = MetricsData->mMetrics->boundingRect( S );

	LuaRectF::pushrectf( L, R );

	return( 1 );
}

#endif
