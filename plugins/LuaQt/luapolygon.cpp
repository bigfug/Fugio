#include "luapolygon.h"

#include "luaqtplugin.h"

#include <fugio/core/uuid.h>
#include <fugio/context_interface.h>
#include <fugio/node_interface.h>
#include <fugio/pin_interface.h>
#include <fugio/core/variant_interface.h>

#include "luaqtplugin.h"
#include "luarectf.h"
#include "luapointf.h"

const char *LuaPolygon::mTypeName = "qt.polygon";

#if defined( LUA_SUPPORTED )

const luaL_Reg LuaPolygon::mLuaFunctions[] =
{
	{ Q_NULLPTR, Q_NULLPTR }
};

const luaL_Reg LuaPolygon::mLuaMetaMethods[] =
{
	{ "__gc",				LuaPolygon::luaDelete },
	{ "__len",				LuaPolygon::luaLen },
	{ "__index",			LuaPolygon::luaIndex },
	{ "__newindex",			LuaPolygon::luaNewIndex },
	{ "__tostring",			LuaPolygon::luaToString },
	{ Q_NULLPTR, Q_NULLPTR }
};

const luaL_Reg LuaPolygon::mLuaMethods[] =
{
	{ "append",				LuaPolygon::luaAppend },
	{ "boundingRect",		LuaPolygon::luaBoundingRect },
	{ "containsPoint",		LuaPolygon::luaContainsPoint },
	{ Q_NULLPTR, Q_NULLPTR }
};

void LuaPolygon::registerExtension( fugio::LuaInterface *LUA )
{
	LuaQtPlugin::addLuaFunction( "polygon", LuaPolygon::luaNew );

	LUA->luaRegisterExtension( LuaPolygon::luaOpen );

	LUA->luaAddPinGet( PID_POLYGON, LuaPolygon::luaPinGet );

	LUA->luaAddPinSet( PID_POLYGON, LuaPolygon::luaPinSet );

	LUA->luaAddPushVariantFunction( QMetaType::QPolygonF, LuaPolygon::pushVariant );

	LUA->luaAddPopVariantFunction( mTypeName, LuaPolygon::popVariant );
}

int LuaPolygon::luaOpen( lua_State *L )
{
	luaL_newmetatable( L, mTypeName );

	lua_pushvalue( L, -1 );
	lua_setfield( L, -2, "__index" );

	luaL_setfuncs( L, mLuaMetaMethods, 0 );

	luaL_newlib( L, mLuaFunctions );

	return( 1 );
}

int LuaPolygon::luaNew( lua_State *L )
{
	const int	LuaTop = lua_gettop( L );

	if( LuaTop == 0 )
	{
		pushpolygon( L, QPolygonF() );
	}
//	else if( LuaTop == 1 )
//	{
//		if( lua_type( L, 1 ) == LUA_TSTRING )
//		{
//			const char		*s = lua_tostring( L, 1 );
//			QString			 S( s ? s : "" );

//			if( mBrushStyleMap.contains( S ) )
//			{
//				pushbrush( L, QBrush( mBrushStyleMap.value( S ) ) );
//			}
//			else
//			{
//				luaL_argcheck( L, false, 1, "Unknown type" );
//			}
//		}
//		else if( lua_type( L, 1 ) == LUA_TUSERDATA )
//		{
//			if( LuaColor::isColor( L ) )
//			{
//				QColor		C = LuaColor::checkcolor( L );

//				pushbrush( L, QBrush( C ) );
//			}
//			else if( LuaBrush::isBrush( L ) )
//			{
//				pushbrush( L, QBrush( *checkbrush( L ) ) );
//			}
//			else if( LuaGradient::isGradient( L ) )
//			{
//				pushbrush( L, QBrush( *LuaGradient::checkgradient( L ) ) );
//			}
//			else
//			{
//				luaL_argcheck( L, false, 1, "Unknown user data" );
//			}
//		}
//		else
//		{
//			luaL_argcheck( L, false, 1, "Unknown argument" );
//		}
//	}
//	else if( LuaTop == 2 )
//	{
//		if( LuaColor::isColor( L ) )
//		{
//			QColor		C = LuaColor::checkcolor( L );

//			if( lua_type( L, 2 ) == LUA_TSTRING )
//			{
//				const char		*s = lua_tostring( L, 2 );
//				QString			 S( s ? s : "" );

//				if( mBrushStyleMap.contains( S ) )
//				{
//					pushbrush( L, QBrush( C, mBrushStyleMap.value( S ) ) );
//				}
//				else
//				{
//					luaL_argcheck( L, false, 2, "Unknown type" );
//				}
//			}
//			else
//			{
//				luaL_argcheck( L, false, 2, "Unknown argument" );
//			}
//		}
//		else
//		{
//			luaL_argcheck( L, false, 1, "Unknown argument" );
//		}
//	}
	else
	{
		return( luaL_error( L, "Wrong number of arguments (%d) to polygon()", LuaTop ) );
	}

	return( 1 );
}

int LuaPolygon::luaPinGet( const QUuid &pPinLocalId, lua_State *L )
{
	fugio::LuaInterface						*Lua  = LuaQtPlugin::lua();
	NodeInterface							*Node = Lua->node( L );
	QSharedPointer<fugio::PinInterface>		 Pin = Node->findPinByLocalId( pPinLocalId );
	QSharedPointer<fugio::PinInterface>		 PinSrc;

	if( !Pin )
	{
		return( luaL_error( L, "No source pin" ) );
	}

	if( Pin->direction() == PIN_OUTPUT )
	{
		PinSrc = Pin;
	}
	else
	{
		PinSrc = Pin->connectedPin();
	}

	if( !PinSrc || !PinSrc->hasControl() )
	{
		return( luaL_error( L, "No point pin" ) );
	}

	fugio::VariantInterface			*SrcVar = qobject_cast<fugio::VariantInterface *>( PinSrc->control()->qobject() );

	if( !SrcVar )
	{
		return( luaL_error( L, "Can't access point" ) );
	}

	return( pushpolygon( L, SrcVar->variant().value<QPolygonF>() ) );
}

int LuaPolygon::luaPinSet( const QUuid &pPinLocalId, lua_State *L, int pIndex )
{
	fugio::LuaInterface						*Lua  = LuaQtPlugin::lua();
	NodeInterface							*Node = Lua->node( L );
	QSharedPointer<fugio::PinInterface>		 Pin = Node->findPinByLocalId( pPinLocalId );
	UserData								*UD = checkuserdata( L, pIndex );

	if( !Pin )
	{
		return( luaL_error( L, "No destination pin" ) );
	}

	if( Pin->direction() != PIN_OUTPUT )
	{
		return( luaL_error( L, "No destination pin" ) );
	}

	if( !Pin->hasControl() )
	{
		return( luaL_error( L, "No quaternion pin" ) );
	}

	fugio::VariantInterface			*DstVar = qobject_cast<fugio::VariantInterface *>( Pin->control()->qobject() );

	if( !DstVar )
	{
		return( luaL_error( L, "Can't access quaternion" ) );
	}

	if( UD->mPolygon != DstVar->variant().value<QPolygonF>() )
	{
		DstVar->setVariant( UD->mPolygon );

		Pin->node()->context()->pinUpdated( Pin );
	}

	return( 0 );
}

int LuaPolygon::luaToString( lua_State *L )
{
	UserData		*UD = checkuserdata( L );

	lua_pushfstring( L, "%i", UD->mPolygon.size() );

	return( 1 );
}

int LuaPolygon::luaDelete( lua_State *L )
{
	UserData	*UD = checkuserdata( L );

	static_cast<UserData *>( UD )->mPolygon.~QPolygonF();

	return( 0 );
}

int LuaPolygon::luaLen( lua_State *L )
{
	UserData	*UD = checkuserdata( L );

	lua_pushinteger( L, UD->mPolygon.size() );

	return( 1 );
}

int LuaPolygon::luaAppend( lua_State *L )
{
	QPointF			Point = LuaPointF::checkpointf( L, 2 );
	UserData		*UD = checkuserdata( L );

	UD->mPolygon.append( Point );

	return( 0 );
}

int LuaPolygon::luaBoundingRect( lua_State *L )
{
	UserData		*UD = checkuserdata( L );

	LuaRectF::pushrectf( L, UD->mPolygon.boundingRect() );

	return( 1 );
}

int LuaPolygon::luaContainsPoint(lua_State *L)
{
	UserData		*UD = checkuserdata( L );
	QPointF			 Point = LuaPointF::checkpointf( L, 2 );
	QString			 FillRuleName = QString::fromLatin1( luaL_checkstring( L, 3 ) );
	Qt::FillRule	 FillRule = Qt::OddEvenFill;

	if( FillRuleName == "OddEvenFill" )
	{
		FillRule = Qt::OddEvenFill;
	}
	else if( FillRuleName == "WindingFill" )
	{
		FillRule = Qt::WindingFill;
	}

	lua_pushboolean( L, UD->mPolygon.containsPoint( Point, FillRule ) );

	return( 1 );
}

int LuaPolygon::luaIndex( lua_State *L )
{
	UserData		*UD = checkuserdata( L );

	if( lua_type( L, 2 ) == LUA_TNUMBER )
	{
		lua_Integer		Index = lua_tointeger( L, 2 ) - 1;

		if( Index < 0 || UD->mPolygon.size() < Index )
		{
			return( luaL_error( L, "invalid index" ) );
		}

		return( LuaPointF::pushpointf( L, UD->mPolygon.at( Index ) ) );
	}
	else
	{
		const char	*S = luaL_checkstring( L, 2 );

		for( const luaL_Reg *R = mLuaMethods ; R->name ; R++ )
		{
			if( !strcmp( R->name, S ) )
			{
				lua_pushcfunction( L, R->func );

				return( 1 );
			}
		}
	}

	return( luaL_error( L, "unknown field" ) );
}

int LuaPolygon::luaNewIndex( lua_State *L )
{
	lua_Integer		Index = luaL_checkinteger( L, 2 ) - 1;
	QPointF			Point = LuaPointF::checkpointf( L, 3 );
	UserData		*UD = checkuserdata( L );

	if( Index >= 0 && UD->mPolygon.size() > Index )
	{
		UD->mPolygon[ Index ] = Point;

		return( 0 );
	}

	return( luaL_error( L, "invalid index" ) );
}

#endif // LUA_SUPPORTED
