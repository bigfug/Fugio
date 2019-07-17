#include "luajsonobject.h"

#include <QJsonValueRef>
#include <QJsonDocument>

#include "luaqtplugin.h"
#include "luajsonarray.h"

const char *LuaJsonObject::JsonObjectUserData::TypeName = "qt.jsonobject";

#if defined( LUA_SUPPORTED )

const luaL_Reg LuaJsonObject::mLuaFunctions[] =
{
	{ 0, 0 }
};

const luaL_Reg LuaJsonObject::mLuaInstance[] =
{
	{ "__index",		LuaJsonObject::luaGet },
	{ "__gc",			LuaJsonObject::luaDelete },
	{ "__tostring",		LuaJsonObject::luaToString },
	{ 0, 0 }
};

const luaL_Reg LuaJsonObject::mLuaMethods[] =
{
	{ "isArray",			LuaJsonObject::luaIsArray },
	{ "isObject",			LuaJsonObject::luaIsObject },
	{ "pairs",				LuaJsonObject::luaBegin },
	{ 0, 0 }
};

void LuaJsonObject::registerExtension(fugio::LuaInterface *LUA)
{
	LuaQtPlugin::addLuaFunction( "jsonobject", LuaJsonObject::luaNew );

	LUA->luaRegisterExtension( LuaJsonObject::luaOpen );
}

int LuaJsonObject::luaOpen( lua_State *L )
{
	luaL_newmetatable( L, JsonObjectUserData::TypeName );

	lua_pushvalue( L, -1 );
	lua_setfield( L, -2, "__index" );

	luaL_setfuncs( L, mLuaInstance, 0 );

	luaL_newlib( L, mLuaFunctions );

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

int LuaJsonObject::luaGet(lua_State *L)
{
	JsonObjectUserData			*UserData = checkjsonobjectdata( L );

	if( lua_type( L, 2 ) == LUA_TSTRING )
	{
		QString		I = QString::fromLatin1( lua_tostring( L, 2 ) );

		if( !UserData )
		{
			return( 0 );
		}

		QJsonValue	V = UserData->mJsonObject.value( I );

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


int LuaJsonObject::luaToString(lua_State *L)
{
	JsonObjectUserData	*UserData = checkjsonobjectdata( L );

	QString				 JsonData = QJsonDocument( UserData->mJsonObject ).toJson( QJsonDocument::Compact );

	lua_pushfstring( L, "%s", JsonData.toLatin1().constData() );

	return( 1 );
}

#endif
