#include "luajsonobject.h"

#include <QJsonValueRef>

#include "luajsonarray.h"

const char *LuaJsonObject::JsonObjectUserData::TypeName = "qt.jsonobject";

#if defined( LUA_SUPPORTED )

const luaL_Reg LuaJsonObject::mLuaInstance[] =
{
	{ 0, 0 }
};

const luaL_Reg LuaJsonObject::mLuaMethods[] =
{
	{ "__gc",				LuaJsonObject::luaDelete },
	{ "isArray",			LuaJsonObject::luaIsArray },
	{ "isObject",			LuaJsonObject::luaIsObject },
	{ "pairs",				LuaJsonObject::luaBegin },
	{ 0, 0 }
};

int LuaJsonObject::luaOpen( lua_State *L )
{
	luaL_newmetatable( L, JsonObjectUserData::TypeName );

	lua_pushvalue( L, -1 );
	lua_setfield( L, -2, "__index" );

	luaL_setfuncs( L, mLuaMethods, 0 );

	luaL_newlib( L, mLuaInstance );

	return( 1 );
}

int LuaJsonObject::luaNew( lua_State *L )
{
	pushjsonobject( L, QJsonObject() );

	return( 1 );
}

int LuaJsonObject::luaDelete( lua_State *L )
{
	JsonObjectUserData		*UserData = checkjsonobjectdata( L );

	if( UserData )
	{
		UserData->mJsonObject.~QJsonObject();
	}

	return( 0 );
}

int LuaJsonObject::luaBegin( lua_State *L )
{
	JsonObjectUserData			*UserData = checkjsonobjectdata( L );

	UserData->mIterator = UserData->mJsonObject.begin();

	lua_pushcclosure( L, LuaJsonObject::luaIteratorNext, 1 );

	return( 1 );
}

int LuaJsonObject::luaIteratorNext( lua_State *L )
{
	JsonObjectUserData			*UserData = (JsonObjectUserData *)lua_touserdata( L, lua_upvalueindex( 1 ) );

	if( UserData->mIterator != UserData->mJsonObject.end() )
	{
		lua_pushfstring( L, "%s", UserData->mIterator.key().toLatin1().constData() );

		QJsonValueRef	 ValRef = *UserData->mIterator;

		switch( ValRef.type() )
		{
			case QJsonValue::Array:
				LuaJsonArray::pushjsonarray( L, ValRef.toArray() );
				break;

			case QJsonValue::Bool:
				lua_pushboolean( L, ValRef.toBool() );
				break;

			case QJsonValue::Double:
				lua_pushnumber( L, ValRef.toDouble() );
				break;

			case QJsonValue::String:
				lua_pushfstring( L, "%s", ValRef.toString().toLatin1().constData() );
				break;

			case QJsonValue::Object:
				LuaJsonObject::pushjsonobject( L, ValRef.toObject() );
				break;

			case QJsonValue::Null:
				lua_pushnil( L );
				break;

			case QJsonValue::Undefined:
				lua_pushnil( L );
				break;
		}

		UserData->mIterator++;

		return( 2 );
	}

	return( 0 );
}

int LuaJsonObject::luaIsArray(lua_State *L)
{
	lua_pushboolean( L, false );

	return( 1 );
}

int LuaJsonObject::luaIsObject(lua_State *L)
{
	lua_pushboolean( L, true );

	return( 1 );
}

#endif
