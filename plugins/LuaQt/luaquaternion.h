#ifndef LUAQUATERNION_H
#define LUAQUATERNION_H

#if defined( LUA_SUPPORTED )
#include <lua.hpp>
#endif

#include <QQuaternion>

#include <fugio/lua/lua_interface.h>

class LuaQuaternion
{
private:
	typedef struct UserData
	{
		QQuaternion			mQuaternion;
	} UserData;

	static const char *mTypeName;

public:
	LuaQuaternion() {}

#if defined( LUA_SUPPORTED )
	static void registerExtension( fugio::LuaInterface *LUA );

	static int luaOpen( lua_State *L );

	static int luaNew( lua_State *L );

	static int pushquaternion( lua_State *L, const QQuaternion &pQ )
	{
		UserData	*UD = (UserData *)lua_newuserdata( L, sizeof( UserData ) );

		if( !UD )
		{
			return( 0 );
		}

		luaL_getmetatable( L,mTypeName );
		lua_setmetatable( L, -2 );

		new( &UD->mQuaternion ) QQuaternion( pQ );

		return( 1 );
	}

	static bool isQuaternion( lua_State *L, int i = 1 )
	{
		return( luaL_testudata( L, i, mTypeName ) != nullptr );
	}

	static QQuaternion checkQuaternion( lua_State *L, int i = 1 )
	{
		UserData *UD = checkuserdata( L, i );

		return( UD->mQuaternion );
	}

	static int luaPinGet( const QUuid &pPinLocalId, lua_State *L );
	static int luaPinSet( const QUuid &pPinLocalId, lua_State *L, int pIndex );

private:
	static UserData *checkuserdata( lua_State *L, int i = 1 )
	{
		UserData *UD = (UserData *)luaL_checkudata( L, i, mTypeName );

		luaL_argcheck( L, UD != NULL, i, "Quaternion expected" );

		return( UD );
	}

	static int luaAdd( lua_State *L );
	static int luaDiv( lua_State *L );
	static int luaEq( lua_State *L );
	static int luaMul( lua_State *L );
	static int luaSub( lua_State *L );

	static int luaDotProduct( lua_State *L );

	static int luaFromEulerAngles( lua_State *L );
	static int luaFromRotationMatrix( lua_State *L );

	static int luaIsIdentity( lua_State *L );
	static int luaIsNull( lua_State *L );

	static int luaLength( lua_State *L );
	static int luaLengthSquared( lua_State *L );

	static int luaNormalize( lua_State *L );
	static int luaNormalized( lua_State *L );

	static int luaRotatedVector( lua_State *L );

	static int luaToEulerAngles( lua_State *L );
	static int luaToRotationMatrix( lua_State *L );

#if QT_VERSION >= QT_VERSION_CHECK( 5, 5, 0 )
	static int luaConjugated( lua_State *L );
	static int luaInverted( lua_State *L );
#endif

	static int luaIndex( lua_State *L );
	static int luaNewIndex( lua_State *L );

private:
	static const luaL_Reg					mLuaFunctions[];
	static const luaL_Reg					mLuaMetaMethods[];
	static const luaL_Reg					mLuaMethods[];
#endif
};

#endif // LUAQUATERNION_H
