#include "luagradient.h"

#include "luaqtplugin.h"

#include "luacolor.h"
#include "luapointf.h"

const char *LuaGradient::GradientUserData::TypeName = "qt.gradient";

#if defined( LUA_SUPPORTED )

const luaL_Reg LuaGradient::mLuaInstance[] =
{
	{ 0, 0 }
};

const luaL_Reg LuaGradient::mLuaMethods[] =
{
	{ "__gc",				LuaGradient::luaDelete },
	{ "setColorAt",			LuaGradient::luaSetColorAt },
	{ 0, 0 }
};

void LuaGradient::registerExtension( fugio::LuaInterface *LUA )
{
	LuaQtPlugin::addLuaFunction( "gradient", LuaGradient::luaNew );

	LUA->luaRegisterExtension( LuaGradient::luaOpen );
}

int LuaGradient::luaOpen( lua_State *L )
{
	luaL_newmetatable( L, GradientUserData::TypeName );

	lua_pushvalue( L, -1 );
	lua_setfield( L, -2, "__index" );

	luaL_setfuncs( L, mLuaMethods, 0 );

	luaL_newlib( L, mLuaInstance );

	return( 1 );
}

int LuaGradient::luaNew( lua_State *L )
{
	const int LuaTop = lua_gettop( L );

	if( LuaTop == 0 )
	{
		luaL_error( L, "Not enough arguments" );
	}

	const char						*TypeString = luaL_checkstring( L, 1 );
	QGradient						*Gradient = nullptr;

	if( strcmp( TypeString, "linear" ) == 0 )
	{
		if( LuaTop == 1 )
		{
			Gradient = new QLinearGradient();
		}
		else if( LuaTop == 3 )
		{
			QPointF		S = LuaPointF::checkpointf( L, 2 );
			QPointF		E = LuaPointF::checkpointf( L, 3 );

			Gradient = new QLinearGradient( S, E );
		}
		else if( LuaTop == 5 )
		{
			// QLinearGradient(qreal x1, qreal y1, qreal x2, qreal y2)

			qreal		x1 = luaL_checknumber( L, 2 );
			qreal		y1 = luaL_checknumber( L, 3 );
			qreal		x2 = luaL_checknumber( L, 4 );
			qreal		y2 = luaL_checknumber( L, 5 );

			Gradient = new QLinearGradient( x1, y1, x2, y2 );
		}
		else
		{
			luaL_error( L, "Incorrect arguments for linear gradient" );
		}
	}
	else if( strcmp( TypeString, "conical" ) == 0 )
	{
		Gradient = new QConicalGradient();
	}
	else if( strcmp( TypeString, "radial" ) == 0 )
	{
		Gradient = new QRadialGradient();
	}

	if( !Gradient )
	{
		luaL_argerror( L, 1, "Unknown gradient type" );
	}

	pushgradient( L, Gradient );

	return( 1 );
}

int LuaGradient::luaDelete( lua_State *L )
{
	GradientUserData		*GradientData = checkgradientdata( L );

	if( GradientData )
	{
		if( GradientData->mGradient )
		{
			delete GradientData->mGradient;

			GradientData->mGradient = 0;
		}
	}

	return( 0 );
}

int LuaGradient::luaSetColorAt( lua_State *L )
{
	GradientUserData		*GradientData = checkgradientdata( L );

	if( lua_gettop( L ) != 3 )
	{
		luaL_error( L, "Incorrect arguments" );
	}

	qreal		P = luaL_checknumber( L, 2 );
	QColor		C = LuaColor::checkcolor( L, 3 );

	if( GradientData && GradientData->mGradient )
	{
		GradientData->mGradient->setColorAt( P, C );
	}

	return( 0 );
}

#endif
