#include "luacolor.h"

#include <fugio/node_interface.h>
#include <fugio/node_control_interface.h>
#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>
#include <fugio/colour/colour_interface.h>
#include <fugio/context_interface.h>

#include "luaqtplugin.h"

const char *LuaColor::ColorUserData::TypeName = "qt.color";

const luaL_Reg LuaColor::mLuaInstance[] =
{
	{ "fromRgba", LuaColor::luaFromRgba },
	{ 0, 0 }
};

const luaL_Reg LuaColor::mLuaMethods[] =
{
	{ "__tostring", LuaColor::luaToString },
	{ "setRgba",	LuaColor::luaSetRgba },
	{ 0, 0 }
};

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

int LuaColor::luaSetRgba( lua_State *L )
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
	}

	if( lua_type( L, 2 ) == LUA_TTABLE )
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
