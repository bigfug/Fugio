#ifndef LUAJSONARRAY_H
#define LUAJSONARRAY_H

#if defined( LUA_SUPPORTED )
#include <lua.hpp>
#endif

#include <QJsonArray>

class LuaJsonArray
{
private:
	typedef struct JsonArrayUserData
	{
		static const char		*TypeName;

		QJsonArray				 mJsonArray;
		QJsonArray::iterator	 mIterator;

	} JsonArrayUserData;

public:
	LuaJsonArray( void ) {}

	virtual ~LuaJsonArray( void ) {}

#if defined( LUA_SUPPORTED )
	static int luaOpen( lua_State *L );

	static int luaNew( lua_State *L );

	static int pushjsonarray( lua_State *L, const QJsonArray &pJsonArray )
	{
		JsonArrayUserData	*UD = (JsonArrayUserData *)lua_newuserdata( L, sizeof( JsonArrayUserData ) );

		if( !UD )
		{
			return( 0 );
		}

		luaL_getmetatable( L, JsonArrayUserData::TypeName );
		lua_setmetatable( L, -2 );

		// Is this terribly evil?

		new( &UD->mJsonArray ) QJsonArray( pJsonArray );
		new( &UD->mIterator )  QJsonArray::iterator();

		return( 1 );
	}

	static bool isJsonArray( lua_State *L, int i = 1 )
	{
		return( luaL_testudata( L, i, JsonArrayUserData::TypeName ) != nullptr );
	}

	static JsonArrayUserData *checkjsonarraydata( lua_State *L, int i = 1 )
	{
		void *ud = luaL_checkudata( L, i, JsonArrayUserData::TypeName );

		luaL_argcheck( L, ud != NULL, i, "JsonArray expected" );

		return( static_cast<JsonArrayUserData *>( ud ) );
	}

	static QJsonArray *checkjsonarray( lua_State *L, int i = 1 )
	{
		JsonArrayUserData	*FUD = checkjsonarraydata( L, i );

		return( FUD ? &FUD->mJsonArray : nullptr );
	}

private:
	static int luaDelete( lua_State *L );

	static int luaBegin( lua_State *L );

	static int luaNext( lua_State *L );

	static int luaIsArray( lua_State *L );
	static int luaIsObject( lua_State *L );

private:
	static const luaL_Reg					mLuaInstance[];
	static const luaL_Reg					mLuaMethods[];
#endif
};

#endif // LUAJSONARRAY_H
