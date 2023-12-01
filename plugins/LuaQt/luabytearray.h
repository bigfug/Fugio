#ifndef LUABYTEARRAY_H
#define LUABYTEARRAY_H

#if defined( LUA_SUPPORTED )
#include <lua.hpp>
#endif

#include <QByteArray>

#include <fugio/lua/lua_interface.h>

class LuaByteArray
{
private:
	typedef struct UserData
	{
		QByteArray		mByteArray;
	} UserData;

	static const char *mTypeName;

public:
	LuaByteArray() {}

#if defined( LUA_SUPPORTED )
	static void registerExtension( fugio::LuaInterface *LUA );

	static int luaOpen( lua_State *L );

	static int luaNew( lua_State *L );

	static int pushbytearray( lua_State *L, const QByteArray &pQ )
	{
		UserData	*UD = (UserData *)lua_newuserdata( L, sizeof( UserData ) );

		if( !UD )
		{
			return( 0 );
		}

		luaL_getmetatable( L,mTypeName );
		lua_setmetatable( L, -2 );

		new( &UD->mByteArray ) QByteArray( pQ );

		return( 1 );
	}

	static bool isByteArray( lua_State *L, int i = 1 )
	{
		return( luaL_testudata( L, i, mTypeName ) != nullptr );
	}

	static QByteArray checkByteArray( lua_State *L, int i = 1 )
	{
		UserData *UD = checkuserdata( L, i );

		return( UD->mByteArray );
	}

	static int luaPinGet( const QUuid &pPinLocalId, lua_State *L );
	static int luaPinSet( const QUuid &pPinLocalId, lua_State *L, int pIndex );

	static int fromString( lua_State *L );

private:
	static UserData *checkuserdata( lua_State *L, int i = 1 )
	{
		UserData *UD = (UserData *)luaL_checkudata( L, i, mTypeName );

		luaL_argcheck( L, UD != NULL, i, "ByteArray expected" );

		return( UD );
	}

	static int luaIndex( lua_State *L );

private:
	static const luaL_Reg					mLuaFunctions[];
	static const luaL_Reg					mLuaMetaMethods[];
	static const luaL_Reg					mLuaMethods[];
#endif
};


#endif // LUABYTEARRAY_H
