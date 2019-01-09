#include "luajsondocument.h"

#include <fugio/context_interface.h>
#include <fugio/node_interface.h>
#include <fugio/lua/lua_interface.h>
#include <fugio/core/variant_interface.h>
#include <fugio/performance.h>
#include <fugio/json/uuid.h>

#include "luaqtplugin.h"
#include "luajsonarray.h"
#include "luajsonobject.h"

const char *LuaJsonDocument::JsonDocumentUserData::TypeName = "qt.jsondocument";

#if defined( LUA_SUPPORTED )

const luaL_Reg LuaJsonDocument::mLuaInstance[] =
{
	{ 0, 0 }
};

const luaL_Reg LuaJsonDocument::mLuaMethods[] =
{
	{ "__gc",				LuaJsonDocument::luaDelete },
	{ "array",				LuaJsonDocument::luaArray },
	{ "fromString",			LuaJsonDocument::luaFromString },
	{ "fromVariant",		LuaJsonDocument::luaFromVariant },
	{ "isArray",			LuaJsonDocument::luaIsArray },
	{ "isObject",			LuaJsonDocument::luaIsObject },
	{ "object",				LuaJsonDocument::luaObject },
	{ "__tostring",			LuaJsonDocument::luaToString },
	{ 0, 0 }
};

void LuaJsonDocument::registerExtension(LuaInterface *LUA)
{
	LuaQtPlugin::addLuaFunction( "jsondocument", LuaJsonDocument::luaNew );

	LUA->luaRegisterExtension( LuaJsonDocument::luaOpen );

	LUA->luaAddPinGet( PID_JSON, LuaJsonDocument::luaPinGet );

	LUA->luaAddPushVariantFunction( QMetaType::QJsonDocument, LuaJsonDocument::pushVariant );
}

int LuaJsonDocument::luaOpen( lua_State *L )
{
	luaL_newmetatable( L, JsonDocumentUserData::TypeName );

	lua_pushvalue( L, -1 );
	lua_setfield( L, -2, "__index" );

	luaL_setfuncs( L, mLuaMethods, 0 );

	luaL_newlib( L, mLuaInstance );

	return( 1 );
}

int LuaJsonDocument::luaNew( lua_State *L )
{
	pushjsondocument( L, QJsonDocument() );

	return( 1 );
}

int LuaJsonDocument::luaDelete( lua_State *L )
{
	JsonDocumentUserData		*UserData = checkjsondocumentdata( L );

	if( UserData )
	{
		if( UserData->mJsonDocument )
		{
			delete UserData->mJsonDocument;

			UserData->mJsonDocument = nullptr;
		}
	}

	return( 0 );
}

int LuaJsonDocument::luaFromString( lua_State *L )
{
	JsonDocumentUserData	*UserData = checkjsondocumentdata( L );

	luaL_checkstring( L, 2 );

	*UserData->mJsonDocument = QJsonDocument::fromJson( QByteArray( lua_tostring( L, 2 ) ) );

	return( 0 );
}

int LuaJsonDocument::luaFromVariant( lua_State *L )
{
	JsonDocumentUserData					*UserData = checkjsondocumentdata( L );

	LuaInterface							*Lua  = LuaQtPlugin::lua();
	NodeInterface							*Node = Lua->node( L );
	QSharedPointer<fugio::PinInterface>		 Pin = Node->findPinByGlobalId( Lua->checkpin( L, 2 ) );
	QSharedPointer<fugio::PinInterface>		 PinSrc;

	if( !Pin )
	{
		return( luaL_error( L, "No source pin" ) );
	}

	if( Pin->direction() == PIN_OUTPUT )
	{
		PinSrc = Pin;
	}
	else
	{
		PinSrc = Pin->connectedPin();
	}

	if( !PinSrc || !PinSrc->hasControl() )
	{
		return( luaL_error( L, "No variant pin" ) );
	}

	fugio::VariantInterface		*Value = qobject_cast<fugio::VariantInterface *>( PinSrc->control()->qobject() );

	if( !Value )
	{
		return( luaL_argerror( L, 2, "Need a variant" ) );
	}

	if( Value->variant().type() == QVariant::String || Value->variant().type() == QVariant::ByteArray )
	{
		*UserData->mJsonDocument = QJsonDocument::fromJson( Value->variant().toByteArray() );
	}
	else
	{
		*UserData->mJsonDocument = QJsonDocument::fromVariant( Value->variant() );
	}

	return( 0 );
}

int LuaJsonDocument::luaIsArray( lua_State *L)
{
	JsonDocumentUserData		*UserData = checkjsondocumentdata( L );

	lua_pushboolean( L, UserData->mJsonDocument->isArray() );

	return( 1 );
}

int LuaJsonDocument::luaIsObject( lua_State *L)
{
	JsonDocumentUserData		*UserData = checkjsondocumentdata( L );

	lua_pushboolean( L, UserData->mJsonDocument->isObject() );

	return( 1 );
}

int LuaJsonDocument::luaArray( lua_State *L )
{
	JsonDocumentUserData		*UserData = checkjsondocumentdata( L );

	LuaJsonArray::pushjsonarray( L, UserData->mJsonDocument->array() );

	return( 1 );
}

int LuaJsonDocument::luaObject(lua_State *L)
{
	JsonDocumentUserData		*UserData = checkjsondocumentdata( L );

	LuaJsonObject::pushjsonobject( L, UserData->mJsonDocument->object() );

	return( 1 );
}

int LuaJsonDocument::luaToString(lua_State *L)
{
	JsonDocumentUserData		*UserData = checkjsondocumentdata( L );

	QString						 JsonData = UserData->mJsonDocument->toJson( QJsonDocument::Compact );

	lua_pushfstring( L, "%s", JsonData.toLatin1().constData() );

	return( 1 );
}

int LuaJsonDocument::luaPinGet(const QUuid &pPinLocalId, lua_State *L)
{
	fugio::LuaInterface						*Lua  = LuaQtPlugin::lua();
	NodeInterface							*Node = Lua->node( L );
	QSharedPointer<fugio::PinInterface>		 Pin = Node->findPinByLocalId( pPinLocalId );
	QSharedPointer<fugio::PinInterface>		 PinSrc;

	if( !Pin )
	{
		return( luaL_error( L, "No source pin" ) );
	}

	if( Pin->direction() == PIN_OUTPUT )
	{
		PinSrc = Pin;
	}
	else
	{
		PinSrc = Pin->connectedPin();
	}

	if( !PinSrc || !PinSrc->hasControl() )
	{
		return( luaL_error( L, "No JSON pin" ) );
	}

	fugio::VariantInterface			*SrcVar = qobject_cast<fugio::VariantInterface *>( PinSrc->control()->qobject() );

	if( !SrcVar )
	{
		return( luaL_error( L, "Can't access matrix" ) );
	}

	return( pushjsondocument( L, SrcVar->variant().value<QJsonDocument>() ) );
}

#endif
