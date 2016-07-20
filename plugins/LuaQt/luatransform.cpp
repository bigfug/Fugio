#include "luatransform.h"

const char *LuaTransform::TransformUserData::TypeName = "qt.transform";

const luaL_Reg LuaTransform::mLuaInstance[] =
{
	{ 0, 0 }
};

const luaL_Reg LuaTransform::mLuaMethods[] =
{
	{ "__gc",				LuaTransform::luaDelete },
	{ 0, 0 }
};

int LuaTransform::luaOpen( lua_State *L )
{
	luaL_newmetatable( L, TransformUserData::TypeName );

	lua_pushvalue( L, -1 );
	lua_setfield( L, -2, "__index" );

	luaL_setfuncs( L, mLuaMethods, 0 );

	luaL_newlib( L, mLuaInstance );

	return( 1 );
}

int LuaTransform::luaNew( lua_State *L )
{
	pushtransform( L, QTransform() );

	return( 1 );
}

int LuaTransform::luaDelete( lua_State *L )
{
	TransformUserData		*UserData = checktransformdata( L );

	if( UserData )
	{
		if( UserData->mTransform )
		{
			delete UserData->mTransform;

			UserData->mTransform = nullptr;
		}
	}

	return( 0 );
}
