#ifndef LUAJSONARRAY_H
#define LUAJSONARRAY_H

#if defined( LUA_SUPPORTED )
#include <lua.hpp>
#endif

#include <QJsonArray>

#include <fugio/lua/lua_interface.h>

class LuaJsonArray
{
private:
	static const char		*mTypeName;

	typedef struct UserData
	{
		QJsonArray				 mJsonArray;
		QJsonArray::iterator	 mIterator;

	} UserData;

public:
	LuaJsonArray( void ) {}

	virtual ~LuaJsonArray( void ) {}

#if defined( LUA_SUPPORTED )
	static void registerExtension( fugio::LuaInterface *LUA );

	static int luaOpen( lua_State *L );

	static int luaNew( lua_State *L );

	static int pushjsonarray( lua_State *L, const QJsonArray &pJsonArray )
	{
		UserData	*UD = (UserData *)lua_newuserdata( L, sizeof( UserData ) );

		if( !UD )
		{
			return( 0 );
		}

		luaL_getmetatable( L, mTypeName );
		lua_setmetatable( L, -2 );

		new( &UD->mJsonArray ) QJsonArray( pJsonArray );
		new( &UD->mIterator )  QJsonArray::iterator();

		return( 1 );
	}

	static bool isJsonArray( lua_State *L, int i = 1 )
	{
		return( luaL_testudata( L, i, mTypeName ) != nullptr );
	}

	static UserData *checkjsonarraydata( lua_State *L, int i = 1 )
	{
		void *ud = luaL_checkudata( L, i, mTypeName );

		luaL_argcheck( L, ud != NULL, i, "JsonArray expected" );

		return( static_cast<UserData *>( ud ) );
	}

	static QJsonArray *checkjsonarray( lua_State *L, int i = 1 )
	{
		UserData	*FUD = checkjsonarraydata( L, i );

		return( FUD ? &FUD->mJsonArray : nullptr );
	}

private:
	static int luaDelete( lua_State *L );

	static int luaBegin( lua_State *L );

	static int luaNext( lua_State *L );

	static int luaIsArray( lua_State *L );
	static int luaIsObject( lua_State *L );

	static int luaGet( lua_State *L );

	static int luaPinLen( lua_State *L );

	static int luaToString(lua_State *L);

private:
	static const luaL_Reg					mLuaInstance[];
	static const luaL_Reg					mLuaMethods[];
	static const luaL_Reg					mLuaFunctions[];
#endif
};

#endif // LUAJSONARRAY_H
