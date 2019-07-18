#include "luabytearray.h"

#include <fugio/node_interface.h>
#include <fugio/node_control_interface.h>
#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>
#include <fugio/context_interface.h>
#include <fugio/lua/lua_interface.h>
#include <fugio/core/variant_interface.h>
#include <fugio/math/uuid.h>

#include "luaqtplugin.h"

#include "luamatrix4x4.h"
#include "luavector3.h"

const char *LuaByteArray::mTypeName = "qt.quaternion";

#if defined( LUA_SUPPORTED )

const luaL_Reg LuaByteArray::mLuaFunctions[] =
{
	{ "fromString",	LuaByteArray::fromString },
	{ 0, 0 }
};

const luaL_Reg LuaByteArray::mLuaMetaMethods[] =
{
	{ "__index",			LuaByteArray::luaIndex },
//	{ "__newindex",			LuaByteArray::luaNewIndex },
	{ 0, 0 }
};

const luaL_Reg LuaByteArray::mLuaMethods[] =
{
	{ 0, 0 }
};

void LuaByteArray::registerExtension(LuaInterface *LUA)
{
	LuaQtPlugin::addLuaFunction( "bytearray", LuaByteArray::luaNew );

	LUA->luaRegisterExtension( LuaByteArray::luaOpen );

	LUA->luaAddPinGet( PID_QUATERNION, LuaByteArray::luaPinGet );

	LUA->luaAddPinSet( PID_QUATERNION, LuaByteArray::luaPinSet );
}

int LuaByteArray::luaOpen( lua_State *L )
{
	luaL_newmetatable( L, mTypeName );

	lua_pushvalue( L, -1 );
	lua_setfield( L, -2, "__index" );

	luaL_setfuncs( L, mLuaMethods, 0 );

	luaL_newlib( L, mLuaFunctions );

	return( 1 );
}

int LuaByteArray::luaNew( lua_State *L )
{
	if( lua_gettop( L ) == 0 )
	{
		return( pushbytearray( L, QByteArray() ) );
	}

	luaL_getmetatable( L, mTypeName );

	return( 1 );
}

int LuaByteArray::luaPinGet( const QUuid &pPinLocalId, lua_State *L )
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
		return( luaL_error( L, "No bytearray pin" ) );
	}

	fugio::VariantInterface			*SrcVar = qobject_cast<fugio::VariantInterface *>( PinSrc->control()->qobject() );

	if( !SrcVar )
	{
		return( luaL_error( L, "Can't access bytearray" ) );
	}

	return( pushbytearray( L, SrcVar->variant().value<QByteArray>() ) );
}

int LuaByteArray::luaPinSet( const QUuid &pPinLocalId, lua_State *L, int pIndex )
{
	fugio::LuaInterface						*Lua  = LuaQtPlugin::lua();
	NodeInterface							*Node = Lua->node( L );
	QSharedPointer<fugio::PinInterface>		 Pin = Node->findPinByLocalId( pPinLocalId );
	UserData								*Q = checkuserdata( L, pIndex );

	if( !Pin )
	{
		return( luaL_error( L, "No destination pin" ) );
	}

	if( Pin->direction() != PIN_OUTPUT )
	{
		return( luaL_error( L, "No destination pin" ) );
	}

	if( !Pin->hasControl() )
	{
		return( luaL_error( L, "No bytearray pin" ) );
	}

	fugio::VariantInterface			*DstVar = qobject_cast<fugio::VariantInterface *>( Pin->control()->qobject() );

	if( !DstVar )
	{
		return( luaL_error( L, "Can't access bytearray" ) );
	}

	if( Q->mByteArray != DstVar->variant().value<QByteArray>() )
	{
		DstVar->setVariant( Q->mByteArray );

		Pin->node()->context()->pinUpdated( Pin );
	}

	return( 0 );
}


int LuaByteArray::fromString( lua_State *L )
{
	size_t		 C;
	const char	*S = luaL_checklstring( L, 2, &C );

	return( pushbytearray( L, QByteArray::fromRawData( S, C ) ) );
}

int LuaByteArray::luaIndex( lua_State *L )
{
	UserData	*UD = checkuserdata( L );

	if( lua_type( L, 2 ) == LUA_TSTRING )
	{
		const char	*S = luaL_checkstring( L, 2 );

		for( const luaL_Reg *R = mLuaMethods ; R->name ; R++ )
		{
			if( !strcmp( R->name, S ) )
			{
				lua_pushcfunction( L, R->func );

				return( 1 );
			}
		}

		return( luaL_error( L, "unknown field" ) );
	}

	int		i = luaL_checkinteger( L, 2 ) - 1;

	if( i < 0 || i >= UD->mByteArray.size() )
	{
		return( 0 );
	}

	lua_pushlstring( L, UD->mByteArray.data() + i, 1 );

	return( 1 );
}

#endif
