#include "luamatrix.h"

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

Q_DECLARE_METATYPE( Eigen::MatrixXd )

const char *LuaMatrix::mTypeName = "qt.matrix";

#if defined( LUA_SUPPORTED )

const luaL_Reg LuaMatrix::mLuaMetaMethods[] =
{
//	{ "__add",				LuaMatrix::luaAdd },
//	{ "__div",				LuaMatrix::luaDiv },
//	{ "__eq",				LuaMatrix::luaEq },
	{ "__mul",				LuaMatrix::luaMul },
//	{ "__sub",				LuaMatrix::luaSub },
	{ "__gc",				LuaMatrix::luaDelete },
	{ "__index",			LuaMatrix::luaIndex },
	{ "__newindex",			LuaMatrix::luaNewIndex },
	{ Q_NULLPTR, Q_NULLPTR }
};

const luaL_Reg LuaMatrix::mLuaMethods[] =
{
	{ "at",					LuaMatrix::luaAt },
	{ "set",				LuaMatrix::luaSet },
	{ "rows",				LuaMatrix::luaRows },
	{ "cols",				LuaMatrix::luaCols },
	{ Q_NULLPTR, Q_NULLPTR }
};

void LuaMatrix::registerExtension(LuaInterface *LUA)
{
	LuaQtPlugin::addLuaFunction( "matrix", LuaMatrix::luaNew );

	LUA->luaRegisterExtension( LuaMatrix::luaOpen );

	LUA->luaAddPinGet( PID_MATRIX, LuaMatrix::luaPinGet );

	LUA->luaAddPinSet( PID_MATRIX, LuaMatrix::luaPinSet );

	LUA->luaAddPushVariantFunction( QMetaType::Type( QMetaType::type( "Eigen::Matrix" ) ), LuaMatrix::pushVariant );

	LUA->luaAddPopVariantFunction( mTypeName, LuaMatrix::popVariant );
}

int LuaMatrix::luaOpen (lua_State *L )
{
	luaL_newmetatable( L, mTypeName );

	lua_pushvalue( L, -1 );
	lua_setfield( L, -2, "__index" );

	luaL_setfuncs( L, mLuaMethods, 0 );

	return( 1 );
}

int LuaMatrix::luaNew( lua_State *L )
{
	if( lua_gettop( L ) == 2 )
	{
		lua_Integer		C = luaL_checkinteger( L, 1 );
		lua_Integer		R = luaL_checkinteger( L, 2 );

		pushmatrix( L, Eigen::MatrixXd(	R, C ).setZero() );
	}
	else
	{
		pushmatrix( L, Eigen::MatrixXd().setZero() );
	}

	//luaL_getmetatable( L, UserData::TypeName );

	return( 1 );
}

int LuaMatrix::luaIndex( lua_State *L )
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

int LuaMatrix::luaNewIndex( lua_State *L )
{
	UserData	*UD = checkuserdata( L );
	int			 Index  = luaL_checkinteger( L, 2 );

	luaL_argcheck( L, Index >= 1 && Index <= 16, 2, "index must be 1-16" );

	lua_Number	 V = luaL_checknumber( L, 3 );

	Index = Index - 1;

	UD->mMatrix( Index / 4, Index % 4 ) = V;

	return( 0 );
}

int LuaMatrix::luaAt(lua_State *L)
{
	UserData	*UD = checkuserdata( L );
	int			 C  = luaL_checkinteger( L, 2 );
	int			 R  = luaL_checkinteger( L, 3 );

	luaL_argcheck( L, C >= 1 && C <= UD->mMatrix.cols(), 2, "cols is invalid" );
	luaL_argcheck( L, R >= 1 && R <= UD->mMatrix.rows(), 3, "rows is invalid" );

	lua_pushnumber( L, UD->mMatrix( R - 1, C - 1 ) );

	return( 1 );
}

int LuaMatrix::luaSet(lua_State *L)
{
	UserData	*UD = checkuserdata( L );
	int			 C  = luaL_checkinteger( L, 2 );
	int			 R  = luaL_checkinteger( L, 3 );
	lua_Number	 V  = luaL_checknumber( L, 4 );

	luaL_argcheck( L, C >= 1 && C <= UD->mMatrix.cols(), 2, "cols is invalid" );
	luaL_argcheck( L, R >= 1 && R <= UD->mMatrix.rows(), 3, "rows is invalid" );

	UD->mMatrix( R - 1, C - 1 ) = V;

	return( 0 );
}

int LuaMatrix::luaRows( lua_State *L )
{
	UserData	*UD = checkuserdata( L );

	lua_pushinteger( L, UD->mMatrix.rows() );

	return( 1 );
}

int LuaMatrix::luaCols( lua_State *L )
{
	UserData	*UD = checkuserdata( L );

	lua_pushinteger( L, UD->mMatrix.cols() );

	return( 1 );
}

int LuaMatrix::luaMul( lua_State *L )
{
	UserData	*MatrixData = checkuserdata( L );

	luaL_checkany( L, 2 );

	if( isMatrix( L, 2 ) )
	{
		UserData	*Matrix2 = checkuserdata( L, 2 );
		Eigen::MatrixXd			 Matrix3 = MatrixData->mMatrix * Matrix2->mMatrix;

		pushmatrix( L, Matrix3 );

		return( 1 );
	}

	if( LuaPointF::isPointF( L, 2 ) )
	{
		QPointF				 Point1 = LuaPointF::checkpointf( L, 2 );

		Eigen::Vector2d		 Vector2 = MatrixData->mMatrix * Eigen::Vector2d( Point1.x(), Point1.y() );

		LuaPointF::pushpointf( L, QPointF( Vector2.x(), Vector2.y() ) );

		return( 1 );
	}

	if( lua_type( L, 2 ) == LUA_TNUMBER )
	{
		Eigen::MatrixXd			 Matrix2 = MatrixData->mMatrix * lua_tonumber( L, 2 );

		pushmatrix( L, Matrix2 );

		return( 1 );
	}

	luaL_argerror( L, 2, "Bad argument type" );

	return( 0 );
}

int LuaMatrix::luaDelete(lua_State *L)
{
	UserData	*UD = checkuserdata( L );

	UD->mMatrix.~Matrix();

	return( 0 );
}

int LuaMatrix::luaFrustum( lua_State *S )
{
	UserData	*MatrixData = checkuserdata( S );

	float				 L, R, B, T, N, F;

	L = luaL_checknumber( S, 2 );
	R = luaL_checknumber( S, 3 );
	B = luaL_checknumber( S, 4 );
	T = luaL_checknumber( S, 5 );
	N = luaL_checknumber( S, 6 );
	F = luaL_checknumber( S, 7 );

//	MatrixData->mMatrix.frustum( L, R, B, T, N, F );

	return( 0 );
}

int LuaMatrix::luaOrtho( lua_State *L )
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

//	MatrixData->mMatrix.ortho( R.left(), R.right(), R.bottom(), R.top(), N, F );

	return( 0 );
}

int LuaMatrix::luaPerspective(lua_State *L)
{
	UserData	*MatrixData = checkuserdata( L );

	float				 A, R, N, F;

	A = luaL_checknumber( L, 2 );
	R = luaL_checknumber( L, 3 );
	N = luaL_checknumber( L, 4 );
	F = luaL_checknumber( L, 5 );

//	MatrixData->mMatrix.perspective( A, R, N, F );

	return( 0 );
}

int LuaMatrix::luaRotate(lua_State *L)
{
	UserData	*MatrixData = checkuserdata( L );

	float		Angle = luaL_checknumber( L, 2 );

	QVector3D	Axis;

	if( lua_gettop( L ) >= 3 ) Axis.setX( luaL_checknumber( L, 3 ) );
	if( lua_gettop( L ) >= 4 ) Axis.setY( luaL_checknumber( L, 4 ) );
	if( lua_gettop( L ) >= 5 ) Axis.setZ( luaL_checknumber( L, 5 ) );

//	MatrixData->mMatrix.rotate( Angle, Axis );

	return( 0 );
}

int LuaMatrix::luaScale(lua_State *L)
{
	UserData	*MatrixData = checkuserdata( L );

	float				 F = luaL_checknumber( L, 2 );

	if( lua_gettop( L ) > 2 )
	{
		QVector3D	Scale( F, 1, 1 );

		if( lua_gettop( L ) >= 3 ) Scale.setY( luaL_checknumber( L, 3 ) );
		if( lua_gettop( L ) >= 4 ) Scale.setZ( luaL_checknumber( L, 4 ) );

//		MatrixData->mMatrix.scale( Scale );
	}
	else
	{
//		MatrixData->mMatrix.scale( F );
	}

	return( 0 );
}

int LuaMatrix::luaTranslate( lua_State *L )
{
	UserData	*MatrixData = checkuserdata( L );

	QVector3D	Translate;

	if( lua_gettop( L ) >= 2 ) Translate.setX( luaL_checknumber( L, 2 ) );
	if( lua_gettop( L ) >= 3 ) Translate.setY( luaL_checknumber( L, 3 ) );
	if( lua_gettop( L ) >= 4 ) Translate.setZ( luaL_checknumber( L, 4 ) );

//	MatrixData->mMatrix.translate( Translate );

	return( 0 );
}

int LuaMatrix::luaLookAt( lua_State *L )
{
	UserData	*MatrixData = checkuserdata( L );

	QVector3D	Eye    = LuaVector3D::checkvector3d( L, 2 );
	QVector3D	Center = LuaVector3D::checkvector3d( L, 3 );
	QVector3D	Up     = LuaVector3D::checkvector3d( L, 4 );

//	MatrixData->mMatrix.lookAt( Eye, Center, Up );

	return( 0 );
}

#if QT_VERSION >= QT_VERSION_CHECK( 5, 5, 0 )
int LuaMatrix::luaIsAffine(lua_State *L)
{
	UserData	*MatrixData = checkuserdata( L );

//	lua_pushboolean( L, MatrixData->mMatrix.isAffine() );

	return( 0 );
}
#endif

int LuaMatrix::luaIsIdentity(lua_State *L)
{
	UserData	*MatrixData = checkuserdata( L );

	lua_pushboolean( L, MatrixData->mMatrix.isIdentity() );

	return( 1 );
}

int LuaMatrix::luaToArray( lua_State *L )
{
	UserData			*MatrixData = checkuserdata( L );
//	const float			*MatDat = MatrixData->mMatrix.constData();

	lua_newtable( L );

	for( int i = 0 ; i < 16 ; i++ )
	{
//		lua_pushnumber( L, MatDat[ i ] );

//		lua_rawseti( L, -2, i + 1 );
	}

	return( 1 );
}

int LuaMatrix::luaPinGet( const QUuid &pPinLocalId, lua_State *L )
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

	return( pushmatrix( L, SrcVar->variant().value<Eigen::MatrixXd>() ) );
}

int LuaMatrix::luaPinSet( const QUuid &pPinLocalId, lua_State *L, int pIndex )
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

	//if( UD->mMatrix != DstVar->variant().value<Eigen::MatrixXd>() )
	{
		QVariant		V;

		V.setValue( UD->mMatrix );

		DstVar->setVariant( V );

		Pin->node()->context()->pinUpdated( Pin );
	}

	return( 0 );
}

int LuaMatrix::pushVariant( lua_State *L, const QVariant &V )
{
	return( pushmatrix( L, V.value<Eigen::MatrixXd>() ) );
}

QVariant LuaMatrix::popVariant( lua_State *L, int pIndex )
{
	UserData *UD = checkuserdata( L, pIndex );

	QVariant	V;

	V.setValue( UD->mMatrix );

	return( V );
}

#endif
