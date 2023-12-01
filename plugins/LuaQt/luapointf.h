#ifndef LUAPOINTF_H
#define LUAPOINTF_H

#if defined( LUA_SUPPORTED )
#include <lua.hpp>
#endif

#include <fugio/lua/lua_interface.h>

#include <QUuid>
#include <QPointF>
#include <QVariant>

class LuaPointF
{
private:
	typedef struct UserData
	{
		QPointF		mPoint;

	} UserData;

public:
	static const char *mTypeName;

	LuaPointF( void ) {}

	virtual ~LuaPointF( void ) {}

#if defined( LUA_SUPPORTED )
	static void registerExtension( fugio::LuaInterface *LUA );

	static int luaOpen( lua_State *L );

	static int luaNew( lua_State *L );

	static int luaPinGet( const QUuid &pPinLocalId, lua_State *L );
	static int luaPinSet( const QUuid &pPinLocalId, lua_State *L, int pIndex );

	static int pushpointf( lua_State *L, const QPointF &pPoint )
	{
		UserData	*UD = static_cast<UserData *>( lua_newuserdata( L, sizeof( UserData ) ) );

		if( !UD )
		{
			return( 0 );
		}

		luaL_getmetatable( L,mTypeName );
		lua_setmetatable( L, -2 );

		new( &UD->mPoint ) QPointF( pPoint );

		return( 1 );
	}

	static bool isPointF( lua_State *L, int i = 1 )
	{
		return( luaL_testudata( L, i, LuaPointF::mTypeName ) != nullptr );
	}

	static QPointF checkpointf( lua_State *L, int i = 1 )
	{
		UserData *UD = checkuserdata( L, i );

		return( UD->mPoint );
	}

	static int pushVariant( lua_State *L, const QVariant &V )
	{
		return( pushpointf( L, V.toPointF() ) );
	}

	static QVariant popVariant( lua_State *L, int pIndex )
	{
		UserData *UD = checkuserdata( L, pIndex );

		return( UD ? UD->mPoint : QVariant() );
	}

private:
	static UserData *checkuserdata( lua_State *L, int i = 1 )
	{
		UserData *UD = (UserData *)luaL_checkudata( L, i, LuaPointF::mTypeName );

		luaL_argcheck( L, UD != NULL, i, "Point expected" );

		return( UD );
	}

	static int luaDelete( lua_State *L );

	static int luaDotProduct( lua_State *L );

	static int luaAdd( lua_State *L );
	static int luaDiv( lua_State *L );
	static int luaEq( lua_State *L );
	static int luaMul( lua_State *L );
	static int luaSub( lua_State *L );

	static int luaIsNull( lua_State *L );
	static int luaManhattanLength( lua_State *L );
	static int luaSetX( lua_State *L );
	static int luaSetY( lua_State *L );
	static int luaToArray( lua_State *L );
	static int luaX( lua_State *L );
	static int luaY( lua_State *L );

private:
	static const luaL_Reg					mLuaInstance[];
	static const luaL_Reg					mLuaMethods[];
#endif
};

#endif // LUAPOINTF_H
