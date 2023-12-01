#ifndef LUAFONT_H
#define LUAFONT_H

#if defined( LUA_SUPPORTED )
#include <lua.hpp>
#endif

#include <fugio/lua/lua_interface.h>

#include <QFont>

class LuaFont
{
private:
	typedef struct FontUserData
	{
		static const char *TypeName;

		QFont		*mFont;
	} FontUserData;

public:
	LuaFont( void ) {}

	virtual ~LuaFont( void ) {}

#if defined( LUA_SUPPORTED )
	static void registerExtension( fugio::LuaInterface *LUA );

	static int luaOpen( lua_State *L );

	static int luaNew( lua_State *L );

	static int pushfont( lua_State *L, const QFont &pFont )
	{
		FontUserData	*UD = (FontUserData *)lua_newuserdata( L, sizeof( FontUserData ) );

		if( !UD )
		{
			return( 0 );
		}

		luaL_getmetatable( L, FontUserData::TypeName );
		lua_setmetatable( L, -2 );

		UD->mFont = new QFont( pFont );

		return( 1 );
	}

	static bool isFont( lua_State *L, int i = 1 )
	{
		return( luaL_testudata( L, i, FontUserData::TypeName ) != nullptr );
	}

	static FontUserData *checkfontdata( lua_State *L, int i = 1 )
	{
		void *ud = luaL_checkudata( L, i, FontUserData::TypeName );

		luaL_argcheck( L, ud != NULL, i, "Font expected" );

		return( static_cast<FontUserData *>( ud ) );
	}

	static QFont *checkfont( lua_State *L, int i = 1 )
	{
		FontUserData	*FUD = checkfontdata( L, i );

		return( FUD ? FUD->mFont : nullptr );
	}

private:
	static int luaToString( lua_State *L );
	static int luaDelete( lua_State *L );

	static int luaFamily( lua_State *L );
	static int luaSetFamily( lua_State *L );

	static int luaSetPixelSize( lua_State *L );
	static int luaSetPointSize( lua_State *L );

private:
	static const luaL_Reg					mLuaInstance[];
	static const luaL_Reg					mLuaMethods[];
#endif
};

#endif // LUAFONT_H
