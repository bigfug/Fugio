#ifndef LUAFONTMETRICS_H
#define LUAFONTMETRICS_H

#if defined( LUA_SUPPORTED )
#include <lua.hpp>
#endif

#include <fugio/lua/lua_interface.h>

#include <QFontMetricsF>

class LuaFontMetrics
{
private:
	typedef struct FontMetricsUserData
	{
		static const char		*TypeName;

		QFont					*mFont;
		QFontMetricsF			*mMetrics;
	} FontMetricsUserData;

public:
	LuaFontMetrics( void ) {}

	virtual ~LuaFontMetrics( void ) {}

#if defined( LUA_SUPPORTED )
	static void registerExtension( fugio::LuaInterface *LUA );

	static int luaOpen( lua_State *L );

	static int luaNew( lua_State *L );

	static int pushfontmetrics( lua_State *L, const QFont &pFont )
	{
		FontMetricsUserData	*UD = (FontMetricsUserData *)lua_newuserdata( L, sizeof( FontMetricsUserData ) );

		if( !UD )
		{
			return( 0 );
		}

		luaL_getmetatable( L, FontMetricsUserData::TypeName );
		lua_setmetatable( L, -2 );

		UD->mFont    = new QFont( pFont );
		UD->mMetrics = new QFontMetricsF( *UD->mFont );

		return( 1 );
	}

	static FontMetricsUserData *checkfontmetricsdata( lua_State *L, int i = 1 )
	{
		void *ud = luaL_checkudata( L, i, FontMetricsUserData::TypeName );

		luaL_argcheck( L, ud != NULL, i, "Font expected" );

		return( static_cast<FontMetricsUserData *>( ud ) );
	}

	static QFontMetricsF *checkfontmetrics( lua_State *L, int i = 1 )
	{
		FontMetricsUserData	*FUD = checkfontmetricsdata( L, i );

		return( FUD ? FUD->mMetrics : nullptr );
	}

private:
	static int luaDelete( lua_State *L );

	static int luaBoundingRect( lua_State *L );

private:
	static const luaL_Reg					mLuaInstance[];
	static const luaL_Reg					mLuaMethods[];
#endif
};

#endif // LUAFONTMETRICS_H
