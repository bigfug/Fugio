#ifndef LUAMATRIX4X4_H
#define LUAMATRIX4X4_H

#if defined( LUA_SUPPORTED )
#include <lua.hpp>
#endif

#include <QMatrix4x4>

class LuaMatrix4x4
{
private:
	typedef struct Matrix4x4UserData
	{
		static const char *TypeName;

		QMatrix4x4			mMatrix;

	} QMatrix4x4UserData;

public:
	LuaMatrix4x4( void ) {}

	virtual ~LuaMatrix4x4( void ) {}

#if defined( LUA_SUPPORTED )
	static int luaOpen( lua_State *L );

	static int luaNew( lua_State *L );

	static int pushmatrix4x4( lua_State *L, const QMatrix4x4 &pMatrix )
	{
		Matrix4x4UserData	*UD = (Matrix4x4UserData *)lua_newuserdata( L, sizeof( Matrix4x4UserData ) );

		if( !UD )
		{
			return( 0 );
		}

		luaL_getmetatable( L, Matrix4x4UserData::TypeName );
		lua_setmetatable( L, -2 );

		new( &UD->mMatrix ) QMatrix4x4( pMatrix );

		return( 1 );
	}

	static bool isMatrix4x4( lua_State *L, int i = 1 )
	{
		return( luaL_testudata( L, i, Matrix4x4UserData::TypeName ) != nullptr );
	}

	static QMatrix4x4 checkMatrix4x4( lua_State *L, int i = 1 )
	{
		Matrix4x4UserData *UD = checkMatrix4x4userdata( L, i );

		return( UD->mMatrix );
	}

	static int luaPinGet( const QUuid &pPinLocalId, lua_State *L );

private:
	static Matrix4x4UserData *checkMatrix4x4userdata( lua_State *L, int i = 1 )
	{
		Matrix4x4UserData *UD = (Matrix4x4UserData *)luaL_checkudata( L, i, Matrix4x4UserData::TypeName );

		luaL_argcheck( L, UD != NULL, i, "Point expected" );

		return( UD );
	}

//	static int luaAdd( lua_State *L );
//	static int luaDiv( lua_State *L );
//	static int luaEq( lua_State *L );
	static int luaMul( lua_State *L );
//	static int luaSub( lua_State *L );

	static int luaOrtho( lua_State *L );
	static int luaPerspective( lua_State *L );

	static int luaRotate( lua_State *L );
	static int luaScale( lua_State *L );
	static int luaTranslate( lua_State *L );

	static int luaIsAffine( lua_State *L );
	static int luaIsIdentity( lua_State *L );

	static int luaToArray( lua_State *L );

private:
	static const luaL_Reg					mLuaInstance[];
	static const luaL_Reg					mLuaMethods[];
#endif
};
#endif // LUAMATRIX4X4_H
