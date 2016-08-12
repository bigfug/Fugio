#ifndef LUAJSONDOCUMENT_H
#define LUAJSONDOCUMENT_H

#if defined( LUA_SUPPORTED )
#include <lua.hpp>
#endif

#include <QJsonDocument>

class LuaJsonDocument
{
private:
	typedef struct JsonDocumentUserData
	{
		static const char		*TypeName;

		QJsonDocument			*mJsonDocument;
	} JsonDocumentUserData;

public:
	LuaJsonDocument( void ) {}

	virtual ~LuaJsonDocument( void ) {}

#if defined( LUA_SUPPORTED )
	static int luaOpen( lua_State *L );

	static int luaNew( lua_State *L );

	static int pushjsondocument( lua_State *L, const QJsonDocument &pJsonDocument )
	{
		JsonDocumentUserData	*UD = (JsonDocumentUserData *)lua_newuserdata( L, sizeof( JsonDocumentUserData ) );

		if( !UD )
		{
			return( 0 );
		}

		luaL_getmetatable( L, JsonDocumentUserData::TypeName );
		lua_setmetatable( L, -2 );

		UD->mJsonDocument = new QJsonDocument( pJsonDocument );

		return( 1 );
	}

	static bool isJsonDocument( lua_State *L, int i = 1 )
	{
		return( luaL_testudata( L, i, JsonDocumentUserData::TypeName ) != nullptr );
	}

	static JsonDocumentUserData *checkjsondocumentdata( lua_State *L, int i = 1 )
	{
		void *ud = luaL_checkudata( L, i, JsonDocumentUserData::TypeName );

		luaL_argcheck( L, ud != NULL, i, "JsonDocument expected" );

		return( static_cast<JsonDocumentUserData *>( ud ) );
	}

	static QJsonDocument *checkjsondocument( lua_State *L, int i = 1 )
	{
		JsonDocumentUserData	*FUD = checkjsondocumentdata( L, i );

		return( FUD ? FUD->mJsonDocument : nullptr );
	}

private:
	static int luaDelete( lua_State *L );

	static int luaFromString( lua_State *L );
	static int luaFromVariant( lua_State *L );

	static int luaIsArray( lua_State *L );
	static int luaIsObject( lua_State *L );

	static int luaArray( lua_State *L );

private:
	static const luaL_Reg					mLuaInstance[];
	static const luaL_Reg					mLuaMethods[];
#endif
};

#endif // LUAJSONDOCUMENT_H
