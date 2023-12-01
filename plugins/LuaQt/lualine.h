#ifndef LUALINE_H
#define LUALINE_H

#if defined( LUA_SUPPORTED )
#include <lua.hpp>
#endif

#include <fugio/lua/lua_interface.h>

#include <QUuid>
#include <QLineF>
#include <QVariant>

class LuaLine
{
private:
	typedef struct UserData
	{
		QLineF				mLine;
	} UserData;

public:
	static const char *mTypeName;

	LuaLine( void ) {}

	virtual ~LuaLine( void ) {}

#if defined( LUA_SUPPORTED )
	static void registerExtension( fugio::LuaInterface *LUA );

	static int luaOpen( lua_State *L );

	static int luaNew( lua_State *L );
	static int luaNewQt( lua_State *L );

	static int pushline( lua_State *L, const QLineF &pLine )
	{
		UserData	*UD = (UserData *)lua_newuserdata( L, sizeof( UserData ) );

		if( !UD )
		{
			return( 0 );
		}

		luaL_getmetatable( L, mTypeName );
		lua_setmetatable( L, -2 );

		new( &UD->mLine ) QLineF( pLine );

		return( 1 );
	}

	static int pushVariant( lua_State *L, const QVariant &V )
	{
		return( pushline( L, V.toLineF() ) );
	}

	static QVariant popVariant( lua_State *L, int pIndex )
	{
		UserData *UD = checkuserdata( L, pIndex );

		return( UD ? UD->mLine : QVariant() );
	}

	static bool isLine( lua_State *L, int i = 1 )
	{
		return( luaL_testudata( L, i, mTypeName ) != nullptr );
	}

	static QLineF checkLine( lua_State *L, int i = 1 )
	{
		UserData *UD = checkuserdata( L, i );

		return( UD->mLine );
	}

	static int luaPinGet( const QUuid &pPinLocalId, lua_State *L );
	static int luaPinSet( const QUuid &pPinLocalId, lua_State *L, int pIndex );

private:
	static UserData *checkuserdata( lua_State *L, int i = 1 )
	{
		UserData *UD = (UserData *)luaL_checkudata( L, i, mTypeName );

		luaL_argcheck( L, UD != NULL, i, "Line expected" );

		return( UD );
	}

	static int luaIndex( lua_State *L );
	static int luaNewIndex( lua_State *L );


	static int luaAngle( lua_State *L );
	static int luaIsNull( lua_State *L );
	static int luaLength( lua_State *L );
	static int luaP1( lua_State *L );
	static int luaP2( lua_State *L );
	static int luaSetAngle( lua_State *L );
	static int luaSetLine( lua_State *L );
	static int luaSetP1( lua_State *L );
	static int luaSetP2( lua_State *L );
	static int luaTranslate( lua_State *L );
	static int luaTranslated( lua_State *L );

private:
	static const luaL_Reg					mLuaFunctions[];
	static const luaL_Reg					mLuaMetaMethods[];
	static const luaL_Reg					mLuaMethods[];
#endif
};

#endif // LUALINE_H
