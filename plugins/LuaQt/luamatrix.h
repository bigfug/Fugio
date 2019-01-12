#ifndef LUAMATRIX_H
#define LUAMATRIX_H

#if defined( LUA_SUPPORTED )
#include <lua.hpp>
#endif

#include <QVariant>

#include <Eigen/Dense>

#include <fugio/lua/lua_interface.h>

class LuaMatrix
{
private:
	typedef struct UserData
	{
		Eigen::MatrixXd		mMatrix;

	} UserData;

	static const char *mTypeName;

public:
	LuaMatrix( void ) {}

	virtual ~LuaMatrix( void ) {}

#if defined( LUA_SUPPORTED )
	static void registerExtension( fugio::LuaInterface *LUA );

	static int luaOpen( lua_State *L );

	static int luaNew( lua_State *L );

	static int pushmatrix( lua_State *L, const Eigen::MatrixXd &pMatrix )
	{
		UserData	*UD = (UserData *)lua_newuserdata( L, sizeof( UserData ) );

		if( !UD )
		{
			return( 0 );
		}

		luaL_getmetatable( L, mTypeName );
		lua_setmetatable( L, -2 );

		new( &UD->mMatrix ) Eigen::MatrixXd( pMatrix );

		return( 1 );
	}

	static bool isMatrix( lua_State *L, int i = 1 )
	{
		return( luaL_testudata( L, i, mTypeName ) != nullptr );
	}

	static Eigen::MatrixXd checkMatrix( lua_State *L, int i = 1 )
	{
		UserData *UD = checkuserdata( L, i );

		return( UD->mMatrix );
	}

	static int luaPinGet( const QUuid &pPinLocalId, lua_State *L );
	static int luaPinSet( const QUuid &pPinLocalId, lua_State *L, int pIndex );

private:
	static UserData *checkuserdata( lua_State *L, int i = 1 )
	{
		UserData *UD = (UserData *)luaL_checkudata( L, i, mTypeName );

		luaL_argcheck( L, UD != Q_NULLPTR, i, "Matrix expected" );

		return( UD );
	}

	static int pushVariant( lua_State *L, const QVariant &V );

	static QVariant popVariant( lua_State *L, int pIndex );

//	static int luaAdd( lua_State *L );
//	static int luaDiv( lua_State *L );
//	static int luaEq( lua_State *L );
	static int luaMul( lua_State *L );
//	static int luaSub( lua_State *L );
	static int luaDelete( lua_State *L );

	static int luaIndex( lua_State *L );
	static int luaNewIndex( lua_State *L );

	static int luaAt( lua_State *L );
	static int luaSet( lua_State *L );
	static int luaRows( lua_State *L );
	static int luaCols( lua_State *L );

	static int luaFrustum( lua_State *L );
	static int luaOrtho( lua_State *L );
	static int luaPerspective( lua_State *L );

	static int luaRotate( lua_State *L );
	static int luaScale( lua_State *L );
	static int luaTranslate( lua_State *L );

	static int luaLookAt( lua_State *L );

#if QT_VERSION >= QT_VERSION_CHECK( 5, 5, 0 )
	static int luaIsAffine( lua_State *L );
#endif

	static int luaIsIdentity( lua_State *L );

	static int luaToArray( lua_State *L );

private:
	static const luaL_Reg					mLuaFunctions[];
	static const luaL_Reg					mLuaMetaMethods[];
	static const luaL_Reg					mLuaMethods[];
#endif
};

#endif // LUAMATRIX_H
