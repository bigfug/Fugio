#ifndef LUAPOINTF_H
#define LUAPOINTF_H

#if defined( LUA_SUPPORTED )
#include <lua.hpp>
#endif

#include <QPointF>

class LuaPointF
{
private:
	typedef struct PointFUserData
	{
		static const char *TypeName;

		qreal	x, y;

		operator QPointF( void ) const
		{
			return( QPointF( x, y ) );
		}

		PointFUserData *operator = ( const QPointF &P )
		{
			x = P.x();
			y = P.y();

			return( this );
		}

	} QPointFUserData;

public:
	LuaPointF( void ) {}

	virtual ~LuaPointF( void ) {}

#if defined( LUA_SUPPORTED )
	static int luaOpen( lua_State *L );

	static int luaNew( lua_State *L );

	static int pushpointf( lua_State *L, const QPointF &pPoint )
	{
		PointFUserData	*UD = (PointFUserData *)lua_newuserdata( L, sizeof( PointFUserData ) );

		if( !UD )
		{
			return( 0 );
		}

		luaL_getmetatable( L, PointFUserData::TypeName );
		lua_setmetatable( L, -2 );

		UD->x = pPoint.x();
		UD->y = pPoint.y();

		return( 1 );
	}

	static bool isPointF( lua_State *L, int i = 1 )
	{
		return( luaL_testudata( L, i, PointFUserData::TypeName ) != nullptr );
	}

	static QPointF checkpointf( lua_State *L, int i = 1 )
	{
		PointFUserData *UD = checkpointfuserdata( L, i );

		return( *UD );
	}

private:
	static PointFUserData *checkpointfuserdata( lua_State *L, int i = 1 )
	{
		PointFUserData *UD = (PointFUserData *)luaL_checkudata( L, i, PointFUserData::TypeName );

		luaL_argcheck( L, UD != NULL, i, "Point expected" );

		return( UD );
	}

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
	static int luaX( lua_State *L );
	static int luaY( lua_State *L );

private:
	static const luaL_Reg					mLuaInstance[];
	static const luaL_Reg					mLuaMethods[];
#endif
};

#endif // LUAPOINTF_H
