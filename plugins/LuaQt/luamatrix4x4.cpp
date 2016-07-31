#include "luamatrix4x4.h"

#include <fugio/node_interface.h>
#include <fugio/node_control_interface.h>
#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>
#include <fugio/context_interface.h>
#include <fugio/lua/lua_interface.h>
#include <fugio/core/variant_interface.h>

#include "luaqtplugin.h"

const char *LuaMatrix4x4::Matrix4x4UserData::TypeName = "qt.matrix4x4";

#if defined( LUA_SUPPORTED )

const luaL_Reg LuaMatrix4x4::mLuaInstance[] =
{
	{ 0, 0 }
};

const luaL_Reg LuaMatrix4x4::mLuaMethods[] =
{
//	{ "__add",				LuaMatrix4x4::luaAdd },
//	{ "__div",				LuaMatrix4x4::luaDiv },
//	{ "__eq",				LuaMatrix4x4::luaEq },
	{ "__mul",				LuaMatrix4x4::luaMul },
//	{ "__sub",				LuaMatrix4x4::luaSub },
	{ "isAffine",			LuaMatrix4x4::luaIsAffine },
	{ "isIdentity",			LuaMatrix4x4::luaIsIdentity },
//	{ "manhattanLength",	LuaMatrix4x4::luaManhattanLength },
//	{ "setX",				LuaMatrix4x4::luaSetX },
//	{ "setY",				LuaMatrix4x4::luaSetY },
//	{ "x",					LuaMatrix4x4::luaX },
//	{ "y",					LuaMatrix4x4::luaY },
	{ 0, 0 }
};

int LuaMatrix4x4::luaOpen (lua_State *L )
{
	luaL_newmetatable( L, Matrix4x4UserData::TypeName );

	lua_pushvalue( L, -1 );
	lua_setfield( L, -2, "__index" );

	luaL_setfuncs( L, mLuaMethods, 0 );

	luaL_newlib( L, mLuaInstance );

	return( 1 );
}

int LuaMatrix4x4::luaNew(lua_State *L)
{
//	if( lua_gettop( L ) == 2 )
//	{
//		float		x = luaL_checknumber( L, 1 );
//		float		y = luaL_checknumber( L, 2 );

//		pushMatrix4x4( L, QMatrix4x4( x, y ) );
//	}
//	else
	{
		pushmatrix4x4( L, QMatrix4x4() );
	}

	return( 1 );
}

int LuaMatrix4x4::luaPinGet( const QUuid &pPinLocalId, lua_State *L )
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
		return( luaL_error( L, "No colour pin" ) );
	}

	fugio::VariantInterface			*SrcVar = qobject_cast<fugio::VariantInterface *>( PinSrc->control()->qobject() );

	if( !SrcVar )
	{
		return( luaL_error( L, "Can't access matrix" ) );
	}

	return( pushmatrix4x4( L, SrcVar->variant().value<QMatrix4x4>() ) );
}

int LuaMatrix4x4::luaMul( lua_State *L )
{
	Matrix4x4UserData	*MatrixData = checkMatrix4x4userdata( L );
	QMatrix4x4			 Matrix = *MatrixData;

	return( 0 );
}

int LuaMatrix4x4::luaIsAffine(lua_State *L)
{
	Matrix4x4UserData	*MatrixData = checkMatrix4x4userdata( L );
	QMatrix4x4			 Matrix = *MatrixData;

	lua_pushboolean( L, Matrix.isAffine() );

	return( 0 );
}

int LuaMatrix4x4::luaIsIdentity(lua_State *L)
{
	Matrix4x4UserData	*MatrixData = checkMatrix4x4userdata( L );
	QMatrix4x4			 Matrix = *MatrixData;

	lua_pushboolean( L, Matrix.isIdentity() );

	return( 0 );
}

int LuaMatrix4x4::luaToArray( lua_State *L )
{
	Matrix4x4UserData *MatrixData = checkMatrix4x4userdata( L );

	lua_newtable( L );

	for( int i = 0 ; i < 16 ; i++ )
	{
		lua_pushnumber( L, MatrixData->mMatDat[ i ] );

		lua_rawseti( L, -2, i + 1 );
	}

	return( 1 );
}

#endif
