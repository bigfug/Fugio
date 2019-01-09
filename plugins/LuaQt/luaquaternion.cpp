#include "luaquaternion.h"

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

const char *LuaQuaternion::mTypeName = "qt.quaternion";

#if defined( LUA_SUPPORTED )

const luaL_Reg LuaQuaternion::mLuaFunctions[] =
{
	{ "fromEulerAngles",	LuaQuaternion::luaFromEulerAngles },
	{ "fromRotationMatrix",	LuaQuaternion::luaFromRotationMatrix },
	{ 0, 0 }
};

const luaL_Reg LuaQuaternion::mLuaMetaMethods[] =
{
	{ "__add",				LuaQuaternion::luaAdd },
	{ "__div",				LuaQuaternion::luaDiv },
	{ "__eq",				LuaQuaternion::luaEq },
	{ "__mul",				LuaQuaternion::luaMul },
	{ "__sub",				LuaQuaternion::luaSub },
	{ "__index",			LuaQuaternion::luaIndex },
	{ "__newindex",			LuaQuaternion::luaNewIndex },
	{ 0, 0 }
};

const luaL_Reg LuaQuaternion::mLuaMethods[] =
{
	{ "dotProduct",			LuaQuaternion::luaDotProduct },
	{ "isIdentity",			LuaQuaternion::luaIsIdentity },
	{ "isNull",				LuaQuaternion::luaIsNull },
	{ "length",				LuaQuaternion::luaLength },
	{ "lengthSquared",		LuaQuaternion::luaLengthSquared },
	{ "normalize",			LuaQuaternion::luaNormalize },
	{ "normalized",			LuaQuaternion::luaNormalized },
	{ "rotatedVector",		LuaQuaternion::luaRotatedVector },
	{ "toEulerAngles",		LuaQuaternion::luaToEulerAngles },
	{ "toRotationMatrix",	LuaQuaternion::luaToRotationMatrix },
#if QT_VERSION >= QT_VERSION_CHECK( 5, 5, 0 )
	{ "conjugated",			LuaQuaternion::luaConjugated },
	{ "inverted",			LuaQuaternion::luaInverted },
#endif
	{ 0, 0 }
};

void LuaQuaternion::registerExtension(LuaInterface *LUA)
{
	LuaQtPlugin::addLuaFunction( "quaternion", LuaQuaternion::luaNew );

	LUA->luaRegisterExtension( LuaQuaternion::luaOpen );

	LUA->luaAddPinGet( PID_QUATERNION, LuaQuaternion::luaPinGet );

	LUA->luaAddPinSet( PID_QUATERNION, LuaQuaternion::luaPinSet );
}

int LuaQuaternion::luaOpen( lua_State *L )
{
	luaL_newmetatable( L, mTypeName );

	lua_pushvalue( L, -1 );
	lua_setfield( L, -2, "__index" );

	luaL_setfuncs( L, mLuaMethods, 0 );

	luaL_newlib( L, mLuaFunctions );

	return( 1 );
}

int LuaQuaternion::luaNew( lua_State *L )
{
	if( lua_gettop( L ) == 0 )
	{
		return( pushquaternion( L, QQuaternion() ) );
	}

	if( lua_gettop( L ) == 4 )
	{
		float		scalar = luaL_checknumber( L, 1 );
		float		xpos = luaL_checknumber( L, 2 );
		float		ypos = luaL_checknumber( L, 3 );
		float		zpos = luaL_checknumber( L, 4 );

		return( pushquaternion( L, QQuaternion( scalar, xpos, ypos, zpos ) ) );
	}

//	if( lua_gettop( L ) == 1 )
//	{
//		QVector4D	vector = LuaVector4D::checkvector4d( L, 2 );

//		return( pushquaternion( L, QQuaternion( vector ) ) );
//	}

	luaL_getmetatable( L, mTypeName );

	return( 1 );

	//return( luaL_error( L, "incorrect arguments" ) );
}

int LuaQuaternion::luaPinGet( const QUuid &pPinLocalId, lua_State *L )
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

int LuaQuaternion::luaPinSet( const QUuid &pPinLocalId, lua_State *L, int pIndex )
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
		return( luaL_error( L, "No quaternion pin" ) );
	}

	fugio::VariantInterface			*DstVar = qobject_cast<fugio::VariantInterface *>( Pin->control()->qobject() );

	if( !DstVar )
	{
		return( luaL_error( L, "Can't access quaternion" ) );
	}

	if( Q->mQuaternion != DstVar->variant().value<QQuaternion>() )
	{
		DstVar->setVariant( Q->mQuaternion );

		Pin->node()->context()->pinUpdated( Pin );
	}

	return( 0 );
}

int LuaQuaternion::luaAdd( lua_State *L )
{
	UserData	*Q1 = checkuserdata( L, 1 );
	UserData	*Q2 = checkuserdata( L, 2 );

	return( pushquaternion( L, Q1->mQuaternion + Q2->mQuaternion ) );
}

int LuaQuaternion::luaDiv( lua_State *L )
{
	UserData	*Q1 = checkuserdata( L, 1 );
	lua_Number	 Number = luaL_checknumber( L, 2 );

	return( pushquaternion( L, Q1->mQuaternion / Number ) );
}

int LuaQuaternion::luaEq( lua_State *L )
{
	UserData	*Q1 = checkuserdata( L, 1 );
	UserData	*Q2 = checkuserdata( L, 2 );

	lua_pushboolean( L, Q1->mQuaternion == Q2->mQuaternion );

	return( 1 );
}

int LuaQuaternion::luaMul(lua_State *L)
{
	UserData	*Q1 = checkuserdata( L, 1 );

	luaL_checkany( L, 2 );

	int			 NumberValid;
	lua_Number	 Number;

	Number = lua_tonumberx( L, 2, &NumberValid );

	if( NumberValid )
	{
		return( pushquaternion( L, Q1->mQuaternion * Number ) );
	}

	if( LuaVector3D::isVector3D( L, 2 ) )
	{
		QVector3D		V3 = LuaVector3D::checkvector3d( L, 2 );

		return( LuaVector3D::pushvector3d( L, Q1->mQuaternion * V3 ) );
	}

	UserData	*Q2 = checkuserdata( L, 2 );

	return( pushquaternion( L, Q1->mQuaternion * Q2->mQuaternion ) );
}

int LuaQuaternion::luaSub(lua_State *L)
{
	UserData	*Q1 = checkuserdata( L, 1 );
	UserData	*Q2 = checkuserdata( L, 2 );

	return( pushquaternion( L, Q1->mQuaternion - Q2->mQuaternion ) );
}

int LuaQuaternion::luaDotProduct(lua_State *L)
{
	UserData	*Q1 = checkuserdata( L, 1 );
	UserData	*Q2 = checkuserdata( L, 2 );

	lua_pushnumber( L, QQuaternion::dotProduct( Q1->mQuaternion, Q2->mQuaternion ) );

	return( 1 );
}

int LuaQuaternion::luaFromEulerAngles( lua_State *L )
{
	if( lua_gettop( L ) == 1 )
	{
		QVector3D	eulerAngles = LuaVector3D::checkvector3d( L, 1 );

		return( pushquaternion( L, QQuaternion::fromEulerAngles( eulerAngles ) ) );
	}

	if( lua_gettop( L ) == 3 )
	{
		float	pitch = luaL_checknumber( L, 1 );
		float	yaw = luaL_checknumber( L, 2 );
		float	roll = luaL_checknumber( L, 3 );

		return( pushquaternion( L, QQuaternion::fromEulerAngles( pitch, yaw, roll ) ) );
	}

	return( luaL_error( L, "incorrect arguments" ) );
}

int LuaQuaternion::luaFromRotationMatrix( lua_State *L )
{
	if( lua_gettop( L ) == 1 )
	{
		QMatrix3x3	rot3x3 = LuaMatrix4x4::checkMatrix4x4( L, 1 ).toGenericMatrix<3,3>();

		return( pushquaternion( L, QQuaternion::fromRotationMatrix( rot3x3 ) ) );
	}

	return( luaL_error( L, "incorrect arguments" ) );
}

int LuaQuaternion::luaIsIdentity(lua_State *L)
{
	UserData	*UD = checkuserdata( L );

	lua_pushboolean( L, UD->mQuaternion.isIdentity() );

	return( 1 );
}

int LuaQuaternion::luaIsNull(lua_State *L)
{
	UserData	*UD = checkuserdata( L );

	lua_pushboolean( L, UD->mQuaternion.isNull() );

	return( 1 );
}

int LuaQuaternion::luaLength( lua_State *L )
{
	UserData	*UD = checkuserdata( L );

	lua_pushnumber( L, UD->mQuaternion.length() );

	return( 1 );
}

int LuaQuaternion::luaLengthSquared(lua_State *L)
{
	UserData	*UD = checkuserdata( L );

	lua_pushnumber( L, UD->mQuaternion.lengthSquared() );

	return( 1 );
}

int LuaQuaternion::luaNormalize(lua_State *L)
{
	UserData	*UD = checkuserdata( L );

	UD->mQuaternion.normalize();

	return( 0 );
}

int LuaQuaternion::luaNormalized(lua_State *L)
{
	UserData	*UD = checkuserdata( L );

	return( pushquaternion( L, UD->mQuaternion.normalized() ) );
}

int LuaQuaternion::luaRotatedVector(lua_State *L)
{
	UserData	*UD = checkuserdata( L );
	QVector3D	 V3 = LuaVector3D::checkvector3d( L, 2 );

	return( LuaVector3D::pushvector3d( L, UD->mQuaternion.rotatedVector( V3 ) ) );
}

int LuaQuaternion::luaToEulerAngles(lua_State *L)
{
	UserData	*UD = checkuserdata( L );

	return( LuaVector3D::pushvector3d( L, UD->mQuaternion.toEulerAngles() ) );
}

int LuaQuaternion::luaToRotationMatrix(lua_State *L)
{
	UserData	*UD = checkuserdata( L );
	QMatrix3x3	 M3 = UD->mQuaternion.toRotationMatrix();
	QMatrix4x4	 M4( M3 );

	return( LuaMatrix4x4::pushmatrix4x4( L, M4 ) );
}

int LuaQuaternion::luaIndex( lua_State *L )
{
	UserData	*UD = checkuserdata( L );
	const char	*S = luaL_checkstring( L, 2 );

	if( !strcmp( S, "scalar" ) )
	{
		lua_pushnumber( L, UD->mQuaternion.scalar() );

		return( 1 );
	}

	if( !strcmp( S, "w" ) )
	{
		lua_pushnumber( L, UD->mQuaternion.scalar() );

		return( 1 );
	}

	if( !strcmp( S, "x" ) )
	{
		lua_pushnumber( L, UD->mQuaternion.x() );

		return( 1 );
	}

	if( !strcmp( S, "y" ) )
	{
		lua_pushnumber( L, UD->mQuaternion.y() );

		return( 1 );
	}

	if( !strcmp( S, "z" ) )
	{
		lua_pushnumber( L, UD->mQuaternion.z() );

		return( 1 );
	}

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

int LuaQuaternion::luaNewIndex(lua_State *L)
{
	UserData	*UD = checkuserdata( L );
	const char	*S = luaL_checkstring( L, 2 );

	if( !strcmp( S, "scalar" ) )
	{
		float	v = luaL_checknumber( L, 3 );

		UD->mQuaternion.setScalar( v );

		return( 0 );
	}

	if( !strcmp( S, "w" ) )
	{
		float	v = luaL_checknumber( L, 3 );

		UD->mQuaternion.setScalar( v );

		return( 0 );
	}

	if( !strcmp( S, "x" ) )
	{
		float	v = luaL_checknumber( L, 3 );

		UD->mQuaternion.setX( v );

		return( 0 );
	}

	if( !strcmp( S, "y" ) )
	{
		float	v = luaL_checknumber( L, 3 );

		UD->mQuaternion.setY( v );

		return( 0 );
	}

	if( !strcmp( S, "z" ) )
	{
		float	v = luaL_checknumber( L, 3 );

		UD->mQuaternion.setZ( v );

		return( 0 );
	}

	return( luaL_error( L, "unknown field" ) );
}

#if QT_VERSION >= QT_VERSION_CHECK( 5, 5, 0 )

int LuaQuaternion::luaConjugated( lua_State *L )
{
	UserData	*UD = checkuserdata( L );

	return( pushquaternion( L, UD->mQuaternion.conjugated() ) );
}

int LuaQuaternion::luaInverted( lua_State *L )
{
	UserData	*UD = checkuserdata( L );

	return( pushquaternion( L, UD->mQuaternion.inverted() ) );
}

#endif

#endif
