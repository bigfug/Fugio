#ifndef LUABRUSH_H
#define LUABRUSH_H

#if defined( LUA_SUPPORTED )
#include <lua.hpp>
#endif

#include <fugio/lua/lua_interface.h>

#include <QMap>
#include <QBrush>

class LuaBrush
{
private:
	typedef struct BrushUserData
	{
		static const char *TypeName;

		QBrush		*mBrush;
	} BrushUserData;

public:
	LuaBrush( void ) {}

	virtual ~LuaBrush( void ) {}

#if defined( LUA_SUPPORTED )
	static void registerExtension( fugio::LuaInterface *LUA );

	static int luaOpen( lua_State *L );

	static int luaNew( lua_State *L );

	static int pushbrush( lua_State *L, const QBrush &pBrush )
	{
		QBrush			*Brush = new QBrush( pBrush );

		if( !Brush )
		{
			return( 0 );
		}

		BrushUserData	*UD = (BrushUserData *)lua_newuserdata( L, sizeof( BrushUserData ) );

		if( !UD )
		{
			delete Brush;

			return( 0 );
		}

		luaL_getmetatable( L, BrushUserData::TypeName );
		lua_setmetatable( L, -2 );

		UD->mBrush = Brush;

		return( 1 );
	}

	static bool isBrush( lua_State *L, int i = 1 )
	{
		return( luaL_testudata( L, i, BrushUserData::TypeName ) != nullptr );
	}

	static QBrush *checkbrush( lua_State *L, int i = 1 )
	{
		void *ud = luaL_checkudata( L, i, BrushUserData::TypeName );

		luaL_argcheck( L, ud != NULL, i, "Brush expected" );

		return( static_cast<BrushUserData *>( ud )->mBrush );
	}

private:
	static int luaDelete( lua_State *L );

	static int luaColor( lua_State *L );
	static int luaSetColor( lua_State *L );
	static int luaSetStyle( lua_State *L );
	static int luaStyle( lua_State *L );

private:
	static const luaL_Reg					mLuaInstance[];
	static const luaL_Reg					mLuaMethods[];

	static const QMap<QString,Qt::BrushStyle>	mBrushStyleMap;
#endif
};


#endif // LUABRUSH_H
