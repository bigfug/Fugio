#ifndef LUAARRAY_H
#define LUAARRAY_H

#if defined( LUA_SUPPORTED )
#include <lua.hpp>
#endif

#include <QObject>

#include <fugio/lua/lua_interface.h>
#include <fugio/core/variant_interface.h>

class LuaArray
{
public:
	LuaArray( void )
	{

	}

	virtual ~LuaArray( void )
	{

	}

#if defined( LUA_SUPPORTED )
private:
	typedef struct LuaArrayUserData
	{
		QObject		*mObject;
		bool		 mReadOnly;
	} LuaArrayUserData;

	// LuaExtension interface
public:
	static int lua_openarray( lua_State *L );

	static int pusharray( lua_State *L, QObject *O, bool pReadOnly )
	{
		LuaArrayUserData	*UD = (LuaArrayUserData *)lua_newuserdata( L, sizeof( LuaArrayUserData ) );

		if( !UD )
		{
			return( 0 );
		}

		UD->mObject = O;
		UD->mReadOnly = pReadOnly;

		luaL_getmetatable( L, "fugio.array" );
		lua_setmetatable( L, -2 );

		return( 1 );
	}

	static LuaArrayUserData *checkarray( lua_State *L, int i = 1 )
	{
		LuaArrayUserData	*UD = (LuaArrayUserData *)luaL_checkudata( L, i, "fugio.array" );

		luaL_argcheck( L, UD, i, "Array expected" );

		return( UD );
	}

private:
	static void pushFloatArray( lua_State *L, float *A, int LstIdx, int LstCnt );
	static void pushIntArray( lua_State *L, float *A, int LstIdx, int LstCnt );

	static int setIndex( lua_State *L, LuaArrayUserData *LstDat, const int LstIdx );
	static int setTable( lua_State *L, LuaArrayUserData *LstDat );
	static void setArrayIndex( lua_State *L, fugio::VariantInterface *ArrInt, int LstIdx, int ValIdx );

private:
	static int luaGet( lua_State *L );
	static int luaSet( lua_State *L );
	static int luaLen( lua_State *L );
	static int luaToArray( lua_State *L );
	static int luaReserve( lua_State *L );
	static int luaResize( lua_State *L );
	static int luaSetCount( lua_State *L );
	static int luaSetTable( lua_State *L );
	static int luaSetType( lua_State *L );

private:
	static const luaL_Reg					mLuaInstance[];
	static const luaL_Reg					mLuaFunctions[];
#endif
};

#endif // LUAARRAY_H
