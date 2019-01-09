#ifndef LUASIZEF_H
#define LUASIZEF_H

#if defined( LUA_SUPPORTED )
#include <lua.hpp>
#endif

#include <QUuid>

#include <QSizeF>

#include <fugio/lua/lua_interface.h>

class LuaSizeF
{
private:
	typedef struct SizeFUserData
	{
		static const char *TypeName;

		qreal	w, h;

		operator QSizeF( void ) const
		{
			return( QSizeF( w, h ) );
		}

		SizeFUserData *operator = ( const QSizeF &S )
		{
			w = S.width();
			h = S.height();

			return( this );
		}

	} QSizeFUserData;

public:
	LuaSizeF( void ) {}

	virtual ~LuaSizeF( void ) {}

#if defined( LUA_SUPPORTED )
	static void registerExtension( fugio::LuaInterface *LUA );

	static int luaOpen( lua_State *L );

	static int luaNew( lua_State *L );

	static int luaPinGet( const QUuid &pPinLocalId, lua_State *L );

	static int pushsizef( lua_State *L, const QSizeF &pSize )
	{
		SizeFUserData	*UD = (SizeFUserData *)lua_newuserdata( L, sizeof( SizeFUserData ) );

		if( !UD )
		{
			return( 0 );
		}

		luaL_getmetatable( L, SizeFUserData::TypeName );
		lua_setmetatable( L, -2 );

		UD->w = pSize.width();
		UD->h = pSize.height();

		return( 1 );
	}

	static bool isSizeF( lua_State *L, int i = 1 )
	{
		return( luaL_testudata( L, i, SizeFUserData::TypeName ) != nullptr );
	}

	static QSizeF checksizef( lua_State *L, int i = 1 )
	{
		SizeFUserData *UD = checksizefuserdata( L, i );

		return( *UD );
	}

private:
	static SizeFUserData *checksizefuserdata( lua_State *L, int i = 1 )
	{
		SizeFUserData *UD = (SizeFUserData *)luaL_checkudata( L, i, SizeFUserData::TypeName );

		luaL_argcheck( L, UD != NULL, i, "Size expected" );

		return( UD );
	}

	static int luaToString( lua_State *L );

	static int luaSetWidth( lua_State *L );
	static int luaSetHeight( lua_State *L );
	static int luaWidth( lua_State *L );
	static int luaHeight( lua_State *L );

	static int luaToArray( lua_State *L );

	static int luaEq( lua_State *L );

private:
	static const luaL_Reg					mLuaInstance[];
	static const luaL_Reg					mLuaMethods[];
#endif
};

#endif // LUASIZEF_H
