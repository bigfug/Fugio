#ifndef LUAVECTOR3D_H
#define LUAVECTOR3D_H

#if defined( LUA_SUPPORTED )
#include <lua.hpp>
#endif

#include <QUuid>
#include <QVector3D>

class LuaVector3D
{
private:
	typedef struct UserData
	{
		static const char *TypeName;

		qreal	x, y;

		QVector3D			mVector3D;

	} UserData;

public:
	LuaVector3D( void ) {}

	virtual ~LuaVector3D( void ) {}

#if defined( LUA_SUPPORTED )
	static int luaOpen( lua_State *L );

	static int luaNew( lua_State *L );

	static int luaPinGet( const QUuid &pPinLocalId, lua_State *L );
	static int luaPinSet( const QUuid &pPinLocalId, lua_State *L, int pIndex );

	static int pushvector3d( lua_State *L, const QVector3D &pVector3D )
	{
		UserData	*UD = (UserData *)lua_newuserdata( L, sizeof( UserData ) );

		if( !UD )
		{
			return( 0 );
		}

		luaL_getmetatable( L, UserData::TypeName );
		lua_setmetatable( L, -2 );

		new( &UD->mVector3D ) QVector3D( pVector3D );

		return( 1 );
	}

	static bool isVector3D( lua_State *L, int i = 1 )
	{
		return( luaL_testudata( L, i, UserData::TypeName ) != nullptr );
	}

	static QVector3D checkvector3d( lua_State *L, int i = 1 )
	{
		UserData *UD = checkuserdata( L, i );

		return( UD->mVector3D );
	}

private:
	static UserData *checkuserdata( lua_State *L, int i = 1 )
	{
		UserData *UD = (UserData *)luaL_checkudata( L, i, UserData::TypeName );

		luaL_argcheck( L, UD != NULL, i, "Vector3D expected" );

		return( UD );
	}

	static int luaCrossProduct( lua_State *L );
	static int luaDotProduct( lua_State *L );

//	static int luaAdd( lua_State *L );
//	static int luaDiv( lua_State *L );
//	static int luaEq( lua_State *L );
//	static int luaMul( lua_State *L );
	static int luaSub( lua_State *L );

	static int luaIndex( lua_State *L );
	static int luaNewIndex( lua_State *L );

	static int luaLength( lua_State *L );
	static int luaToArray( lua_State *L );
	static int luaNormalize( lua_State *L );
	static int luaNormalized( lua_State *L );
	static int luaX( lua_State *L );
	static int luaY( lua_State *L );
	static int luaZ( lua_State *L );

private:
	static const luaL_Reg					mLuaFunctions[];
	static const luaL_Reg					mLuaMetaMethods[];
	static const luaL_Reg					mLuaMethods[];
#endif
};


#endif // LUAVECTOR3D_H
