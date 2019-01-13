#include "luamatrix4x4.h"

#include <fugio/node_interface.h>
#include <fugio/node_control_interface.h>
#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>
#include <fugio/context_interface.h>
#include <fugio/lua/lua_interface.h>
#include <fugio/core/variant_interface.h>
#include <fugio/math/uuid.h>

#include "luaqtplugin.h"

#include "luapointf.h"
#include "luarectf.h"
#include "luavector3.h"

const char *LuaMatrix4x4::mTypeName = "qt.matrix4x4";

#if defined( LUA_SUPPORTED )

const luaL_Reg LuaMatrix4x4::mLuaFunctions[] =
{
	{ Q_NULLPTR, Q_NULLPTR }
};

const luaL_Reg LuaMatrix4x4::mLuaMetaMethods[] =
{
//	{ "__add",				LuaMatrix4x4::luaAdd },
//	{ "__div",				LuaMatrix4x4::luaDiv },
//	{ "__eq",				LuaMatrix4x4::luaEq },
	{ "__mul",				LuaMatrix4x4::luaMul },
//	{ "__sub",				LuaMatrix4x4::luaSub },
	{ "__index",			LuaMatrix4x4::luaIndex },
	{ "__newindex",			LuaMatrix4x4::luaNewIndex },
	{ Q_NULLPTR, Q_NULLPTR }
};

const luaL_Reg LuaMatrix4x4::mLuaMethods[] =
{
#if QT_VERSION >= QT_VERSION_CHECK( 5, 5, 0 )
	{ "isAffine",			LuaMatrix4x4::luaIsAffine },
#endif
	{ "isIdentity",			LuaMatrix4x4::luaIsIdentity },
	{ "frustum",			LuaMatrix4x4::luaFrustum },
	{ "ortho",				LuaMatrix4x4::luaOrtho },
	{ "lookAt",				LuaMatrix4x4::luaLookAt },
	{ "perspective",		LuaMatrix4x4::luaPerspective },
	{ "rotate",				LuaMatrix4x4::luaRotate },
	{ "scale",				LuaMatrix4x4::luaScale },
	{ "toArray",			LuaMatrix4x4::luaToArray },
	{ "translate",			LuaMatrix4x4::luaTranslate },
	{ Q_NULLPTR, Q_NULLPTR }
};

void LuaMatrix4x4::registerExtension(LuaInterface *LUA)
{
	LuaQtPlugin::addLuaFunction( "matrix4x4", LuaMatrix4x4::luaNew );

	LUA->luaRegisterExtension( LuaMatrix4x4::luaOpen );

	LUA->luaAddPinGet( PID_MATRIX4, LuaMatrix4x4::luaPinGet );

	LUA->luaAddPinSet( PID_MATRIX4, LuaMatrix4x4::luaPinSet );
}

int LuaMatrix4x4::luaOpen (lua_State *L )
{
	luaL_newmetatable( L, mTypeName );

	lua_pushvalue( L, -1 );
	lua_setfield( L, -2, "__index" );

	luaL_setfuncs( L, mLuaMetaMethods, 0 );

	luaL_newlib( L, mLuaFunctions );

	return( 1 );
}

int LuaMatrix4x4::luaNew( lua_State *L )
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

	//luaL_getmetatable( L, UserData::TypeName );

	return( 1 );
}

int LuaMatrix4x4::luaNewQt( lua_State *L )
{
	pushmatrix4x4( L, QMatrix4x4() );

	return( 1 );
}

int LuaMatrix4x4::luaIndex( lua_State *L )
{
//	UserData	*UD = checkuserdata( L );
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

int LuaMatrix4x4::luaNewIndex(lua_State *L)
{
	UserData	*UD = checkuserdata( L );
	int			 Index  = luaL_checkinteger( L, 2 );

	luaL_argcheck( L, Index >= 1 && Index <= 16, 2, "index must be 1-16" );

	lua_Number	 V = luaL_checknumber( L, 3 );

	Index = Index - 1;

	UD->mMatrix( Index / 4, Index % 4 ) = V;

	return( 0 );
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
	UserData	*MatrixData = checkuserdata( L );

	luaL_checkany( L, 2 );

	if( isMatrix4x4( L, 2 ) )
	{
		UserData	*Matrix2 = checkuserdata( L, 2 );
		QMatrix4x4			 Matrix3 = MatrixData->mMatrix * Matrix2->mMatrix;

		pushmatrix4x4( L, Matrix3 );

		return( 1 );
	}

	if( LuaPointF::isPointF( L, 2 ) )
	{
		QPointF				 Point1 = LuaPointF::checkpointf( L, 2 );
		QPointF				 Point2 = MatrixData->mMatrix * Point1;

		LuaPointF::pushpointf( L, Point2 );

		return( 1 );
	}

	if( lua_type( L, 2 ) == LUA_TNUMBER )
	{
		QMatrix4x4			 Matrix2 = MatrixData->mMatrix * lua_tonumber( L, 2 );

		pushmatrix4x4( L, Matrix2 );

		return( 1 );
	}

	luaL_argerror( L, 2, "Bad argument type" );

	return( 0 );
}

int LuaMatrix4x4::luaFrustum( lua_State *S )
{
	UserData	*MatrixData = checkuserdata( S );

	float				 L, R, B, T, N, F;

	L = luaL_checknumber( S, 2 );
	R = luaL_checknumber( S, 3 );
	B = luaL_checknumber( S, 4 );
	T = luaL_checknumber( S, 5 );
	N = luaL_checknumber( S, 6 );
	F = luaL_checknumber( S, 7 );

	MatrixData->mMatrix.frustum( L, R, B, T, N, F );

	return( 0 );
}

int LuaMatrix4x4::luaOrtho( lua_State *L )
{
	UserData	*MatrixData = checkuserdata( L );

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

	MatrixData->mMatrix.ortho( R.left(), R.right(), R.bottom(), R.top(), N, F );

	return( 0 );
}

int LuaMatrix4x4::luaPerspective(lua_State *L)
{
	UserData	*MatrixData = checkuserdata( L );

	float				 A, R, N, F;

	A = luaL_checknumber( L, 2 );
	R = luaL_checknumber( L, 3 );
	N = luaL_checknumber( L, 4 );
	F = luaL_checknumber( L, 5 );

	MatrixData->mMatrix.perspective( A, R, N, F );

	return( 0 );
}

int LuaMatrix4x4::luaRotate(lua_State *L)
{
	UserData	*MatrixData = checkuserdata( L );

	float		Angle = luaL_checknumber( L, 2 );

	QVector3D	Axis;

	if( lua_gettop( L ) >= 3 ) Axis.setX( luaL_checknumber( L, 3 ) );
	if( lua_gettop( L ) >= 4 ) Axis.setY( luaL_checknumber( L, 4 ) );
	if( lua_gettop( L ) >= 5 ) Axis.setZ( luaL_checknumber( L, 5 ) );

	MatrixData->mMatrix.rotate( Angle, Axis );

	return( 0 );
}

int LuaMatrix4x4::luaScale(lua_State *L)
{
	UserData	*MatrixData = checkuserdata( L );

	float				 F = luaL_checknumber( L, 2 );

	if( lua_gettop( L ) > 2 )
	{
		QVector3D	Scale( F, 1, 1 );

		if( lua_gettop( L ) >= 3 ) Scale.setY( luaL_checknumber( L, 3 ) );
		if( lua_gettop( L ) >= 4 ) Scale.setZ( luaL_checknumber( L, 4 ) );

		MatrixData->mMatrix.scale( Scale );
	}
	else
	{
		MatrixData->mMatrix.scale( F );
	}

	return( 0 );
}

int LuaMatrix4x4::luaTranslate( lua_State *L )
{
	UserData	*MatrixData = checkuserdata( L );

	QVector3D	Translate;

	if( lua_gettop( L ) >= 2 ) Translate.setX( luaL_checknumber( L, 2 ) );
	if( lua_gettop( L ) >= 3 ) Translate.setY( luaL_checknumber( L, 3 ) );
	if( lua_gettop( L ) >= 4 ) Translate.setZ( luaL_checknumber( L, 4 ) );

	MatrixData->mMatrix.translate( Translate );

	return( 0 );
}

int LuaMatrix4x4::luaLookAt( lua_State *L )
{
	UserData	*MatrixData = checkuserdata( L );

	QVector3D	Eye    = LuaVector3D::checkvector3d( L, 2 );
	QVector3D	Center = LuaVector3D::checkvector3d( L, 3 );
	QVector3D	Up     = LuaVector3D::checkvector3d( L, 4 );

	MatrixData->mMatrix.lookAt( Eye, Center, Up );

	return( 0 );
}

#if QT_VERSION >= QT_VERSION_CHECK( 5, 5, 0 )
int LuaMatrix4x4::luaIsAffine(lua_State *L)
{
	UserData	*MatrixData = checkuserdata( L );

	lua_pushboolean( L, MatrixData->mMatrix.isAffine() );

	return( 0 );
}
#endif

int LuaMatrix4x4::luaIsIdentity(lua_State *L)
{
	UserData	*MatrixData = checkuserdata( L );

	lua_pushboolean( L, MatrixData->mMatrix.isIdentity() );

	return( 1 );
}

int LuaMatrix4x4::luaToArray( lua_State *L )
{
	UserData	*MatrixData = checkuserdata( L );
	const float			*MatDat = MatrixData->mMatrix.constData();

	lua_newtable( L );

	for( int i = 0 ; i < 16 ; i++ )
	{
		lua_pushnumber( L, MatDat[ i ] );

		lua_rawseti( L, -2, i + 1 );
	}

	return( 1 );
}

int LuaMatrix4x4::luaPinSet( const QUuid &pPinLocalId, lua_State *L, int pIndex )
{
	fugio::LuaInterface						*Lua  = LuaQtPlugin::lua();
	NodeInterface							*Node = Lua->node( L );
	QSharedPointer<fugio::PinInterface>		 Pin = Node->findPinByLocalId( pPinLocalId );
	UserData								*UD = checkuserdata( L, pIndex );

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
		return( luaL_error( L, "No quaternion pin" ) );
	}

	fugio::VariantInterface			*DstVar = qobject_cast<fugio::VariantInterface *>( Pin->control()->qobject() );

	if( !DstVar )
	{
		return( luaL_error( L, "Can't access quaternion" ) );
	}

	if( UD->mMatrix != DstVar->variant().value<QMatrix4x4>() )
	{
		DstVar->setVariant( UD->mMatrix );

		Pin->node()->context()->pinUpdated( Pin );
	}

	return( 0 );
}

#endif
