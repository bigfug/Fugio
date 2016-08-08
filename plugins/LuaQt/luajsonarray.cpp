#include "luajsonarray.h"

#include <QJsonValueRef>

#include "luajsonobject.h"

const char *LuaJsonArray::JsonArrayUserData::TypeName = "qt.jsonarray";
const char *LuaJsonArray::JsonArrayIteratorUserData::TypeName = "qt.jsonarray.iterator";

#if defined( LUA_SUPPORTED )

const luaL_Reg LuaJsonArray::mLuaInstance[] =
{
	{ 0, 0 }
};

const luaL_Reg LuaJsonArray::mLuaMethods[] =
{
	{ "__gc",				LuaJsonArray::luaDelete },
	{ "ipairs",				LuaJsonArray::luaBegin },
	{ "isArray",			LuaJsonArray::luaIsArray },
	{ "isObject",			LuaJsonArray::luaIsObject },
	{ 0, 0 }
};

int LuaJsonArray::luaOpen( lua_State *L )
{
	luaL_newmetatable( L, JsonArrayIteratorUserData::TypeName );

	lua_pushcfunction( L, LuaJsonArray::luaIteratorEnd );
	lua_setfield( L, -2, "__gc" );

	luaL_newlib( L, mLuaInstance );

	lua_pop( L, 1 );

	//------------------------------------------------

	luaL_newmetatable( L, JsonArrayUserData::TypeName );

	lua_pushvalue( L, -1 );
	lua_setfield( L, -2, "__index" );

	luaL_setfuncs( L, mLuaMethods, 0 );

	luaL_newlib( L, mLuaInstance );

	return( 1 );
}

int LuaJsonArray::luaNew( lua_State *L )
{
	pushjsonarray( L, QJsonArray() );

	return( 1 );
}

int LuaJsonArray::luaDelete( lua_State *L )
{
	JsonArrayUserData		*UserData = checkjsonarraydata( L );

	if( UserData )
	{
		if( UserData->mJsonArray )
		{
			delete UserData->mJsonArray;

			UserData->mJsonArray = nullptr;
		}
	}

	return( 0 );
}

int LuaJsonArray::luaBegin( lua_State *L )
{
	JsonArrayUserData			*UserData = checkjsonarraydata( L );

	JsonArrayIteratorUserData	*IteratorData = (JsonArrayIteratorUserData *)lua_newuserdata( L, sizeof( JsonArrayIteratorUserData ) );

	luaL_getmetatable( L, JsonArrayIteratorUserData::TypeName );
	lua_setmetatable( L, -2 );

	IteratorData->mArray    = UserData->mJsonArray;
	IteratorData->mIterator = new QJsonArray::iterator( UserData->mJsonArray->begin() );
	IteratorData->mIndex    = 1;

	lua_pushcclosure( L, LuaJsonArray::luaIteratorNext, 1 );

	return( 1 );
}

int LuaJsonArray::luaIteratorNext( lua_State *L )
{
	JsonArrayIteratorUserData	*IteratorData = (JsonArrayIteratorUserData *)lua_touserdata( L, lua_upvalueindex( 1 ) );

	if( *IteratorData->mIterator != IteratorData->mArray->end() )
	{
		lua_pushinteger( L, IteratorData->mIndex );

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

		IteratorData->mIndex++;

		(*IteratorData->mIterator)++;

		return( 2 );
	}

	return( 0 );
}

int LuaJsonArray::luaIteratorEnd( lua_State *L )
{
	JsonArrayIteratorUserData	*IteratorData = (JsonArrayIteratorUserData *)lua_touserdata( L, 1 );

	IteratorData->mArray = 0;

	if( IteratorData->mIterator )
	{
		delete IteratorData->mIterator;

		IteratorData->mIterator = 0;
	}

	return( 0 );
}

int LuaJsonArray::luaIsArray(lua_State *L)
{
	lua_pushboolean( L, true );

	return( 1 );
}

int LuaJsonArray::luaIsObject(lua_State *L)
{
	lua_pushboolean( L, false );

	return( 1 );
}

#endif
