#include "luajsonobject.h"

#include <QJsonValueRef>

#include "luajsonarray.h"

const char *LuaJsonObject::JsonObjectUserData::TypeName = "qt.jsonobject";
const char *LuaJsonObject::JsonObjectIteratorUserData::TypeName = "qt.jsonobject.iterator";

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
	luaL_newmetatable( L, JsonObjectIteratorUserData::TypeName );

	lua_pushcfunction( L, LuaJsonObject::luaIteratorEnd );
	lua_setfield( L, -2, "__gc" );

	luaL_newlib( L, mLuaInstance );

	lua_pop( L, 1 );

	//------------------------------------------------

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
		if( UserData->mJsonObject )
		{
			delete UserData->mJsonObject;

			UserData->mJsonObject = nullptr;
		}
	}

	return( 0 );
}

int LuaJsonObject::luaBegin( lua_State *L )
{
	JsonObjectUserData			*UserData = checkjsonobjectdata( L );

	JsonObjectIteratorUserData	*IteratorData = (JsonObjectIteratorUserData *)lua_newuserdata( L, sizeof( JsonObjectIteratorUserData ) );

	luaL_getmetatable( L, JsonObjectIteratorUserData::TypeName );
	lua_setmetatable( L, -2 );

	IteratorData->mObject    = UserData->mJsonObject;
	IteratorData->mIterator = new QJsonObject::iterator( UserData->mJsonObject->begin() );

	lua_pushcclosure( L, LuaJsonObject::luaIteratorNext, 1 );

	return( 1 );
}

int LuaJsonObject::luaIteratorNext( lua_State *L )
{
	JsonObjectIteratorUserData	*IteratorData = (JsonObjectIteratorUserData *)lua_touserdata( L, lua_upvalueindex( 1 ) );

	if( *IteratorData->mIterator != IteratorData->mObject->end() )
	{
		lua_pushfstring( L, "%s", IteratorData->mIterator->key().toLatin1().constData() );

		QJsonValueRef	 ValRef = *(*IteratorData->mIterator);

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

		(*IteratorData->mIterator)++;

		return( 2 );
	}

	return( 0 );
}

int LuaJsonObject::luaIteratorEnd( lua_State *L )
{
	JsonObjectIteratorUserData	*IteratorData = (JsonObjectIteratorUserData *)lua_touserdata( L, 1 );

	IteratorData->mObject = 0;

	if( IteratorData->mIterator )
	{
		delete IteratorData->mIterator;

		IteratorData->mIterator = 0;
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
