#include "luaquaternion.h"

#include <fugio/node_interface.h>
#include <fugio/node_control_interface.h>
#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>
#include <fugio/context_interface.h>
#include <fugio/lua/lua_interface.h>
#include <fugio/core/variant_interface.h>

#include "luaqtplugin.h"

#include "luamatrix4x4.h"

const char *LuaQuaternion::UserData::TypeName = "qt.quaternion";

#if defined( LUA_SUPPORTED )

const luaL_Reg LuaQuaternion::mLuaInstance[] =
{
	{ 0, 0 }
};

const luaL_Reg LuaQuaternion::mLuaMethods[] =
{
//	{ "__add",				LuaQuaternion::luaAdd },
//	{ "__div",				LuaQuaternion::luaDiv },
//	{ "__eq",				LuaQuaternion::luaEq },
//	{ "__mul",				LuaQuaternion::luaMul },
//	{ "__sub",				LuaQuaternion::luaSub },
	{ "length",				LuaQuaternion::luaLength },
#if QT_VERSION >= QT_VERSION_CHECK( 5, 5, 0 )
	{ "conjugated",			LuaQuaternion::luaConjugated },
#endif
	{ 0, 0 }
};

int LuaQuaternion::luaOpen (lua_State *L )
{
	luaL_newmetatable( L, UserData::TypeName );

	lua_pushvalue( L, -1 );
	lua_setfield( L, -2, "__index" );

	luaL_setfuncs( L, mLuaMethods, 0 );

	luaL_newlib( L, mLuaInstance );

	return( 1 );
}

int LuaQuaternion::luaNew( lua_State *L )
{
	if( lua_gettop( L ) == 1 )
	{
		if( LuaMatrix4x4::isMatrix4x4( L ) )
		{
			QMatrix4x4	M = LuaMatrix4x4::checkMatrix4x4( L );

			pushquaternion( L, QQuaternion::fromRotationMatrix( M.toGenericMatrix<3,3>() ) );
		}
	}
//	if( lua_gettop( L ) == 2 )
//	{
//		float		x = luaL_checknumber( L, 1 );
//		float		y = luaL_checknumber( L, 2 );

//		pushMatrix4x4( L, QMatrix4x4( x, y ) );
//	}
//	else
	{
		pushquaternion( L, QQuaternion() );
	}

	return( 1 );
}

int LuaQuaternion::luaPinGet(const QUuid &pPinLocalId, lua_State *L)
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
		return( luaL_error( L, "No quaternion pin" ) );
	}

	fugio::VariantInterface			*SrcVar = qobject_cast<fugio::VariantInterface *>( PinSrc->control()->qobject() );

	if( !SrcVar )
	{
		return( luaL_error( L, "Can't access quaternion" ) );
	}

	return( pushquaternion( L, SrcVar->variant().value<QQuaternion>() ) );
}

int LuaQuaternion::luaLength( lua_State *L )
{
	UserData	*UD = checkuserdata( L );

	lua_pushnumber( L, UD->mQuaternion.length() );

	return( 1 );
}

#if QT_VERSION >= QT_VERSION_CHECK( 5, 5, 0 )

int LuaQuaternion::luaConjugated( lua_State *L )
{
	UserData	*UD = checkuserdata( L );

	pushquaternion( L, UD->mQuaternion.conjugated() );

	return( 1 );
}

#endif

#endif
