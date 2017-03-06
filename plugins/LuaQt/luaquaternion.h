#ifndef LUAQUATERNION_H
#define LUAQUATERNION_H

#if defined( LUA_SUPPORTED )
#include <lua.hpp>
#endif

#include <QQuaternion>

class LuaQuaternion
{
private:
	typedef struct UserData
	{
		static const char *TypeName;

		QQuaternion			mQuaternion;
	} UserData;

public:
	LuaQuaternion() {}

#if defined( LUA_SUPPORTED )
	static int luaOpen( lua_State *L );

	static int luaNew( lua_State *L );

	static int pushquaternion( lua_State *L, const QQuaternion &pQ )
	{
		UserData	*UD = (UserData *)lua_newuserdata( L, sizeof( UserData ) );

		if( !UD )
		{
			return( 0 );
		}

		luaL_getmetatable( L, UserData::TypeName );
		lua_setmetatable( L, -2 );

		new( &UD->mQuaternion ) QQuaternion( pQ );

		return( 1 );
	}

	static bool isQuaternion( lua_State *L, int i = 1 )
	{
		return( luaL_testudata( L, i, UserData::TypeName ) != nullptr );
	}

	static QQuaternion checkQuaternion( lua_State *L, int i = 1 )
	{
		UserData *UD = checkuserdata( L, i );

		return( UD->mQuaternion );
	}

	static int luaPinGet( const QUuid &pPinLocalId, lua_State *L );

private:
	static UserData *checkuserdata( lua_State *L, int i = 1 )
	{
		UserData *UD = (UserData *)luaL_checkudata( L, i, UserData::TypeName );

		luaL_argcheck( L, UD != NULL, i, "Quaternion expected" );

		return( UD );
	}

	static int luaLength( lua_State *L );

#if QT_VERSION >= QT_VERSION_CHECK( 5, 5, 0 )
	static int luaConjugated( lua_State *L );
#endif

private:
	static const luaL_Reg					mLuaInstance[];
	static const luaL_Reg					mLuaMethods[];
#endif
};

#endif // LUAQUATERNION_H
