#ifndef LUACOLOR_H
#define LUACOLOR_H

#if defined( LUA_SUPPORTED )
#include <lua.hpp>
#endif

#include <QColor>
#include <QUuid>

#include <fugio/lua/lua_interface.h>

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

#if defined( LUA_SUPPORTED )
	static void registerExtension( fugio::LuaInterface *LUA );

	static int luaOpen( lua_State *L );

	static int luaNew( lua_State *L );

	static int luaPinGet( const QUuid &pPinLocalId, lua_State *L );

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

	static int luaSetHsl( lua_State *L );
	static int luaSetRgb( lua_State *L );

	static int luaSetHslF( lua_State *L );
	static int luaSetRgbF( lua_State *L );

	static int luaAlpha( lua_State *L );
	static int luaAlphaF( lua_State *L );

	static int luaBlue( lua_State *L );
	static int luaBlueF( lua_State *L );
	static int luaGreen( lua_State *L );
	static int luaGreenF( lua_State *L );
	static int luaRed( lua_State *L );
	static int luaRedF( lua_State *L );

	static int luaHue( lua_State *L );
	static int luaHueF( lua_State *L );
	static int luaSaturation( lua_State *L );
	static int luaSaturationF( lua_State *L );
	static int luaLightness( lua_State *L );
	static int luaLightnessF( lua_State *L );

	static int luaSetAlpha( lua_State *L );
	static int luaSetAlphaF( lua_State *L );

	static int luaSetBlue( lua_State *L );
	static int luaSetBlueF( lua_State *L );
	static int luaSetGreen( lua_State *L );
	static int luaSetGreenF( lua_State *L );
	static int luaSetRed( lua_State *L );
	static int luaSetRedF( lua_State *L );

	static int luaToArray( lua_State *L );

private:
	static const luaL_Reg					mLuaInstance[];
	static const luaL_Reg					mLuaMethods[];
#endif
};


#endif // LUACOLOR_H
