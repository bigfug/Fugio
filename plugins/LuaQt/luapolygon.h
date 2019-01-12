#ifndef LUAPOLYGON_H
#define LUAPOLYGON_H

#if defined( LUA_SUPPORTED )
#include <lua.hpp>
#endif

#include <fugio/lua/lua_interface.h>

#include <QVariant>
#include <QPolygonF>

class LuaPolygon
{
private:
	static const char *mTypeName;

	typedef struct UserData
	{
		QPolygonF	mPolygon;
	} UserData;

public:
	LuaPolygon( void ) {}

	virtual ~LuaPolygon( void ) {}

#if defined( LUA_SUPPORTED )
	static void registerExtension( fugio::LuaInterface *LUA );

	static int luaOpen( lua_State *L );

	static int luaNew( lua_State *L );

	static int pushpolygon( lua_State *L, const QPolygonF &pPolygon )
	{
		UserData	*UD = static_cast<UserData *>( lua_newuserdata( L, sizeof( UserData ) ) );

		if( !UD )
		{
			return( 0 );
		}

		luaL_getmetatable( L,mTypeName );
		lua_setmetatable( L, -2 );

		new( &UD->mPolygon ) QPolygonF( pPolygon );

		return( 1 );
	}

	static bool isPolygon( lua_State *L, int i = 1 )
	{
		return( luaL_testudata( L, i, mTypeName ) != nullptr );
	}

	static QPolygonF *checkpolygon( lua_State *L, int i = 1 )
	{
		void *ud = luaL_checkudata( L, i, mTypeName );

		luaL_argcheck( L, ud != NULL, i, "Polygon expected" );

		return( &static_cast<UserData *>( ud )->mPolygon );
	}

private:
	static UserData *checkuserdata( lua_State *L, int i = 1 )
	{
		UserData *UD = static_cast<UserData *>( luaL_checkudata( L, i, mTypeName ) );

		luaL_argcheck( L, UD != NULL, i, "Polygon expected" );

		return( UD );
	}

	static int pushVariant( lua_State *L, const QVariant &V )
	{
		return( pushpolygon( L, V.value<QPolygonF>() ) );
	}

	static QVariant popVariant( lua_State *L, int pIndex )
	{
		UserData *UD = checkuserdata( L, pIndex );

		return( UD ? UD->mPolygon : QVariant() );
	}

	static int luaPinGet( const QUuid &pPinLocalId, lua_State *L );
	static int luaPinSet( const QUuid &pPinLocalId, lua_State *L, int pIndex );

	static int luaToString( lua_State *L );

	static int luaDelete( lua_State *L );
	static int luaLen( lua_State *L );
	static int luaAppend( lua_State *L );

	static int luaBoundingRect( lua_State *L );
	static int luaContainsPoint( lua_State *L );

	static int luaIndex( lua_State *L );

	static int luaNewIndex( lua_State *L );

private:
	static const luaL_Reg					mLuaFunctions[];
	static const luaL_Reg					mLuaMetaMethods[];
	static const luaL_Reg					mLuaMethods[];
#endif
};

#endif // LUAPOLYGON_H
