#include "luaarray.h"

#include <fugio/core/list_interface.h>
#include <fugio/core/array_interface.h>

#include "luaplugin.h"

const luaL_Reg LuaArray::mLuaFunctions[] =
{
	{ 0, 0 }
};

const luaL_Reg LuaArray::mLuaInstance[] =
{
	{ "__index",	LuaArray::luaGet },
	{ "__newindex", LuaArray::luaSet },
	{ "__len",		LuaArray::luaLen },
	{ "array",		LuaArray::luaToArray },
	{ "resize",		LuaArray::luaResize },
	{ "setType",	LuaArray::luaSetType },
	{ 0, 0 }
};

int LuaArray::lua_openarray( lua_State *L )
{
	luaL_newmetatable( L, "fugio.array" );

	luaL_setfuncs( L, mLuaInstance, 0 );

	luaL_newlib( L, mLuaFunctions );

	return( 1 );
}

int LuaArray::luaGet( lua_State *L )
{
	LuaArrayUserData			*LstDat = checkarray( L );

	if( lua_type( L, 2 ) == LUA_TSTRING )
	{
		const char	*s = luaL_checkstring( L, 2 );

		for( const luaL_Reg *F = mLuaInstance ; F->func ; F++ )
		{
			if( !strcmp( s, F->name ) )
			{
				lua_pushcfunction( L, F->func );

				return( 1 );
			}
		}

		return( luaL_error( L, "Unknown array method: %s", s ) );
	}

	int							 LstIdx = luaL_checkinteger( L, 2 );

	fugio::ListInterface		*LstInt = qobject_cast<fugio::ListInterface *>( LstDat->mObject );

	if( !LstInt )
	{
		return( luaL_error( L, "No list" ) );
	}

	if( LstIdx < 1 || LstIdx > LstInt->listSize() )
	{
		return( luaL_error( L, "List index out of bounds (asked for %d, size is %d)", LstIdx, LstInt->listSize() ) );
	}

	return( LuaPlugin::pushVariant( L, LstInt->listIndex( LstIdx - 1 ) ) );
}

int LuaArray::luaSet( lua_State *L )
{
	LuaArrayUserData			*LstDat = checkarray( L );
	int							 LstIdx = luaL_checkinteger( L, 2 ) - 1;	// Lua indexes start at 1

	if( LstDat->mReadOnly )
	{
		return( luaL_error( L, "Can't set data in input list" ) );
	}

	luaL_checkany( L, 3 );

	fugio::ArrayInterface		*ArrInt = qobject_cast<fugio::ArrayInterface *>( LstDat->mObject );

	if( ArrInt )
	{
		if( LstIdx < 0 || LstIdx >= ArrInt->count() )
		{
			return( luaL_error( L, "List index out of bounds (asked for %d, size is %d)", LstIdx + 1, ArrInt->count() ) );
		}

		if( ArrInt->type() == QMetaType::Float )
		{
			float		*A = (float *)ArrInt->array();

			if( A && ArrInt->stride() == sizeof( float ) )
			{
				if( ArrInt->size() == 1 )
				{
					A[ LstIdx ] = lua_tonumber( L, 3 );
				}
				else if( lua_type( L, 3 ) == LUA_TTABLE )
				{
					for( int i = 1 ; i < ArrInt->size() ; i++ )
					{
						lua_rawgeti( L, 3, i );

						if( lua_isnil( L, -1 ) )
						{
							lua_pop( L, 1 );

							break;
						}

						A[ ( LstIdx * ArrInt->size() ) + i - 1 ] = lua_tonumber( L, -1 );

						lua_pop( L, 1 );
					}
				}
			}

			return( 0 );
		}

		if( ArrInt->type() == QMetaType::Int )
		{
			int		*A = (int *)ArrInt->array();

			if( A && ArrInt->stride() == sizeof( int ) )
			{
				if( ArrInt->size() == 1 )
				{
					A[ LstIdx ] = lua_tointeger( L, 3 );
				}
				else if( lua_type( L, 3 ) == LUA_TTABLE )
				{
					for( int i = 1 ; i < ArrInt->size() ; i++ )
					{
						lua_rawgeti( L, 3, i );

						if( lua_isnil( L, -1 ) )
						{
							lua_pop( L, 1 );

							break;
						}

						A[ ( LstIdx * ArrInt->size() ) + i - 1 ] = lua_tointeger( L, -1 );

						lua_pop( L, 1 );
					}
				}
			}

			return( 0 );
		}

		if( ArrInt->type() == QMetaType::QVector3D )
		{
			float		*A = (float *)ArrInt->array();

			if( A )
			{
				if( lua_type( L, 3 ) == LUA_TTABLE )
				{
					for( int i = 1 ; i <= 3 ; i++ )
					{
						lua_rawgeti( L, 3, i );

						if( lua_isnil( L, -1 ) )
						{
							lua_pop( L, 1 );

							break;
						}

						A[ ( LstIdx * 3 ) + i - 1 ] = lua_tonumber( L, -1 );

						lua_pop( L, 1 );
					}
				}
			}

			return( 0 );
		}
	}

	fugio::ListInterface		*LstInt = qobject_cast<fugio::ListInterface *>( LstDat->mObject );

	if( !LstInt )
	{
		return( luaL_error( L, "No list" ) );
	}

	if( LstIdx < 0 || LstIdx >= LstInt->listSize() )
	{
		return( luaL_error( L, "List index out of bounds (asked for %d, size is %d)", LstIdx + 1, LstInt->listSize() ) );
	}

	QVariant		V = LuaPlugin::popVariant( L, 3 );

	LstInt->listSetIndex( LstIdx, V );

	return( 0 );
}

int LuaArray::luaLen( lua_State *L )
{
	LuaArrayUserData			*LstDat = checkarray( L );

	fugio::ListInterface		*LstInt = qobject_cast<fugio::ListInterface *>( LstDat->mObject );

	if( LstInt )
	{
		lua_pushinteger( L, LstInt->listSize() );

		return( 1 );
	}

	fugio::ArrayInterface		*ArrInt = qobject_cast<fugio::ArrayInterface *>( LstDat->mObject );

	if( ArrInt )
	{
		lua_pushinteger( L, ArrInt->count() );

		return( 1 );
	}

	return( 0 );
}

int LuaArray::luaToArray( lua_State *L )
{
	LuaArrayUserData			*ArrDat = checkarray( L );

	fugio::ArrayInterface		*ArrInt = qobject_cast<fugio::ArrayInterface *>( ArrDat->mObject );

	if( ArrInt )
	{
		if( ArrInt->type() == QMetaType::Float )
		{
			const float	*A = (const float *)ArrInt->array();

			lua_newtable( L );

			if( ArrInt->size() == 1 )
			{
				for( int i = 0 ; i < ArrInt->count() ; i++ )
				{
					lua_pushnumber( L, A[ i ] );

					lua_rawseti( L, -2, i + 1 );
				}
			}
			else
			{
				for( int i = 0 ; i < ArrInt->count() ; i++ )
				{
					lua_newtable( L );

					for( int j = 0 ; j < ArrInt->size() ; j++ )
					{
						lua_pushnumber( L, A[ ( i * ArrInt->size() ) + j ] );

						lua_rawseti( L, -2, j + 1 );
					}

					lua_rawseti( L, -2, i + 1 );
				}
			}

			return( 1 );
		}

		if( ArrInt->type() == QMetaType::Int )
		{
			const int	*A = (const int *)ArrInt->array();

			lua_newtable( L );

			if( ArrInt->size() == 1 )
			{
				for( int i = 0 ; i < ArrInt->count() ; i++ )
				{
					lua_pushinteger( L, A[ i ] );

					lua_rawseti( L, -2, i + 1 );
				}
			}
			else
			{
				for( int i = 0 ; i < ArrInt->count() ; i++ )
				{
					lua_newtable( L );

					for( int j = 0 ; j < ArrInt->size() ; j++ )
					{
						lua_pushinteger( L, A[ ( i * ArrInt->size() ) + j ] );

						lua_rawseti( L, -2, j + 1 );
					}

					lua_rawseti( L, -2, i + 1 );
				}
			}

			return( 1 );
		}
	}

	fugio::ListInterface		*LstInt = qobject_cast<fugio::ListInterface *>( ArrDat->mObject );

	if( LstInt )
	{
		lua_newtable( L );

		for( int i = 0 ; i < LstInt->listSize() ; i++ )
		{
			LuaPlugin::pushVariant( L, LstInt->listIndex( i ) );

			lua_rawseti( L, -2, i + 1 );
		}

		return( 1 );
	}

	return( 0 );
}

int LuaArray::luaResize( lua_State *L )
{
	LuaArrayUserData			*LstDat = checkarray( L );
	int							 LstSiz = luaL_checkinteger( L, 2 );

	fugio::ArrayInterface		*LstInt = qobject_cast<fugio::ArrayInterface *>( LstDat->mObject );

	if( !LstInt )
	{
		return( luaL_error( L, "No list" ) );
	}

	if( LstDat->mReadOnly )
	{
		return( luaL_error( L, "Can't resize input list" ) );
	}

	if( LstSiz < 0 )
	{
		return( luaL_error( L, "Invalid list size: %d", LstSiz ) );
	}

	if( LstInt->type() == QMetaType::UnknownType )
	{
		return( luaL_error( L, "Array has unknown type" ) );
	}

	if( LstInt->count() != LstSiz )
	{
		LstInt->setCount( LstSiz );
	}

	return( 0 );
}

int LuaArray::luaSetType( lua_State *L )
{
	LuaArrayUserData		*LstDat = checkarray( L );
	const char				*LstTyp = lua_tostring( L, 2 );

	fugio::ArrayInterface		*LstInt = qobject_cast<fugio::ArrayInterface *>( LstDat->mObject );

	if( !LstInt )
	{
		return( luaL_error( L, "No array" ) );
	}

	if( LstDat->mReadOnly )
	{
		return( luaL_error( L, "Can't set type on input array" ) );
	}

	if( strcmp( LstTyp, "float" ) == 0 )
	{
		LstInt->setType( QMetaType::Float );
		LstInt->setStride( sizeof( float ) );
		LstInt->setSize( 1 );
	}
	else if( strcmp( LstTyp, "int" ) == 0 )
	{
		LstInt->setType( QMetaType::Int );
		LstInt->setStride( sizeof( int ) );
		LstInt->setSize( 1 );
	}
	else if( strcmp( LstTyp, "vec3" ) == 0 )
	{
		LstInt->setType( QMetaType::QVector3D );
		LstInt->setStride( sizeof( float ) * 3 );
		LstInt->setSize( 3 );
	}
	else if( strcmp( LstTyp, "vec4" ) == 0 )
	{
		LstInt->setType( QMetaType::QVector4D );
		LstInt->setStride( sizeof( float ) * 4 );
		LstInt->setSize( 4 );
	}
	else if( strcmp( LstTyp, "mat4" ) == 0 )
	{
		LstInt->setType( QMetaType::QMatrix4x4 );
		LstInt->setStride( sizeof( float ) * 4 * 4 );
		LstInt->setSize( 4 * 4 );
	}
	else
	{
		return( luaL_error( L, "Unknown list type: %s", LstTyp ) );
	}

	return( 0 );
}

