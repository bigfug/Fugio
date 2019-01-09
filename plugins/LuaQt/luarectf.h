#ifndef LUARECTF_H
#define LUARECTF_H

#if defined( LUA_SUPPORTED )
#include <lua.hpp>
#endif

#include <QRectF>
#include <QUuid>

#include <fugio/lua/lua_interface.h>

class LuaRectF
{
private:
	typedef struct RectFUserData
	{
		static const char *TypeName;

		qreal	x, y, w, h;

		operator QRectF( void ) const
		{
			return( QRectF( x, y, w, h ) );
		}

		RectFUserData *operator = ( const QRectF &R )
		{
			x = R.x();
			y = R.y();
			w = R.width();
			h = R.height();

			return( this );
		}

	} RectFUserData;

public:
	LuaRectF( void ) {}

	~LuaRectF( void ) {}

#if defined( LUA_SUPPORTED )
	static void registerExtension( fugio::LuaInterface *LUA );

	static int luaOpen( lua_State *L );

	static QRectF parseRectF( lua_State *L, int &pNewTop, bool *pRectOk = nullptr );

	static int luaNew( lua_State *L );

	static int pushrectf( lua_State *L, const QRectF &pRect )
	{
		RectFUserData	*UD = (RectFUserData *)lua_newuserdata( L, sizeof( RectFUserData ) );

		if( !UD )
		{
			return( 0 );
		}

		luaL_getmetatable( L, RectFUserData::TypeName );
		lua_setmetatable( L, -2 );

		UD->x = pRect.x();
		UD->y = pRect.y();
		UD->w = pRect.width();
		UD->h = pRect.height();

		return( 1 );
	}

	static bool isRectF( lua_State *L, int i = 1 )
	{
		return( luaL_testudata( L, i, RectFUserData::TypeName ) != nullptr );
	}

	static QRectF checkrectf( lua_State *L, int i = 1 )
	{
		RectFUserData *UD = checkrectfuserdata( L, i );

		return( *UD );
	}

	static int luaPinGet( const QUuid &pPinLocalId, lua_State *L );

private:
	static RectFUserData *checkrectfuserdata( lua_State *L, int i = 1 )
	{
		RectFUserData *UD = (RectFUserData *)luaL_checkudata( L, i, RectFUserData::TypeName );

		luaL_argcheck( L, UD != NULL, i, "Rect expected" );

		return( UD );
	}

	static int luaAdjust( lua_State *L );
	static int luaAdjusted( lua_State *L );
	static int luaCenter( lua_State *L );
	static int luaHeight( lua_State *L );
	static int luaSetHeight( lua_State *L );
	static int luaSetWidth( lua_State *L );
	static int luaSetX( lua_State *L );
	static int luaSetY( lua_State *L );
	static int luaSize( lua_State *L );
	static int luaToArray( lua_State *L );
	static int luaTranslate( lua_State *L );
	static int luaWidth( lua_State *L );
	static int luaX( lua_State *L );
	static int luaY( lua_State *L );

private:
	static const luaL_Reg					mLuaInstance[];
	static const luaL_Reg					mLuaMethods[];
#endif
};

#endif // LUARECTF_H
