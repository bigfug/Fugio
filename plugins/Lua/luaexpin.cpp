#include "luaexpin.h"

#include <QVector2D>
#include <QVector3D>

#include <fugio/global_interface.h>
#include <fugio/core/variant_interface.h>
#include <fugio/lua/lua_interface.h>
#include <fugio/context_interface.h>
#include <fugio/global_signals.h>
#include <fugio/node_interface.h>
#include <fugio/node_signals.h>
#include <fugio/core/array_interface.h>
#include <fugio/core/list_interface.h>

#include "luaplugin.h"
#include "luaarray.h"

using namespace fugio;

const luaL_Reg LuaExPin::mLuaPinFunctions[] =
{
	{ 0, 0 }
};

const luaL_Reg LuaExPin::mLuaInstance[] =
{
	{ "__index",	LuaExPin::luaGet },
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
	{ 0, 0 }
};

LuaExPin::LuaExPin()
{

}

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

	const char	*s = luaL_checkstring( L, 2 );

	for( const luaL_Reg *F = mLuaPinMethods ; F->func ; F++ )
	{
		if( !strcmp( s, F->name ) )
		{
			lua_pushcfunction( L, F->func );

			return( 1 );
		}
	}

	for( const luaL_Reg &F : LuaPlugin::instance()->pinFunctions( P->controlUuid() ) )
	{
		if( !strcmp( s, F.name ) )
		{
			lua_pushcfunction( L, F.func );

			return( 1 );
		}
	}

	return( 0 );
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

	QVariant			 V = LuaPlugin::popVariant( L, 2 );

	if( V.isValid() )
	{
		IV->setFromBaseVariant( V );

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

	QSharedPointer<fugio::PinControlInterface>		PinCtl;

	if( P->direction() == PIN_INPUT )
	{
		PinCtl = ( P->isConnected() && P->connectedPin()->hasControl() ? P->connectedPin()->control() : QSharedPointer<fugio::PinControlInterface>() );
	}
	else
	{
		PinCtl = P->control();
	}

	if( PinCtl )
	{
		fugio::ArrayInterface		*ArrInt = qobject_cast<fugio::ArrayInterface *>( PinCtl->qobject() );

		if( ArrInt )
		{
			LuaArray::pusharray( L, PinCtl->qobject(), P->direction() == PIN_INPUT );

			return( 1 );
		}

		fugio::ListInterface		*LstInt = qobject_cast<fugio::ListInterface *>( PinCtl->qobject() );

		if( LstInt )
		{
			LuaArray::pusharray( L, PinCtl->qobject(), P->direction() == PIN_INPUT );

			return( 1 );
		}

		VariantInterface			*IV = qobject_cast<VariantInterface *>( PinCtl->qobject() );

		if( IV )
		{
			V = IV->baseVariant();
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

