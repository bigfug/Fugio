#ifndef LUAGRADIENT_H
#define LUAGRADIENT_H

#if defined( LUA_SUPPORTED )
#include <lua.hpp>
#endif

#include <fugio/lua/lua_interface.h>

#include <QGradient>
#include <QConicalGradient>

class LuaGradient
{
private:
	typedef struct GradientUserData
	{
		static const char		*TypeName;

		QGradient				*mGradient;
	} GradientUserData;

public:
	LuaGradient( void ) {}

	virtual ~LuaGradient( void ) {}

#if defined( LUA_SUPPORTED )
	static void registerExtension( fugio::LuaInterface *LUA );

	static int luaOpen( lua_State *L );

	static int luaNew( lua_State *L );

	static int pushgradient( lua_State *L, QGradient *pGradient )
	{
		GradientUserData	*UD = (GradientUserData *)lua_newuserdata( L, sizeof( GradientUserData ) );

		if( !UD )
		{
			return( 0 );
		}

		luaL_getmetatable( L, GradientUserData::TypeName );
		lua_setmetatable( L, -2 );

		UD->mGradient = pGradient;

		return( 1 );
	}

	static bool isGradient( lua_State *L, int i = 1 )
	{
		return( luaL_testudata( L, i, GradientUserData::TypeName ) != nullptr );
	}

	static GradientUserData *checkgradientdata( lua_State *L, int i = 1 )
	{
		void *ud = luaL_checkudata( L, i, GradientUserData::TypeName );

		luaL_argcheck( L, ud != NULL, i, "Gradient expected" );

		return( static_cast<GradientUserData *>( ud ) );
	}

	static QGradient *checkgradient( lua_State *L, int i = 1 )
	{
		GradientUserData	*FUD = checkgradientdata( L, i );

		return( FUD->mGradient );
	}

private:
	static int luaDelete( lua_State *L );

	static int luaSetColorAt( lua_State *L );

private:
	static const luaL_Reg					mLuaInstance[];
	static const luaL_Reg					mLuaMethods[];
#endif
};

#endif // LUAGRADIENT_H
