#include "luamatrix4x4.h"

#include <fugio/node_interface.h>
#include <fugio/node_control_interface.h>
#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>
#include <fugio/context_interface.h>
#include <fugio/lua/lua_interface.h>
#include <fugio/core/variant_interface.h>

#include "luaqtplugin.h"

#include "luapointf.h"
#include "luarectf.h"

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
	{ "ortho",				LuaMatrix4x4::luaOrtho },
	{ "perspective",		LuaMatrix4x4::luaPerspective },
	{ "rotate",				LuaMatrix4x4::luaRotate },
	{ "scale",				LuaMatrix4x4::luaScale },
	{ "toArray",			LuaMatrix4x4::luaToArray },
	{ "translate",			LuaMatrix4x4::luaTranslate },
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

	luaL_checkany( L, 2 );

	if( isMatrix4x4( L, 2 ) )
	{
		Matrix4x4UserData	*MatrixData = checkMatrix4x4userdata( L, 2 );
		QMatrix4x4			 Matrix2 = *MatrixData;
		QMatrix4x4			 Matrix3 = Matrix * Matrix2;

		pushmatrix4x4( L, Matrix3 );

		return( 1 );
	}

	if( LuaPointF::isPointF( L, 2 ) )
	{
		QPointF				 Point1 = LuaPointF::checkpointf( L, 2 );
		QPointF				 Point2 = Matrix * Point1;

		LuaPointF::pushpointf( L, Point2 );

		return( 1 );
	}

	if( lua_type( L, 2 ) == LUA_TNUMBER )
	{
		QMatrix4x4			 Matrix2 = Matrix * lua_tonumber( L, 2 );

		pushmatrix4x4( L, Matrix2 );

		return( 1 );
	}

	luaL_argerror( L, 2, "Bad argument type" );

	return( 0 );
}

int LuaMatrix4x4::luaOrtho( lua_State *L )
{
	Matrix4x4UserData	*MatrixData = checkMatrix4x4userdata( L );
	QMatrix4x4			 Matrix = *MatrixData;

	QRectF				 R;
	float				 N, F;

	if( LuaRectF::isRectF( L, 2 ) )
	{
		R = LuaRectF::checkrectf( L, 2 );
		N = -1;
		F = -1;
	}
	else
	{
		R.setLeft( luaL_checknumber( L, 2 ) );
		R.setRight( luaL_checknumber( L, 3 ) );
		R.setBottom( luaL_checknumber( L, 4 ) );
		R.setTop( luaL_checknumber( L, 5 ) );

		N = luaL_checknumber( L, 6 );
		F = luaL_checknumber( L, 7 );
	}

	Matrix.ortho( R.left(), R.right(), R.bottom(), R.top(), N, F );

	MatrixData->fromQMatrix4x4( Matrix );

	return( 0 );
}

int LuaMatrix4x4::luaPerspective(lua_State *L)
{
	Matrix4x4UserData	*MatrixData = checkMatrix4x4userdata( L );
	QMatrix4x4			 Matrix = *MatrixData;

	float				 A, R, N, F;

	A = luaL_checknumber( L, 2 );
	R = luaL_checknumber( L, 3 );
	N = luaL_checknumber( L, 4 );
	F = luaL_checknumber( L, 5 );

	Matrix.perspective( A, R, N, F );

	MatrixData->fromQMatrix4x4( Matrix );

	return( 0 );
}

int LuaMatrix4x4::luaRotate(lua_State *L)
{
	Matrix4x4UserData	*MatrixData = checkMatrix4x4userdata( L );
	QMatrix4x4			 Matrix = *MatrixData;

	float		Angle = luaL_checknumber( L, 2 );

	QVector3D	Axis;

	if( lua_gettop( L ) >= 3 ) Axis.setX( luaL_checknumber( L, 3 ) );
	if( lua_gettop( L ) >= 4 ) Axis.setY( luaL_checknumber( L, 4 ) );
	if( lua_gettop( L ) >= 5 ) Axis.setZ( luaL_checknumber( L, 5 ) );

	Matrix.rotate( Angle, Axis );

	MatrixData->fromQMatrix4x4( Matrix );

	return( 0 );
}

int LuaMatrix4x4::luaScale(lua_State *L)
{
	Matrix4x4UserData	*MatrixData = checkMatrix4x4userdata( L );
	QMatrix4x4			 Matrix = *MatrixData;

	float				 F = luaL_checknumber( L, 2 );

	if( lua_gettop( L ) > 2 )
	{
		QVector3D	Scale( F, 1, 1 );

		if( lua_gettop( L ) >= 3 ) Scale.setY( luaL_checknumber( L, 3 ) );
		if( lua_gettop( L ) >= 4 ) Scale.setZ( luaL_checknumber( L, 4 ) );

		Matrix.scale( Scale );
	}
	else
	{
		Matrix.scale( F );
	}

	MatrixData->fromQMatrix4x4( Matrix );

	return( 0 );
}

int LuaMatrix4x4::luaTranslate( lua_State *L )
{
	Matrix4x4UserData	*MatrixData = checkMatrix4x4userdata( L );
	QMatrix4x4			 Matrix = *MatrixData;

	QVector3D	Translate;

	if( lua_gettop( L ) >= 2 ) Translate.setX( luaL_checknumber( L, 2 ) );
	if( lua_gettop( L ) >= 3 ) Translate.setY( luaL_checknumber( L, 3 ) );
	if( lua_gettop( L ) >= 4 ) Translate.setZ( luaL_checknumber( L, 4 ) );

	Matrix.translate( Translate );

	MatrixData->fromQMatrix4x4( Matrix );

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
