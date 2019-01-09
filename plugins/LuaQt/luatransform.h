#ifndef LUATRANSFORM_H
#define LUATRANSFORM_H

#if defined( LUA_SUPPORTED )
#include <lua.hpp>
#endif

#include <QTransform>

#include <fugio/lua/lua_interface.h>

class LuaTransform
{
private:
	typedef struct TransformUserData
	{
		static const char	*TypeName;

		QTransform			*mTransform;
	} TransformUserData;

public:
	LuaTransform( void ) {}

	virtual ~LuaTransform( void ) {}

#if defined( LUA_SUPPORTED )
	static void registerExtension( fugio::LuaInterface *LUA );

	static int luaOpen( lua_State *L );

	static int luaNew( lua_State *L );

	static int pushtransform( lua_State *L, const QTransform &pTransform )
	{
		TransformUserData	*UD = (TransformUserData *)lua_newuserdata( L, sizeof( TransformUserData ) );

		if( !UD )
		{
			return( 0 );
		}

		luaL_getmetatable( L, TransformUserData::TypeName );
		lua_setmetatable( L, -2 );

		UD->mTransform = new QTransform( pTransform );

		return( 1 );
	}

	static bool isTransform( lua_State *L, int i = 1 )
	{
		return( luaL_testudata( L, i, TransformUserData::TypeName ) != nullptr );
	}

	static TransformUserData *checktransformdata( lua_State *L, int i = 1 )
	{
		void *ud = luaL_checkudata( L, i, TransformUserData::TypeName );

		luaL_argcheck( L, ud != NULL, i, "Transform expected" );

		return( static_cast<TransformUserData *>( ud ) );
	}

	static QTransform *checktransform( lua_State *L, int i = 1 )
	{
		TransformUserData	*FUD = checktransformdata( L, i );

		return( FUD ? FUD->mTransform : nullptr );
	}

private:
	static int luaDelete( lua_State *L );

private:
	static const luaL_Reg					mLuaInstance[];
	static const luaL_Reg					mLuaMethods[];
#endif
};


#endif // LUATRANSFORM_H
