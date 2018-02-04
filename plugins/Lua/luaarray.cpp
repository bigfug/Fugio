#include "luaarray.h"

#include <QRect>
#include <QRectF>

#include <fugio/core/array_interface.h>

#include "luaplugin.h"

#if defined( LUA_SUPPORTED )

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
	{ "resize",		LuaArray::luaSetCount },
	{ "reserve",	LuaArray::luaReserve },
	{ "set",		LuaArray::luaSetTable },
	{ "setCount",	LuaArray::luaSetCount },
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

	fugio::VariantInterface		*VarInt = qobject_cast<fugio::VariantInterface *>( LstDat->mObject );

	if( VarInt )
	{
		if( LstIdx < 1 || LstIdx > VarInt->variantCount() )
		{
			return( luaL_error( L, "List index out of bounds (asked for %d, size is %d)", LstIdx, VarInt->variantCount() ) );
		}

		return( LuaPlugin::pushVariant( L, VarInt->variant( LstIdx - 1 ) ) );
	}

	return( 0 );
}

void LuaArray::pushFloatArray( lua_State *L, float *A, int LstIdx, int LstCnt )
{
	if( A )
	{
		if( lua_type( L, 3 ) == LUA_TTABLE )
		{
			for( int i = 1 ; i <= LstCnt ; i++ )
			{
				lua_rawgeti( L, 3, i );

				if( lua_isnil( L, -1 ) )
				{
					lua_pop( L, 1 );

					break;
				}

				A[ ( LstIdx * LstCnt ) + i - 1 ] = lua_tonumber( L, -1 );

				lua_pop( L, 1 );
			}
		}
	}
}

void LuaArray::pushIntArray( lua_State *L, float *A, int LstIdx, int LstCnt )
{
	if( A )
	{
		if( lua_type( L, 3 ) == LUA_TTABLE )
		{
			for( int i = 1 ; i <= LstCnt ; i++ )
			{
				lua_rawgeti( L, 3, i );

				if( lua_isnil( L, -1 ) )
				{
					lua_pop( L, 1 );

					break;
				}

				A[ ( LstIdx * LstCnt ) + i - 1 ] = lua_tointeger( L, -1 );

				lua_pop( L, 1 );
			}
		}
	}
}

int LuaArray::luaSet( lua_State *L )
{
	LuaArrayUserData			*LstDat = checkarray( L );

	if( LstDat->mReadOnly )
	{
		return( luaL_error( L, "Can't set data in input list" ) );
	}

	int		LstIdx = luaL_checkinteger( L, 2 ) - 1;		// Lua indexes start at 1

	luaL_checkany( L, 3 );

	fugio::VariantInterface		*VarInf = qobject_cast<fugio::VariantInterface *>( LstDat->mObject );

	if( !VarInf )
	{
		return( luaL_error( L, "No variant found" ) );
	}

	if( LstIdx < 0 || LstIdx >= VarInf->variantCount() )
	{
		return( luaL_error( L, "List index out of bounds (asked for %d, size is %d)", LstIdx + 1, VarInf->variantCount() ) );
	}

	QVariant		V = LuaPlugin::popVariant( L, 3 );

	VarInf->setVariant( LstIdx, V );

	return( 0 );
}

void LuaArray::setArrayIndex( lua_State *L, fugio::VariantInterface *VarInf, int LstIdx, int ValIdx )
{
	if( VarInf->variantType() == QMetaType::QLineF || VarInf->variantType() == QMetaType::QPointF )
	{
		QVariant	V = LuaPlugin::popVariant( L, ValIdx );

		if( QMetaType::Type( V.type() ) == VarInf->variantType() )
		{
			VarInf->setFromBaseVariant( LstIdx, V );
		}

		return;
	}

	if( VarInf->variantType() == QMetaType::Float )
	{
		float		*A = (float *)VarInf->variantArray();

		if( A && VarInf->variantStride() == sizeof( float ) )
		{
			if( VarInf->variantElementCount() == 1 )
			{
				A[ LstIdx ] = lua_tonumber( L, ValIdx );
			}
			else if( lua_type( L, ValIdx ) == LUA_TTABLE )
			{
				for( int i = 1 ; i < VarInf->variantElementCount() ; i++ )
				{
					lua_rawgeti( L, ValIdx, i );

					if( lua_isnil( L, -1 ) )
					{
						lua_pop( L, 1 );

						break;
					}

					A[ ( LstIdx * VarInf->variantElementCount() ) + i - 1 ] = lua_tonumber( L, -1 );

					lua_pop( L, 1 );
				}
			}
		}

		return;
	}

	if( VarInf->variantType() == QMetaType::Int )
	{
		float		*A = (float *)VarInf->variantArray();

		if( A && VarInf->variantStride() == sizeof( float ) )
		{
			if( VarInf->variantElementCount() == 1 )
			{
				A[ LstIdx ] = lua_tonumber( L, ValIdx );
			}
			else if( lua_type( L, ValIdx ) == LUA_TTABLE )
			{
				for( int i = 1 ; i < VarInf->variantElementCount() ; i++ )
				{
					lua_rawgeti( L, ValIdx, i );

					if( lua_isnil( L, -1 ) )
					{
						lua_pop( L, 1 );

						break;
					}

					A[ ( LstIdx * VarInf->variantElementCount() ) + i - 1 ] = lua_tointeger( L, -1 );

					lua_pop( L, 1 );
				}
			}
		}

		return;
	}

	if( VarInf->variantType() == QMetaType::QVector2D )
	{
		pushFloatArray( L, (float *)VarInf->variantArray(), LstIdx, 2 );
	}
	else if( VarInf->variantType() == QMetaType::QVector3D )
	{
		pushFloatArray( L, (float *)VarInf->variantArray(), LstIdx, 3 );
	}
	else if( VarInf->variantType() == QMetaType::QVector4D || VarInf->variantType() == QMetaType::QColor || VarInf->variantType() == QMetaType::QLineF )
	{
		pushFloatArray( L, (float *)VarInf->variantArray(), LstIdx, 4 );
	}
	else if( VarInf->variantType() == QMetaType::QMatrix4x4 )
	{
		pushFloatArray( L, (float *)VarInf->variantArray(), LstIdx, 16 );
	}
}

int LuaArray::luaSetTable( lua_State *L )
{
	LuaArrayUserData			*LstDat = checkarray( L );

	if( LstDat->mReadOnly )
	{
		return( luaL_error( L, "Can't set data in input list" ) );
	}

	luaL_checktype( L, 2, LUA_TTABLE );

	if( fugio::VariantInterface *ArrInt = qobject_cast<fugio::VariantInterface *>( LstDat->mObject ) )
	{
		const int	SrcLen = lua_rawlen( L, 2 );

		ArrInt->setVariantCount( SrcLen );

		for( int i = 1 ; i <= SrcLen ; i++ )
		{
			lua_rawgeti( L, 2, i );

			setArrayIndex( L, ArrInt, i - 1, -1 );

			lua_pop( L, 1 );
		}

		return( 0 );
	}

	return( luaL_error( L, "No list" ) );
}

int LuaArray::luaLen( lua_State *L )
{
	LuaArrayUserData			*LstDat = checkarray( L );

	fugio::VariantInterface		*VarInt = qobject_cast<fugio::VariantInterface *>( LstDat->mObject );

	if( VarInt )
	{
		lua_pushinteger( L, VarInt->variantCount() );

		return( 1 );
	}

	return( 0 );
}

int LuaArray::luaToArray( lua_State *L )
{
	LuaArrayUserData			*ArrDat = checkarray( L );

//	fugio::ArrayListInterface	*ArLInt = qobject_cast<fugio::ArrayListInterface *>( ArrDat->mObject );

//	if( ArLInt )
//	{
//		return( luaL_error( L, "Can't toArray from array list" ) );
//	}

//	fugio::ArrayInterface		*ArrInt = qobject_cast<fugio::ArrayInterface *>( ArrDat->mObject );

//	if( ArrInt )
//	{
//		if( ArrInt->type() == QMetaType::Float )
//		{
//			const float	*A = (const float *)ArrInt->array();

//			lua_newtable( L );

//			if( ArrInt->size() == 1 )
//			{
//				for( int i = 0 ; i < ArrInt->count() ; i++ )
//				{
//					lua_pushnumber( L, A[ i ] );

//					lua_rawseti( L, -2, i + 1 );
//				}
//			}
//			else
//			{
//				for( int i = 0 ; i < ArrInt->count() ; i++ )
//				{
//					lua_newtable( L );

//					for( int j = 0 ; j < ArrInt->size() ; j++ )
//					{
//						lua_pushnumber( L, A[ ( i * ArrInt->size() ) + j ] );

//						lua_rawseti( L, -2, j + 1 );
//					}

//					lua_rawseti( L, -2, i + 1 );
//				}
//			}

//			return( 1 );
//		}

//		if( ArrInt->type() == QMetaType::Int )
//		{
//			const int	*A = (const int *)ArrInt->array();

//			lua_newtable( L );

//			if( ArrInt->size() == 1 )
//			{
//				for( int i = 0 ; i < ArrInt->count() ; i++ )
//				{
//					lua_pushinteger( L, A[ i ] );

//					lua_rawseti( L, -2, i + 1 );
//				}
//			}
//			else
//			{
//				for( int i = 0 ; i < ArrInt->count() ; i++ )
//				{
//					lua_newtable( L );

//					for( int j = 0 ; j < ArrInt->size() ; j++ )
//					{
//						lua_pushinteger( L, A[ ( i * ArrInt->size() ) + j ] );

//						lua_rawseti( L, -2, j + 1 );
//					}

//					lua_rawseti( L, -2, i + 1 );
//				}
//			}

//			return( 1 );
//		}

//		if( ArrInt->type() == QMetaType::QPoint )
//		{
//			const int	*A = (const int *)ArrInt->array();

//			lua_newtable( L );

//			if( ArrInt->size() == 1 )
//			{
//				for( int i = 0 ; i < ArrInt->count() ; i++ )
//				{
//					lua_newtable( L );

//					for( int j = 0 ; j < 2 ; j++ )
//					{
//						lua_pushinteger( L, A[ ( i * 2 ) + j ] );

//						lua_rawseti( L, -2, j + 1 );
//					}

//					lua_rawseti( L, -2, i + 1 );
//				}
//			}

//			return( 1 );
//		}

//		if( ArrInt->type() == QMetaType::QPointF )
//		{
//			const float	*A = (const float *)ArrInt->array();

//			lua_newtable( L );

//			if( ArrInt->size() == 1 )
//			{
//				for( int i = 0 ; i < ArrInt->count() ; i++ )
//				{
//					lua_newtable( L );

//					for( int j = 0 ; j < 2 ; j++ )
//					{
//						lua_pushnumber( L, A[ ( i * 2 ) + j ] );

//						lua_rawseti( L, -2, j + 1 );
//					}

//					lua_rawseti( L, -2, i + 1 );
//				}
//			}

//			return( 1 );
//		}

//		if( ArrInt->type() == QMetaType::QLine )
//		{
//			const int	*A = (const int *)ArrInt->array();

//			lua_newtable( L );

//			if( ArrInt->size() == 1 )
//			{
//				for( int i = 0 ; i < ArrInt->count() ; i++ )
//				{
//					lua_newtable( L );

//					for( int j = 0 ; j < 4 ; j++ )
//					{
//						lua_pushinteger( L, A[ ( i * 4 ) + j ] );

//						lua_rawseti( L, -2, j + 1 );
//					}

//					lua_rawseti( L, -2, i + 1 );
//				}
//			}

//			return( 1 );
//		}

//		if( ArrInt->type() == QMetaType::QLineF )
//		{
//			const float	*A = (const float *)ArrInt->array();

//			lua_newtable( L );

//			if( ArrInt->size() == 1 )
//			{
//				for( int i = 0 ; i < ArrInt->count() ; i++ )
//				{
//					lua_newtable( L );

//					for( int j = 0 ; j < 4 ; j++ )
//					{
//						lua_pushnumber( L, A[ ( i * 4 ) + j ] );

//						lua_rawseti( L, -2, j + 1 );
//					}

//					lua_rawseti( L, -2, i + 1 );
//				}
//			}

//			return( 1 );
//		}
//	}

//	fugio::ListInterface		*LstInt = qobject_cast<fugio::ListInterface *>( ArrDat->mObject );

//	if( LstInt )
//	{
//		lua_newtable( L );

//		for( int i = 0 ; i < LstInt->listSize() ; i++ )
//		{
//			LuaPlugin::pushVariant( L, LstInt->listIndex( i ) );

//			lua_rawseti( L, -2, i + 1 );
//		}

//		return( 1 );
//	}

	return( 0 );
}

int LuaArray::luaResize( lua_State *L )
{
	LuaArrayUserData			*LstDat = checkarray( L );
	int							 LstSiz = luaL_checkinteger( L, 2 );

//	fugio::ArrayListInterface	*ArLInt = qobject_cast<fugio::ArrayListInterface *>( LstDat->mObject );

//	if( ArLInt )
//	{
//		return( luaL_error( L, "Can't resize array list (yet)" ) );
//	}

//	if( LstDat->mReadOnly )
//	{
//		return( luaL_error( L, "Can't resize input list" ) );
//	}

//	if( LstSiz < 0 )
//	{
//		return( luaL_error( L, "Invalid list size: %d", LstSiz ) );
//	}

	fugio::ArrayInterface		*ArrInt = qobject_cast<fugio::ArrayInterface *>( LstDat->mObject );

	if( ArrInt )
	{
		if( ArrInt->type() == QMetaType::UnknownType )
		{
			return( luaL_error( L, "Array has unknown type" ) );
		}

		if( ArrInt->count() != LstSiz )
		{
			ArrInt->setCount( LstSiz );
		}

		return( 0 );
	}

//	fugio::ListInterface		*LstInt = qobject_cast<fugio::ListInterface *>( LstDat->mObject );

//	if( LstInt )
//	{
//		LstInt->listSetSize( LstSiz );

//		return( 0 );
//	}

	return( 0 );
}

int LuaArray::luaReserve( lua_State *L )
{
	LuaArrayUserData			*LstDat = checkarray( L );
	int							 LstSiz = luaL_checkinteger( L, 2 );

	if( LstSiz < 0 )
	{
		return( luaL_error( L, "Invalid list size: %d", LstSiz ) );
	}

	fugio::ArrayInterface		*LstInt = qobject_cast<fugio::ArrayInterface *>( LstDat->mObject );

	if( LstInt )
	{
		if( LstDat->mReadOnly )
		{
			return( luaL_error( L, "Can't reserve input list" ) );
		}

		if( LstInt->type() == QMetaType::UnknownType )
		{
			return( luaL_error( L, "Array has unknown type" ) );
		}

		LstInt->reserve( LstSiz );

		return( 0 );
	}

	fugio::VariantInterface		*VarInt = qobject_cast<fugio::VariantInterface *>( LstDat->mObject );

	if( VarInt )
	{
		VarInt->variantReserve( LstSiz );

		return( 0 );
	}

	return( luaL_error( L, "No list" ) );
}

int LuaArray::luaSetCount( lua_State *L )
{
	LuaArrayUserData			*LstDat = checkarray( L );
	const int					 LstCnt = lua_tointeger( L, 2 );

	if( LstDat->mReadOnly )
	{
		return( luaL_error( L, "Can't set type on input array" ) );
	}

	fugio::ArrayInterface		*LstInt = qobject_cast<fugio::ArrayInterface *>( LstDat->mObject );

	if( LstInt )
	{
		LstInt->setCount( LstCnt );
	}

	fugio::VariantInterface		*VarInt = qobject_cast<fugio::VariantInterface *>( LstDat->mObject );

	if( VarInt )
	{
		VarInt->setVariantCount( LstCnt );
	}

	return( 0 );
}

int LuaArray::luaSetType( lua_State *L )
{
	LuaArrayUserData		*LstDat = checkarray( L );
	const char				*LstTyp = lua_tostring( L, 2 );

	if( LstDat->mReadOnly )
	{
		return( luaL_error( L, "Can't set type on input array" ) );
	}

	fugio::VariantInterface		*ArrInt = qobject_cast<fugio::VariantInterface *>( LstDat->mObject );

	if( ArrInt )
	{
		if( strcmp( LstTyp, "float" ) == 0 )
		{
			ArrInt->setVariantType( QMetaType::Float );
			ArrInt->variantSetStride( sizeof( float ) );
			ArrInt->setVariantCount( 1 );
		}
		else if( strcmp( LstTyp, "int" ) == 0 )
		{
			ArrInt->setVariantType( QMetaType::Int );
			ArrInt->variantSetStride( sizeof( int ) );
			ArrInt->setVariantCount( 1 );
		}
		else if( strcmp( LstTyp, "point" ) == 0 )
		{
			ArrInt->setVariantType( QMetaType::QPointF );
			ArrInt->variantSetStride( sizeof( float ) * 2 );
			ArrInt->setVariantCount( 1 );
		}
		else if( strcmp( LstTyp, "vec2" ) == 0 )
		{
			ArrInt->setVariantType( QMetaType::QVector2D );
			ArrInt->variantSetStride( sizeof( float ) * 2 );
			ArrInt->setVariantCount( 1 );
		}
		else if( strcmp( LstTyp, "vec3" ) == 0 )
		{
			ArrInt->setVariantType( QMetaType::QVector3D );
			ArrInt->variantSetStride( sizeof( float ) * 3 );
			ArrInt->setVariantCount( 1 );
		}
		else if( strcmp( LstTyp, "vec4" ) == 0 )
		{
			ArrInt->setVariantType( QMetaType::QVector4D );
			ArrInt->variantSetStride( sizeof( float ) * 4 );
			ArrInt->setVariantCount( 1 );
		}
		else if( strcmp( LstTyp, "mat4" ) == 0 )
		{
			ArrInt->setVariantType( QMetaType::QMatrix4x4 );
			ArrInt->variantSetStride( sizeof( float ) * 4 * 4 );
			ArrInt->setVariantCount( 1 );
		}
		else if( strcmp( LstTyp, "rect" ) == 0 )
		{
			ArrInt->setVariantType( QMetaType::QRect );
			ArrInt->variantSetStride( sizeof( QRect ) );
			ArrInt->setVariantCount( 1 );
		}
		else if( strcmp( LstTyp, "rectf" ) == 0 )
		{
			ArrInt->setVariantType( QMetaType::QRectF );
			ArrInt->variantSetStride( sizeof( QRectF ) );
			ArrInt->setVariantCount( 1 );
		}
		else if( strcmp( LstTyp, "line" ) == 0 )
		{
			ArrInt->setVariantType( QMetaType::QLine );
			ArrInt->variantSetStride( sizeof( QLine ) );
			ArrInt->setVariantCount( 1 );
		}
		else if( strcmp( LstTyp, "linef" ) == 0 )
		{
			ArrInt->setVariantType( QMetaType::QLineF );
			ArrInt->variantSetStride( sizeof( QLineF ) );
			ArrInt->setVariantCount( 1 );
		}
		else
		{
			return( luaL_error( L, "Unknown list type: %s", LstTyp ) );
		}

		return( 0 );
	}

	return( 0 );
}

#endif
