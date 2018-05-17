#include "luajsonarray.h"

#include <QJsonValueRef>
#include <QJsonDocument>

#include "luajsonobject.h"

const char *LuaJsonArray::JsonArrayUserData::TypeName = "qt.jsonarray";

#if defined( LUA_SUPPORTED )

const luaL_Reg LuaJsonArray::mLuaFunctions[] =
{
	{ 0, 0 }
};

const luaL_Reg LuaJsonArray::mLuaInstance[] =
{
	{ "__index",		LuaJsonArray::luaGet },
//	{ "__newindex",		LuaJsonArray::luaSet },
	{ "__len",			LuaJsonArray::luaPinLen },
	{ "__gc",			LuaJsonArray::luaDelete },
	{ "__tostring",		LuaJsonArray::luaToString },
	{ 0, 0 }
};

const luaL_Reg LuaJsonArray::mLuaMethods[] =
{
	{ "ipairs",				LuaJsonArray::luaBegin },
	{ "isArray",			LuaJsonArray::luaIsArray },
	{ "isObject",			LuaJsonArray::luaIsObject },
	{ 0, 0 }
};

int LuaJsonArray::luaOpen( lua_State *L )
{
	luaL_newmetatable( L, JsonArrayUserData::TypeName );

	lua_pushvalue( L, -1 );
	lua_setfield( L, -2, "__index" );

	luaL_setfuncs( L, mLuaInstance, 0 );

	luaL_newlib( L, mLuaFunctions );

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
		UserData->mJsonArray.~QJsonArray();
	}

	return( 0 );
}

int LuaJsonArray::luaBegin( lua_State *L )
{
	JsonArrayUserData			*UserData = checkjsonarraydata( L );

	UserData->mIterator = UserData->mJsonArray.begin();

	lua_pushcclosure( L, LuaJsonArray::luaNext, 1 );

	return( 1 );
}

int LuaJsonArray::luaNext( lua_State *L )
{
	JsonArrayUserData			*UserData = (JsonArrayUserData *)lua_touserdata( L, lua_upvalueindex( 1 ) );

	if( UserData->mIterator != UserData->mJsonArray.end() )
	{
		lua_pushinteger( L, UserData->mIterator.i + 1 );

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

int LuaJsonArray::luaGet( lua_State *L )
{
	JsonArrayUserData			*UserData = checkjsonarraydata( L );

	if( lua_type( L, 2 ) == LUA_TNUMBER )
	{
		int									 i = lua_tointeger( L, 2 );

		if( !UserData )
		{
			return( 0 );
		}

		if( i <= 0 || i > UserData->mJsonArray.count() )
		{
			return( 0 );
		}

		QJsonValue	V = UserData->mJsonArray.at( i - 1 );

		switch( V.type() )
		{
			case QJsonValue::Array:
				LuaJsonArray::pushjsonarray( L, V.toArray() );
				break;

			case QJsonValue::Bool:
				lua_pushboolean( L, V.toBool() );
				break;

			case QJsonValue::Double:
				lua_pushnumber( L, V.toDouble() );
				break;

			case QJsonValue::String:
				lua_pushfstring( L, "%s", V.toString().toLatin1().constData() );
				break;

			case QJsonValue::Object:
				LuaJsonObject::pushjsonobject( L, V.toObject() );
				break;

			case QJsonValue::Null:
				lua_pushnil( L );
				break;

			case QJsonValue::Undefined:
				lua_pushnil( L );
				break;
		}

		return( 1 );
	}

	const char	*s = luaL_checkstring( L, 2 );

	for( const luaL_Reg *F = mLuaMethods ; F->func ; F++ )
	{
		if( !strcmp( s, F->name ) )
		{
			lua_pushcfunction( L, F->func );

			return( 1 );
		}
	}

	return( 0 );
}

int LuaJsonArray::luaPinLen( lua_State *L )
{
	JsonArrayUserData			*UserData = checkjsonarraydata( L );

	if( !UserData )
	{
		return( 0 );
	}

	lua_pushinteger( L, UserData->mJsonArray.count() );

	return( 1 );
}

int LuaJsonArray::luaToString(lua_State *L)
{
	JsonArrayUserData			*UserData = checkjsonarraydata( L );

	QString						 JsonData = QJsonDocument( UserData->mJsonArray ).toJson( QJsonDocument::Compact );

	lua_pushfstring( L, "%s", JsonData.toLatin1().constData() );

	return( 1 );
}

#endif
