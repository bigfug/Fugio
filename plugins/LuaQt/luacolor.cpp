#include "luacolor.h"

#include <fugio/node_interface.h>
#include <fugio/node_control_interface.h>
#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>
#include <fugio/colour/colour_interface.h>
#include <fugio/context_interface.h>

#include "luaqtplugin.h"

const char *LuaColor::ColorUserData::TypeName = "qt.color";

#if defined( LUA_SUPPORTED )

const luaL_Reg LuaColor::mLuaInstance[] =
{
	{ "fromRgba", LuaColor::luaFromRgba },
	{ 0, 0 }
};

const luaL_Reg LuaColor::mLuaMethods[] =
{
	{ "__tostring",	 LuaColor::luaToString },
	{ "alpha",			LuaColor::luaAlpha },
	{ "alphaF",			LuaColor::luaAlphaF },
	{ "blue",			LuaColor::luaBlue },
	{ "blueF",			LuaColor::luaBlueF },
	{ "green",			LuaColor::luaGreen },
	{ "greenF",			LuaColor::luaGreenF },
	{ "hue",			LuaColor::luaHue },
	{ "hueF",			LuaColor::luaHueF },
	{ "lightness",		LuaColor::luaLightness },
	{ "lightnessF",		LuaColor::luaLightnessF },
	{ "red",			LuaColor::luaRed },
	{ "redF",			LuaColor::luaRedF },
	{ "saturation",		LuaColor::luaSaturation },
	{ "saturationF",	LuaColor::luaSaturationF },
	{ "setAlpha",		LuaColor::luaSetAlpha },
	{ "setAlphaF",		LuaColor::luaSetAlphaF },
	{ "setBlue",		LuaColor::luaSetBlue },
	{ "setBlueF",		LuaColor::luaSetBlueF },
	{ "setGreen",		LuaColor::luaSetGreen },
	{ "setGreenF",		LuaColor::luaSetGreenF },
	{ "setRed",			LuaColor::luaSetRed },
	{ "setRedF",		LuaColor::luaSetRedF },
	{ "setHsl",			LuaColor::luaSetHsl },
	{ "setRgb",			LuaColor::luaSetRgb },
	{ "setHslF",		LuaColor::luaSetHslF },
	{ "setRgbF",		LuaColor::luaSetRgbF },
	{ "toArray",		LuaColor::luaToArray },
	{ 0, 0 }
};

void LuaColor::registerExtension( LuaInterface *LUA )
{
	LuaQtPlugin::addLuaFunction( "color", LuaColor::luaNew );

	LUA->luaRegisterExtension( LuaColor::luaOpen );

	LUA->luaAddPinGet( PID_COLOUR, LuaColor::luaPinGet );

//	LUA->luaAddPushVariantFunction( QMetaType::QColor, LuaColor::pushVariant );
}

int LuaColor::luaOpen( lua_State *L )
{
	luaL_newmetatable( L, ColorUserData::TypeName );

	lua_pushvalue( L, -1 );
	lua_setfield( L, -2, "__index" );

	luaL_setfuncs( L, mLuaMethods, 0 );

	luaL_newlib( L, mLuaInstance );

	return( 1 );
}

int LuaColor::luaNew( lua_State *L )
{
	if( lua_gettop( L ) >= 1 )
	{
		luaFromRgba( L );
	}
	else
	{
		pushcolor( L, QColor() );
	}

	return( 1 );
}

int LuaColor::luaPinGet( const QUuid &pPinLocalId, lua_State *L )
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

	fugio::ColourInterface			*SrcCol = qobject_cast<fugio::ColourInterface *>( PinSrc->control()->qobject() );

	if( !SrcCol )
	{
		return( luaL_error( L, "Can't access colour" ) );
	}

	return( pushcolor( L, SrcCol->colour() ) );
}

QColor LuaColor::popRGBA( lua_State *L, int Top )
{
	int			ArgCnt = lua_gettop( L ) - Top;
	int			rgba[ 4 ];

	for( int i = 0 ; i < 4 ; i++ )
	{
		rgba[ i ] = 255;
	}

	for( int i = 0 ; i <= ArgCnt ; i++ )
	{
		rgba[ i ] = luaL_checknumber( L, Top + i );
	}

	for( int i = 0 ; i < 4 ; i++ )
	{
		rgba[ i ] = qBound( 0, rgba[ i ], 255 );
	}

	return( QColor( rgba[ 0 ], rgba[ 1 ], rgba[ 2 ],rgba[ 3 ] ) );
}

QColor LuaColor::popTableRGBA( lua_State *L, int Top )
{
	int			ArgCnt = lua_gettop( L ) - Top;
	int			rgba[ 4 ];

	for( int i = 0 ; i < 4 ; i++ )
	{
		rgba[ i ] = 255;
	}

	for( int i = 0 ; i < ArgCnt ; i++ )
	{
		lua_rawgeti( L, Top, i );

		if( lua_isnil( L, -1 ) )
		{
			lua_pop( L, 1 );

			break;
		}

		rgba[ i ] = lua_tointeger( L, -1 );

		lua_pop( L, 1 );
	}

	for( int i = 0 ; i < 4 ; i++ )
	{
		rgba[ i ] = qBound( 0, rgba[ i ], 255 );
	}

	return( QColor( rgba[ 0 ], rgba[ 1 ], rgba[ 2 ],rgba[ 3 ] ) );
}

int LuaColor::luaToString( lua_State *L )
{
	ColorUserData		*ColorData = checkcolordata( L );
	QColor				 Color( ColorData->mColor );

	lua_pushfstring( L, "%s", Color.name( QColor::HexArgb ).toLatin1().constData() );

	return( 1 );
}

int LuaColor::luaFromRgba( lua_State *L )
{
	int			rgba[ 4 ];

	for( int i = 0 ; i < 4 ; i++ )
	{
		rgba[ i ] = 255;
	}

	luaL_checkany( L, 1 );

	if( lua_type( L, 1 ) == LUA_TNUMBER )
	{
		rgba[ 0 ] = luaL_checknumber( L, 1 );
		rgba[ 1 ] = luaL_checknumber( L, 2 );
		rgba[ 2 ] = luaL_checknumber( L, 3 );
	}

	if( lua_type( L, 1 ) == LUA_TTABLE )
	{
		for( int i = 0 ; i < 3 ; i++ )
		{
			lua_rawgeti( L, 1, i );

			if( lua_isnil( L, -1 ) )
			{
				lua_pop( L, 1 );

				break;
			}

			rgba[ i ] = lua_tointeger( L, -1 );

			lua_pop( L, 1 );
		}
	}

	for( int i = 0 ; i < 4 ; i++ )
	{
		rgba[ i ] = qBound( 0, rgba[ i ], 255 );
	}

	pushcolor( L, QColor::fromRgb( rgba[ 0 ], rgba[ 1 ], rgba[ 2 ], rgba[ 3 ] ) );

	return( 1 );
}

int LuaColor::luaSetHslF(lua_State *L)
{
	ColorUserData		*ColorData = checkcolordata( L );
	QColor				 Color( ColorData->mColor );
	qreal				 hsla[ 4 ];

	hsla[ 0 ] = Color.hueF();
	hsla[ 1 ] = Color.saturationF();
	hsla[ 2 ] = Color.lightnessF();
	hsla[ 3 ] = Color.alphaF();

	luaL_checkany( L, 2 );

	if( lua_type( L, 2 ) == LUA_TNUMBER )
	{
		hsla[ 0 ] = luaL_checknumber( L, 2 );
		hsla[ 1 ] = luaL_checknumber( L, 3 );
		hsla[ 2 ] = luaL_checknumber( L, 4 );

		if( lua_gettop( L ) >= 5 )
		{
			hsla[ 3 ] = luaL_checknumber( L, 5 );
		}
	}
	else if( lua_type( L, 2 ) == LUA_TTABLE )
	{
		for( int i = 0 ; i < 3 ; i++ )
		{
			lua_rawgeti( L, 2, i );

			if( lua_isnil( L, -1 ) )
			{
				lua_pop( L, 1 );

				break;
			}

			hsla[ i ] = lua_tonumberx( L, -1, 0 );

			lua_pop( L, 1 );
		}
	}

	for( int i = 0 ; i < 4 ; i++ )
	{
		hsla[ i ] = qBound( 0.0, hsla[ i ], 1.0 );
	}

	Color.setHslF( hsla[ 0 ], hsla[ 2 ], hsla[ 2 ], hsla[ 3 ] );

	ColorData->mColor = Color.rgba();

	return( 0 );
}

int LuaColor::luaSetRgbF( lua_State *L )
{
	ColorUserData		*ColorData = checkcolordata( L );
	QColor				 Color( ColorData->mColor );
	qreal				 rgba[ 4 ];

	rgba[ 0 ] = Color.redF();
	rgba[ 1 ] = Color.greenF();
	rgba[ 2 ] = Color.blueF();
	rgba[ 3 ] = Color.alphaF();

	luaL_checkany( L, 2 );

	if( lua_type( L, 2 ) == LUA_TNUMBER )
	{
		rgba[ 0 ] = luaL_checknumber( L, 2 );
		rgba[ 1 ] = luaL_checknumber( L, 3 );
		rgba[ 2 ] = luaL_checknumber( L, 4 );

		if( lua_gettop( L ) >= 5 )
		{
			rgba[ 3 ] = luaL_checknumber( L, 5 );
		}
	}
	else if( lua_type( L, 2 ) == LUA_TTABLE )
	{
		for( int i = 0 ; i < 3 ; i++ )
		{
			lua_rawgeti( L, 2, i );

			if( lua_isnil( L, -1 ) )
			{
				lua_pop( L, 1 );

				break;
			}

			rgba[ i ] = lua_tonumberx( L, -1, 0 );

			lua_pop( L, 1 );
		}
	}

	for( int i = 0 ; i < 4 ; i++ )
	{
		rgba[ i ] = qBound( 0.0, rgba[ i ], 1.0 );
	}

	Color.setRgbF( rgba[ 0 ], rgba[ 1 ], rgba[ 2 ], rgba[ 3 ] );

	ColorData->mColor = Color.rgba();

	return( 0 );
}

int LuaColor::luaSetRgb( lua_State *L )
{
	ColorUserData		*ColorData = checkcolordata( L );
	QColor				 Color( ColorData->mColor );
	int					 rgba[ 4 ];

	rgba[ 0 ] = Color.red();
	rgba[ 1 ] = Color.green();
	rgba[ 2 ] = Color.blue();
	rgba[ 3 ] = Color.alpha();

	luaL_checkany( L, 2 );

	if( lua_type( L, 2 ) == LUA_TNUMBER )
	{
		rgba[ 0 ] = luaL_checknumber( L, 2 );
		rgba[ 1 ] = luaL_checknumber( L, 3 );
		rgba[ 2 ] = luaL_checknumber( L, 4 );

		if( lua_gettop( L ) >= 5 )
		{
			rgba[ 3 ] = luaL_checknumber( L, 5 );
		}
	}
	else if( lua_type( L, 2 ) == LUA_TTABLE )
	{
		for( int i = 0 ; i < 3 ; i++ )
		{
			lua_rawgeti( L, 2, i );

			if( lua_isnil( L, -1 ) )
			{
				lua_pop( L, 1 );

				break;
			}

			rgba[ i ] = lua_tointeger( L, -1 );

			lua_pop( L, 1 );
		}
	}

	for( int i = 0 ; i < 4 ; i++ )
	{
		rgba[ i ] = qBound( 0, rgba[ i ], 255 );
	}

	Color.setRed( rgba[ 0 ] );
	Color.setGreen( rgba[ 1 ] );
	Color.setBlue( rgba[ 2 ] );
	Color.setAlpha( rgba[ 3 ] );

	ColorData->mColor = Color.rgba();

	return( 0 );
}

int LuaColor::luaSetHsl( lua_State *L )
{
	ColorUserData		*ColorData = checkcolordata( L );
	QColor				 Color( ColorData->mColor );
	int					 hsla[ 4 ];

	hsla[ 0 ] = Color.hue();
	hsla[ 1 ] = Color.saturation();
	hsla[ 2 ] = Color.lightness();
	hsla[ 3 ] = Color.alpha();

	luaL_checkany( L, 2 );

	if( lua_type( L, 2 ) == LUA_TNUMBER )
	{
		hsla[ 0 ] = luaL_checknumber( L, 2 );
		hsla[ 1 ] = luaL_checknumber( L, 3 );
		hsla[ 2 ] = luaL_checknumber( L, 4 );

		if( lua_gettop( L ) >= 5 )
		{
			hsla[ 3 ] = luaL_checknumber( L, 5 );
		}
	}
	else if( lua_type( L, 2 ) == LUA_TTABLE )
	{
		for( int i = 0 ; i < 3 ; i++ )
		{
			lua_rawgeti( L, 2, i );

			if( lua_isnil( L, -1 ) )
			{
				lua_pop( L, 1 );

				break;
			}

			hsla[ i ] = lua_tointeger( L, -1 );

			lua_pop( L, 1 );
		}
	}

	hsla[ 0 ] = qBound( 0, hsla[ 0 ], 359 );

	for( int i = 1 ; i < 4 ; i++ )
	{
		hsla[ i ] = qBound( 0, hsla[ i ], 255 );
	}

	Color.setHsl( hsla[ 0 ], hsla[ 2 ], hsla[ 2 ], hsla[ 3 ] );

	ColorData->mColor = Color.rgba();

	return( 0 );
}

int LuaColor::luaAlpha(lua_State *L)
{
	ColorUserData		*ColorData = checkcolordata( L );
	QColor				 Color( ColorData->mColor );

	lua_pushinteger( L, Color.alpha() );

	return( 1 );
}

int LuaColor::luaAlphaF(lua_State *L)
{
	ColorUserData		*ColorData = checkcolordata( L );
	QColor				 Color( ColorData->mColor );

	lua_pushnumber( L, Color.alphaF() );

	return( 1 );
}

int LuaColor::luaBlue(lua_State *L)
{
	ColorUserData		*ColorData = checkcolordata( L );
	QColor				 Color( ColorData->mColor );

	lua_pushinteger( L, Color.blue() );

	return( 1 );
}

int LuaColor::luaBlueF(lua_State *L)
{
	ColorUserData		*ColorData = checkcolordata( L );
	QColor				 Color( ColorData->mColor );

	lua_pushnumber( L, Color.blueF() );

	return( 1 );
}

int LuaColor::luaGreen(lua_State *L)
{
	ColorUserData		*ColorData = checkcolordata( L );
	QColor				 Color( ColorData->mColor );

	lua_pushinteger( L, Color.green() );

	return( 1 );
}

int LuaColor::luaGreenF(lua_State *L)
{
	ColorUserData		*ColorData = checkcolordata( L );
	QColor				 Color( ColorData->mColor );

	lua_pushnumber( L, Color.greenF() );

	return( 1 );
}

int LuaColor::luaRed(lua_State *L)
{
	ColorUserData		*ColorData = checkcolordata( L );
	QColor				 Color( ColorData->mColor );

	lua_pushinteger( L, Color.red() );

	return( 1 );
}

int LuaColor::luaRedF(lua_State *L)
{
	ColorUserData		*ColorData = checkcolordata( L );
	QColor				 Color( ColorData->mColor );

	lua_pushnumber( L, Color.redF() );

	return( 1 );
}

int LuaColor::luaHue(lua_State *L)
{
	ColorUserData		*ColorData = checkcolordata( L );
	QColor				 Color( ColorData->mColor );

	lua_pushinteger( L, Color.hue() );

	return( 1 );
}

int LuaColor::luaHueF(lua_State *L)
{
	ColorUserData		*ColorData = checkcolordata( L );
	QColor				 Color( ColorData->mColor );

	lua_pushnumber( L, Color.hueF() );

	return( 1 );
}

int LuaColor::luaSaturation(lua_State *L)
{
	ColorUserData		*ColorData = checkcolordata( L );
	QColor				 Color( ColorData->mColor );

	lua_pushinteger( L, Color.saturation() );

	return( 1 );
}

int LuaColor::luaSaturationF(lua_State *L)
{
	ColorUserData		*ColorData = checkcolordata( L );
	QColor				 Color( ColorData->mColor );

	lua_pushnumber( L, Color.saturationF() );

	return( 1 );
}

int LuaColor::luaLightness(lua_State *L)
{
	ColorUserData		*ColorData = checkcolordata( L );
	QColor				 Color( ColorData->mColor );

	lua_pushinteger( L, Color.lightness() );

	return( 1 );
}

int LuaColor::luaLightnessF(lua_State *L)
{
	ColorUserData		*ColorData = checkcolordata( L );
	QColor				 Color( ColorData->mColor );

	lua_pushnumber( L, Color.lightnessF() );

	return( 1 );
}

int LuaColor::luaSetAlpha(lua_State *L)
{
	ColorUserData		*ColorData = checkcolordata( L );
	QColor				 Color( ColorData->mColor );
	int					 V = luaL_checkinteger( L, 2 );

	luaL_argcheck( L, V >= 0 && V <= 255, 2, "value between 0 and 255 expected" );

	Color.setAlpha( V );

	ColorData->mColor = Color.rgba();

	return( 0 );
}

int LuaColor::luaSetAlphaF(lua_State *L)
{
	ColorUserData		*ColorData = checkcolordata( L );
	QColor				 Color( ColorData->mColor );
	qreal				 V = luaL_checknumber( L, 2 );

	luaL_argcheck( L, V >= 0.0 && V <= 1.0, 2, "value between 0.0 and 1.0 expected" );

	Color.setAlphaF( V );

	ColorData->mColor = Color.rgba();

	return( 0 );
}

int LuaColor::luaSetBlue(lua_State *L)
{
	ColorUserData		*ColorData = checkcolordata( L );
	QColor				 Color( ColorData->mColor );
	int					 V = luaL_checkinteger( L, 2 );

	luaL_argcheck( L, V >= 0 && V <= 255, 2, "value between 0 and 255 expected" );

	Color.setBlue( V );

	ColorData->mColor = Color.rgba();

	return( 0 );
}

int LuaColor::luaSetBlueF(lua_State *L)
{
	ColorUserData		*ColorData = checkcolordata( L );
	QColor				 Color( ColorData->mColor );
	qreal				 V = luaL_checknumber( L, 2 );

	luaL_argcheck( L, V >= 0.0 && V <= 1.0, 2, "value between 0.0 and 1.0 expected" );

	Color.setBlueF( V );

	ColorData->mColor = Color.rgba();

	return( 0 );
}

int LuaColor::luaSetGreen(lua_State *L)
{
	ColorUserData		*ColorData = checkcolordata( L );
	QColor				 Color( ColorData->mColor );
	int					 V = luaL_checkinteger( L, 2 );

	luaL_argcheck( L, V >= 0 && V <= 255, 2, "value between 0 and 255 expected" );

	Color.setGreen( V );

	ColorData->mColor = Color.rgba();

	return( 0 );
}

int LuaColor::luaSetGreenF(lua_State *L)
{
	ColorUserData		*ColorData = checkcolordata( L );
	QColor				 Color( ColorData->mColor );
	qreal				 V = luaL_checknumber( L, 2 );

	luaL_argcheck( L, V >= 0.0 && V <= 1.0, 2, "value between 0.0 and 1.0 expected" );

	Color.setGreenF( V );

	ColorData->mColor = Color.rgba();

	return( 0 );
}

int LuaColor::luaSetRed(lua_State *L)
{
	ColorUserData		*ColorData = checkcolordata( L );
	QColor				 Color( ColorData->mColor );
	int					 V = luaL_checkinteger( L, 2 );

	luaL_argcheck( L, V >= 0 && V <= 255, 2, "value between 0 and 255 expected" );

	Color.setRed( V );

	ColorData->mColor = Color.rgba();

	return( 0 );
}

int LuaColor::luaSetRedF(lua_State *L)
{
	ColorUserData		*ColorData = checkcolordata( L );
	QColor				 Color( ColorData->mColor );
	qreal				 V = luaL_checknumber( L, 2 );

	luaL_argcheck( L, V >= 0.0 && V <= 1.0, 2, "value between 0.0 and 1.0 expected" );

	Color.setRedF( V );

	ColorData->mColor = Color.rgba();

	return( 0 );
}

int LuaColor::luaToArray(lua_State *L)
{
	ColorUserData		*ColorData = checkcolordata( L );
	QColor				 Color( ColorData->mColor );

	lua_newtable( L );

	lua_pushnumber( L, Color.redF() );		lua_rawseti( L, -2, 1 );
	lua_pushnumber( L, Color.greenF() );	lua_rawseti( L, -2, 2 );
	lua_pushnumber( L, Color.blueF() );		lua_rawseti( L, -2, 3 );
	lua_pushnumber( L, Color.alphaF() );	lua_rawseti( L, -2, 4 );

	return( 1 );
}

#endif
