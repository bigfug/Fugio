#include "luabrush.h"

#include "luaqtplugin.h"

#include "luacolor.h"
#include "luagradient.h"

const char *LuaBrush::BrushUserData::TypeName = "qt.brush";

#if defined( LUA_SUPPORTED )

const QMap<QString,Qt::BrushStyle>	LuaBrush::mBrushStyleMap =
{
	{ "none", Qt::NoBrush },
	{ "solid", Qt::SolidPattern },
	{ "dense1", Qt::Dense1Pattern },
	{ "dense2", Qt::Dense2Pattern },
	{ "dense3", Qt::Dense3Pattern },
	{ "dense4", Qt::Dense4Pattern },
	{ "dense5", Qt::Dense5Pattern },
	{ "dense6", Qt::Dense6Pattern },
	{ "dense7", Qt::Dense7Pattern },
	{ "hor", Qt::HorPattern },
	{ "ver", Qt::VerPattern },
	{ "cross", Qt::CrossPattern },
	{ "bdiag", Qt::BDiagPattern },
	{ "fdiag", Qt::FDiagPattern },
	{ "diagcross", Qt::DiagCrossPattern },
	{ "linear", Qt::LinearGradientPattern },
	{ "radial", Qt::RadialGradientPattern },
	{ "conical", Qt::ConicalGradientPattern },
	{ "texture", Qt::TexturePattern }
};

const luaL_Reg LuaBrush::mLuaInstance[] =
{
	{ 0, 0 }
};

const luaL_Reg LuaBrush::mLuaMethods[] =
{
	{ "__gc",				LuaBrush::luaDelete },
	{ "color",				LuaBrush::luaColor },
	{ "setColor",			LuaBrush::luaSetColor },
	{ "setStyle",			LuaBrush::luaSetStyle },
	{ "style",				LuaBrush::luaStyle },
	{ 0, 0 }
};

void LuaBrush::registerExtension( fugio::LuaInterface *LUA )
{
	LuaQtPlugin::addLuaFunction( "brush", LuaBrush::luaNew );

	LUA->luaRegisterExtension( LuaBrush::luaOpen );
}

int LuaBrush::luaOpen( lua_State *L )
{
	luaL_newmetatable( L, BrushUserData::TypeName );

	lua_pushvalue( L, -1 );
	lua_setfield( L, -2, "__index" );

	luaL_setfuncs( L, mLuaMethods, 0 );

	luaL_newlib( L, mLuaInstance );

	return( 1 );
}

int LuaBrush::luaNew( lua_State *L )
{
	const int	LuaTop = lua_gettop( L );

	if( LuaTop == 0 )
	{
		pushbrush( L, QBrush() );
	}
	else if( LuaTop == 1 )
	{
		if( lua_type( L, 1 ) == LUA_TSTRING )
		{
			const char		*s = lua_tostring( L, 1 );
			QString			 S( s ? s : "" );

			if( mBrushStyleMap.contains( S ) )
			{
				pushbrush( L, QBrush( mBrushStyleMap.value( S ) ) );
			}
			else
			{
				luaL_argcheck( L, false, 1, "Unknown type" );
			}
		}
		else if( lua_type( L, 1 ) == LUA_TUSERDATA )
		{
			if( LuaColor::isColor( L ) )
			{
				QColor		C = LuaColor::checkcolor( L );

				pushbrush( L, QBrush( C ) );
			}
			else if( LuaBrush::isBrush( L ) )
			{
				pushbrush( L, QBrush( *checkbrush( L ) ) );
			}
			else if( LuaGradient::isGradient( L ) )
			{
				pushbrush( L, QBrush( *LuaGradient::checkgradient( L ) ) );
			}
			else
			{
				luaL_argcheck( L, false, 1, "Unknown user data" );
			}
		}
		else
		{
			luaL_argcheck( L, false, 1, "Unknown argument" );
		}
	}
	else if( LuaTop == 2 )
	{
		if( LuaColor::isColor( L ) )
		{
			QColor		C = LuaColor::checkcolor( L );

			if( lua_type( L, 2 ) == LUA_TSTRING )
			{
				const char		*s = lua_tostring( L, 2 );
				QString			 S( s ? s : "" );

				if( mBrushStyleMap.contains( S ) )
				{
					pushbrush( L, QBrush( C, mBrushStyleMap.value( S ) ) );
				}
				else
				{
					luaL_argcheck( L, false, 2, "Unknown type" );
				}
			}
			else
			{
				luaL_argcheck( L, false, 2, "Unknown argument" );
			}
		}
		else
		{
			luaL_argcheck( L, false, 1, "Unknown argument" );
		}
	}
	else
	{
		return( luaL_error( L, "Wrong number of arguments (%d) to brush()", LuaTop ) );
	}

	return( 1 );
}

int LuaBrush::luaDelete( lua_State *L )
{
	void *ud = luaL_checkudata( L, 1, BrushUserData::TypeName );

	luaL_argcheck( L, ud != NULL, 1, "Brush expected" );

	delete static_cast<BrushUserData *>( ud )->mBrush;

	return( 0 );
}

int LuaBrush::luaColor( lua_State *L )
{
	QBrush		*Brush = checkbrush( L );

	LuaColor::pushcolor( L, Brush->color() );

	return( 1 );
}

int LuaBrush::luaSetColor( lua_State *L )
{
	QBrush		*Brush = checkbrush( L );
	QColor		 Color = LuaColor::checkcolor( L, 2 );

	Brush->setColor( Color );

	return( 0 );
}

int LuaBrush::luaSetStyle( lua_State *L )
{
	QBrush			*Brush = checkbrush( L );
	const char		*StyleNameRaw = luaL_checkstring( L, 2 );
	const QString	 StyleName( StyleNameRaw );

	if( !mBrushStyleMap.contains( StyleName ) )
	{
		return( luaL_error( L, "Unknown style: %s", StyleNameRaw ) );
	}

	Qt::BrushStyle	 Style = mBrushStyleMap.value( QString( StyleName ), Brush->style() );

	Brush->setStyle( Style );

	return( 0 );
}

int LuaBrush::luaStyle( lua_State *L )
{
	QBrush		*Brush = checkbrush( L );

	lua_pushfstring( L, "%s", mBrushStyleMap.key( Brush->style() ).toLatin1().constData() );

	return( 1 );
}

#endif
