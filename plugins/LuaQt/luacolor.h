#ifndef LUACOLOR_H
#define LUACOLOR_H

#if defined( LUA_SUPPORTED )
#include <lua.hpp>
#endif

#include <QColor>

class LuaColor
{
private:
	typedef struct ColorUserData
	{
		static const char *TypeName;

		QRgb		mColor;
	} ColorUserData;

public:
	LuaColor( void ) {}

	virtual ~LuaColor( void ) {}

	static int luaOpen( lua_State *L );

	static int luaNew( lua_State *L );

	static int pushcolor( lua_State *L, const QColor &pColor )
	{
		ColorUserData	*UD = (ColorUserData *)lua_newuserdata( L, sizeof( ColorUserData ) );

		if( !UD )
		{
			return( 0 );
		}

		luaL_getmetatable( L, ColorUserData::TypeName );
		lua_setmetatable( L, -2 );

		UD->mColor = pColor.rgba();

		return( 1 );
	}

	static QColor checkcolor( lua_State *L, int i = 1 )
	{
		ColorUserData		*ud = checkcolordata( L, i );

		return( QColor( ud->mColor ) );
	}

	static ColorUserData *checkcolordata( lua_State *L, int i = 1 )
	{
		void *ud = luaL_checkudata( L, i, ColorUserData::TypeName );

		luaL_argcheck( L, ud != NULL, i, "Color expected" );

		return( static_cast<ColorUserData *>( ud ) );
	}

	static bool isColor( lua_State *L, int i = 1 )
	{
		return( luaL_testudata( L, i, ColorUserData::TypeName ) != nullptr );
	}

	static QColor popRGBA( lua_State *L, int Top = 1 );
	static QColor popTableRGBA( lua_State *L, int Top = 1 );

private:
	static int luaToString( lua_State *L );

	static int luaFromRgba( lua_State *L );

	static int luaSetRgba( lua_State *L );

private:
	static const luaL_Reg					mLuaInstance[];
	static const luaL_Reg					mLuaMethods[];
};


#endif // LUACOLOR_H
