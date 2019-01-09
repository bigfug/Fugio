#ifndef LUAPEN_H
#define LUAPEN_H

#if defined( LUA_SUPPORTED )
#include <lua.hpp>
#endif

#include <QPen>

#include <fugio/lua/lua_interface.h>

class LuaPen
{
private:
	typedef struct PenUserData
	{
		static const char *TypeName;

		QPen		*mPen;
	} FontUserData;

public:
	LuaPen( void ) {}

	virtual ~LuaPen( void ) {}

#if defined( LUA_SUPPORTED )
	static void registerExtension( fugio::LuaInterface *LUA );

	static int luaOpen( lua_State *L );

	static int luaNew( lua_State *L );

	static int pushpen( lua_State *L, const QPen &pPen )
	{
		QPen		*Pen = new QPen( pPen );

		if( !Pen )
		{
			return( 0 );
		}

		PenUserData		*UD = (PenUserData *)lua_newuserdata( L, sizeof( PenUserData ) );

		if( !UD )
		{
			delete Pen;

			return( 0 );
		}

		luaL_getmetatable( L, PenUserData::TypeName );
		lua_setmetatable( L, -2 );

		UD->mPen = Pen;

		return( 1 );
	}

	static bool isPen( lua_State *L, int i = 1 )
	{
		return( luaL_testudata( L, i, PenUserData::TypeName ) != nullptr );
	}

	static PenUserData *checkpendata( lua_State *L, int i = 1 )
	{
		void *ud = luaL_checkudata( L, i, PenUserData::TypeName );

		luaL_argcheck( L, ud != NULL, i, "Pen expected" );

		return( static_cast<PenUserData *>( ud ) );
	}

	static QPen *checkpen( lua_State *L, int i = 1 )
	{
		PenUserData	*UD = checkpendata( L, i );

		return( UD->mPen );
	}

private:
	static int luaDelete( lua_State *L );

	static int luaColor( lua_State *L );
	static int luaSetBrush( lua_State *L );
	static int luaSetColor( lua_State *L );
	static int luaSetWidth( lua_State *L );
	static int luaWidth( lua_State *L );

private:
	static const luaL_Reg					mLuaInstance[];
	static const luaL_Reg					mLuaMethods[];
#endif
};

#endif // LUAPEN_H
