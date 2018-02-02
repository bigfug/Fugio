#include "luaexpin.h"

#include <fugio/global_interface.h>
#include <fugio/core/variant_interface.h>
#include <fugio/lua/lua_interface.h>
#include <fugio/context_interface.h>
#include <fugio/global_signals.h>
#include <fugio/node_interface.h>
#include <fugio/node_signals.h>
#include <fugio/core/array_interface.h>

#include "luaplugin.h"
#include "luaarray.h"

using namespace fugio;

#if defined( LUA_SUPPORTED )
const luaL_Reg LuaExPin::mLuaPinFunctions[] =
{
	{ 0, 0 }
};

const luaL_Reg LuaExPin::mLuaInstance[] =
{
	{ "__index",	LuaExPin::luaGet },
	{ "__newindex", LuaExPin::luaSet },
	{ "__len",		LuaExPin::luaPinLen },
	{ "__tostring", LuaExPin::luaToString },
	{ 0, 0 }
};

const luaL_Reg LuaExPin::mLuaPinMethods[] =
{
	{ "name",		LuaExPin::luaPinGetName },
	{ "isUpdated",	LuaExPin::luaIsUpdated },
	{ "set",		LuaExPin::luaPinSetValue },
	{ "get",		LuaExPin::luaPinGetValue },
	{ "updated",	LuaExPin::luaUpdated },
	{ "update",		LuaExPin::luaUpdate },
	{ "setCount",	LuaExPin::luaPinSetCount },
	{ 0, 0 }
};

//{ "__index",	LuaArray::luaGet },

//virtual void setVariantCount( int pCount ) = 0;
//virtual int variantCount( void ) const = 0;
//virtual void setVariantElementCount( int pElementCount ) = 0;
//virtual int variantElementCount( void ) const = 0;
//virtual void variantReserve( int pCount ) = 0;
//virtual void variantClear( void ) = 0;
//virtual void variantAppend( const QVariant &pValue ) = 0;

//-------------------------------------------------------------------------
// Register fugio.pin

int LuaExPin::lua_openpin( lua_State *L )
{
	 /* metatable = {} */

	luaL_newmetatable( L, "fugio.pin" );

	luaL_setfuncs( L, mLuaInstance, 0 );

	luaL_newlib( L, mLuaPinFunctions );

	return( 1 );
}

int LuaExPin::luaGet( lua_State *L )
{
	QSharedPointer<fugio::PinInterface>	 P = LuaPlugin::getpin( L );

	if( lua_type( L, 2 ) == LUA_TNUMBER )
	{
		int									 i = lua_tointeger( L, 2 );

		fugio::VariantInterface				*V = nullptr;

		if( P->direction() == PIN_INPUT )
		{
			QSharedPointer<fugio::PinInterface>	 C = P->connectedPin();

			if( C && C->hasControl() )
			{
				V = qobject_cast<VariantInterface *>( C->control()->qobject() );
			}
		}
		else if( P->hasControl() )
		{
			V = qobject_cast<VariantInterface *>( P->control()->qobject() );
		}

		if( !V )
		{
			return( 0 );
		}

		if( i <= 0 || i > V->variantCount() )
		{
			return( 0 );
		}

		return( LuaPlugin::pushVariant( L, V->variant( i - 1 ) ) );
	}

	const char	*s = luaL_checkstring( L, 2 );

	for( const luaL_Reg *F = mLuaPinMethods ; F->func ; F++ )
	{
		if( !strcmp( s, F->name ) )
		{
			lua_pushcfunction( L, F->func );

			return( 1 );
		}
	}

	QUuid		ControlUuid;

	if( P->direction() == PIN_OUTPUT )
	{
		ControlUuid = P->controlUuid();
	}
	else if( P->isConnected() )
	{
		ControlUuid = P->connectedPin()->controlUuid();
	}

	for( const luaL_Reg &F : LuaPlugin::instance()->pinFunctions( ControlUuid ) )
	{
		if( !strcmp( s, F.name ) )
		{
			lua_pushcfunction( L, F.func );

			return( 1 );
		}
	}

	return( 0 );
}

int LuaExPin::luaSet( lua_State *L )
{
	QSharedPointer<fugio::PinInterface>	 P = LuaPlugin::getpin( L );

	if( !P )
	{
		return( 0 );
	}

	if( P->direction() == PIN_OUTPUT && P->hasControl() )
	{
		int			i = 1;
		QVariant	v;
		int			DataType = LUA_TNONE;

		if( lua_gettop( L ) == 4 )
		{
			if( lua_type( L, 2 ) == LUA_TNUMBER )
			{
				i = lua_tointeger( L, 2 );
				v = LuaPlugin::popVariant( L, 3 );

				DataType = lua_type( L, 3 );
			}
		}
		else if( lua_gettop( L ) == 3 )
		{
			v = LuaPlugin::popVariant( L, 2 );

			DataType = lua_type( L, 2 );
		}

		fugio::VariantInterface				*V = qobject_cast<VariantInterface *>( P->control()->qobject() );

		if( V )
		{
			if( i <= 0 || i > V->variantCount() )
			{
				return( 0 );
			}

			if( DataType == LUA_TUSERDATA )
			{
				V->setVariant( i - 1, v );
			}
			else
			{
				V->setFromBaseVariant( i - 1, v );
			}
		}
	}

	return( 0 );
}

int LuaExPin::luaPinSetCount( lua_State *L )
{
	QSharedPointer<fugio::PinInterface>	 P = LuaPlugin::getpin( L );

	if( !P )
	{
		return( 0 );
	}

	int		NewCnt = luaL_checkinteger( L, 2 );

	if( NewCnt < 0 )
	{
		return( 0 );
	}

	if( P->direction() == PIN_OUTPUT && P->hasControl() )
	{
		fugio::VariantInterface				*V = qobject_cast<VariantInterface *>( P->control()->qobject() );

		if( V && V->variantCount() != NewCnt )
		{
			V->setVariantCount( NewCnt );
		}
	}

	return( 0 );
}

int LuaExPin::luaPinLen( lua_State *L )
{
	QSharedPointer<fugio::PinInterface>	 P = LuaPlugin::getpin( L );

	if( !P )
	{
		return( 0 );
	}

	fugio::VariantInterface				*V = nullptr;

	if( P->direction() == PIN_INPUT )
	{
		QSharedPointer<fugio::PinInterface>	 C = P->connectedPin();

		if( C && C->hasControl() )
		{
			V = qobject_cast<VariantInterface *>( C->control()->qobject() );
		}
	}
	else if( P->hasControl() )
	{
		V = qobject_cast<VariantInterface *>( P->control()->qobject() );
	}

	if( !V )
	{
		return( 0 );
	}

	lua_pushinteger( L, V->variantCount() );

	return( 1 );
}

int LuaExPin::luaToString( lua_State *L )
{
	return( luaPinGetName( L ) );
}

int LuaExPin::luaUpdated(lua_State *L)
{
	QSharedPointer<fugio::PinInterface>	 P = LuaPlugin::getpin( L );

	if( !P )
	{
		return( 0 );
	}

	lua_pushinteger( L, P->updated() );

	return( 1 );
}

int LuaExPin::luaIsUpdated( lua_State *L )
{
	QSharedPointer<fugio::PinInterface>	 P = LuaPlugin::getpin( L );

	if( !P )
	{
		return( 0 );
	}

	qint64			TimeStamp = luaL_checkinteger( L, 2 );

	lua_pushboolean( L, P->isUpdated( TimeStamp ) );

	return( 1 );
}

int LuaExPin::luaUpdate( lua_State *L )
{
	QSharedPointer<fugio::PinInterface>	 P = LuaPlugin::getpin( L );

	if( !P || P->direction() == PIN_INPUT )
	{
		return( 0 );
	}

	P->node()->context()->pinUpdated( P );

	return( 0 );
}

int LuaExPin::luaPinGetName( lua_State *L )
{
	QSharedPointer<fugio::PinInterface>	 P = LuaPlugin::getpin( L );

	if( !P )
	{
		return( 0 );
	}

	lua_pushfstring( L, "%s", P->name().toLatin1().data() );

	return( 1 );
}

int LuaExPin::luaPinSetValue( lua_State *L )
{
	QSharedPointer<fugio::PinInterface>	 P = LuaPlugin::getpin( L );

	if( P->direction() != PIN_OUTPUT || !P->hasControl() )
	{
		qDebug() << "No Output Pin";

		return( 0 );
	}

	luaL_checkany( L, 2 );

	VariantInterface	*IV = qobject_cast<VariantInterface *>( P->control()->qobject() );

	if( !IV )
	{
		qDebug() << "No fugio::VariantInterface";

		return( 0 );
	}

	LuaPlugin::luaPinSetFunc		PinSetFnc = LuaPlugin::instance()->setFunctions().value( P->controlUuid() );

	if( PinSetFnc )
	{
		return( PinSetFnc( P->localId(), L, 2 ) );
	}

	QVariant			 V = LuaPlugin::popVariant( L, 2 );

	if( V.isValid() )
	{
		if( V.userType() == QVariant::List )
		{
			QVariantList	VL = V.toList();

			for( int i = 0 ; i < VL.size() ; i++ )
			{
				IV->setFromBaseVariant( i, VL.at( i ) );
			}
		}
		else
		{
			IV->setFromBaseVariant( V );
		}

		P->node()->context()->pinUpdated( P );
	}
	else
	{
		qDebug() << "Unknown output type" << lua_type( L, 2 ) << lua_typename( L, lua_type( L, 2 ) );
	}

	return( 0 );
}

int LuaExPin::luaPinGetValue( lua_State *L )
{
	QSharedPointer<fugio::PinInterface>	 P = LuaPlugin::getpin( L );

	if( !P )
	{
		return( 0 );
	}

	int						ArgCnt = 0;

	QVariant				V = ( P->direction() == PIN_INPUT ? P->value() : QVariant() );

	QSharedPointer<fugio::PinInterface>				PinSrc;
	QSharedPointer<fugio::PinControlInterface>		PinCtl;

	if( P->direction() == PIN_INPUT )
	{
		PinSrc = ( P->isConnected() && P->connectedPin()->hasControl() ? P->connectedPin() : QSharedPointer<fugio::PinInterface>() );
	}
	else
	{
		PinSrc = P;
	}

	if( PinSrc )
	{
		PinCtl = PinSrc->control();
	}

	if( PinCtl )
	{
		QObject							*CtlObj = PinCtl->qobject();
		LuaPlugin::luaPinGetFunc		PinGetFnc = LuaPlugin::instance()->getFunctions().value( PinSrc->controlUuid() );

		if( PinGetFnc )
		{
			return( PinGetFnc( P->localId(), L ) );
		}

//		fugio::ArrayListInterface	*ArLInt = qobject_cast<fugio::ArrayListInterface *>( CtlObj );

//		if( ArLInt )
//		{
//			LuaArray::pusharray( L, CtlObj, P->direction() == PIN_INPUT );

//			return( 1 );
//		}

		fugio::ArrayInterface		*ArrInt = qobject_cast<fugio::ArrayInterface *>( CtlObj );

		if( ArrInt )
		{
			LuaArray::pusharray( L, CtlObj, P->direction() == PIN_INPUT );

			return( 1 );
		}

//		fugio::ListInterface		*LstInt = qobject_cast<fugio::ListInterface *>( CtlObj );

//		if( LstInt )
//		{
//			LuaArray::pusharray( L, CtlObj, P->direction() == PIN_INPUT );

//			return( 1 );
//		}

		fugio::VariantInterface			*IV = qobject_cast<fugio::VariantInterface *>( CtlObj );

		if( IV )
		{
			if( IV->variantCount() > 1 )
			{
				QVariantList	VL;

				for( int i = 0 ; i < IV->variantCount() ; i++ )
				{
					VL << IV->baseVariant( i );
				}

				V = VL;
			}
			else
			{
				V = IV->baseVariant();
			}
		}
	}

	ArgCnt = LuaPlugin::pushVariant( L, V );

	if( !ArgCnt )
	{
		lua_pushfstring( L, "%s", V.toString().toLatin1().data() );

		ArgCnt = 1;
	}

	return( ArgCnt );
}

#endif
