#ifndef LUAJSONOBJECT_H
#define LUAJSONOBJECT_H

#if defined( LUA_SUPPORTED )
#include <lua.hpp>
#endif

#include <QJsonObject>

#include <fugio/lua/lua_interface.h>

class LuaJsonObject
{
private:
	typedef struct JsonObjectUserData
	{
		static const char		*TypeName;

		QJsonObject				 mJsonObject;
		QJsonObject::iterator	 mIterator;

	} JsonObjectUserData;

public:
	LuaJsonObject( void ) {}

	virtual ~LuaJsonObject( void ) {}

#if defined( LUA_SUPPORTED )
	static void registerExtension( fugio::LuaInterface *LUA );

	static int luaOpen( lua_State *L );

	static int luaNew( lua_State *L );

	static int pushjsonobject( lua_State *L, const QJsonObject &pJsonObject )
	{
		JsonObjectUserData	*UD = (JsonObjectUserData *)lua_newuserdata( L, sizeof( JsonObjectUserData ) );

		if( !UD )
		{
			return( 0 );
		}

		luaL_getmetatable( L, JsonObjectUserData::TypeName );
		lua_setmetatable( L, -2 );

		new( &UD->mJsonObject ) QJsonObject( pJsonObject );
		new( &UD->mIterator )   QJsonObject::iterator();

		return( 1 );
	}

	static bool isJsonObject( lua_State *L, int i = 1 )
	{
		return( luaL_testudata( L, i, JsonObjectUserData::TypeName ) != nullptr );
	}

	static JsonObjectUserData *checkjsonobjectdata( lua_State *L, int i = 1 )
	{
		void *ud = luaL_checkudata( L, i, JsonObjectUserData::TypeName );

		luaL_argcheck( L, ud != NULL, i, "JsonObject expected" );

		return( static_cast<JsonObjectUserData *>( ud ) );
	}

	static QJsonObject *checkjsonobject( lua_State *L, int i = 1 )
	{
		JsonObjectUserData	*FUD = checkjsonobjectdata( L, i );

		return( FUD ? &FUD->mJsonObject : nullptr );
	}

private:
	static int luaDelete( lua_State *L );

	static int luaBegin( lua_State *L );

	static int luaIteratorNext( lua_State *L );
	static int luaIteratorEnd( lua_State *L );

	static int luaIsArray( lua_State *L );
	static int luaIsObject( lua_State *L );

	static int luaGet( lua_State *L );

	static int luaToString( lua_State *L );

private:
	static const luaL_Reg					mLuaInstance[];
	static const luaL_Reg					mLuaMethods[];
	static const luaL_Reg					mLuaFunctions[];
#endif
};

#endif // LUAJSONOBJECT_H
