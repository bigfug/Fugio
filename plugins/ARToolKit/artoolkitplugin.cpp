#include "artoolkitplugin.h"

#include <QTranslator>
#include <QCoreApplication>

#include <fugio/global_interface.h>
#include <fugio/global_signals.h>

#include <fugio/artoolkit/uuid.h>

#include <fugio/nodecontrolbase.h>

#include <fugio/lua/lua_interface.h>

#if defined( ARTOOLKIT_SUPPORTED )
#include <AR/ar.h>
#endif

#include "paramloadnode.h"
#include "trackernode.h"
#include "paramcameranode.h"

#include "parampin.h"

#if defined( Q_OS_WIN )
FILE _iob[] = {*stdin, *stdout, *stderr};

extern "C" FILE * __cdecl __iob_func(void)
{
	return _iob;
}
#endif

const luaL_Reg ARToolKitPlugin::mLuaFunctions[] =
{
#if defined( LUA_SUPPORTED )
	{ "paramLoad", ARToolKitPlugin::luaParamLoad },
#endif
	{ 0, 0 }
};

const luaL_Reg ARToolKitPlugin::mLuaMethods[] =
{
	{ 0, 0 }
};

QList<QUuid>	NodeControlBase::PID_UUID;

fugio::GlobalInterface		*ARToolKitPlugin::mApp = 0;
ARToolKitPlugin				*ARToolKitPlugin::mInstance = 0;

ClassEntry	NodeClasses[] =
{
	ClassEntry( "Param Load", "ARToolKit", NID_AR_PARAM_LOAD, &ParamLoadNode::staticMetaObject ),
	ClassEntry( "Camera", "ARToolKit", NID_AR_PARAM_CAMERA, &ParamCameraNode::staticMetaObject ),
	ClassEntry( "Tracker", "ARToolKit", NID_AR_TRACKER, &TrackerNode::staticMetaObject ),
	ClassEntry()
};

ClassEntry PinClasses[] =
{
	ClassEntry( "Param", "ARToolKit", PID_AR_PARAM, &ParamPin::staticMetaObject ),
	ClassEntry()
};

ARToolKitPlugin::ARToolKitPlugin()
{
	mInstance = this;

	//-------------------------------------------------------------------------
	// Install translator

	static QTranslator		Translator;

	if( Translator.load( QLocale(), QLatin1String( "translations" ), QLatin1String( "_" ), ":/" ) )
	{
		qApp->installTranslator( &Translator );
	}
}

LuaInterface *ARToolKitPlugin::lua()
{
	return( qobject_cast<LuaInterface *>( mApp->findInterface( IID_LUA ) ) );
}

PluginInterface::InitResult ARToolKitPlugin::initialise( fugio::GlobalInterface *pApp, bool pLastChance )
{
	mApp = pApp;

	LuaInterface		*LUA = lua();

	if( !LUA )
	{
		return( pLastChance ? INIT_FAILED : INIT_DEFER );
	}

	mApp->registerNodeClasses( NodeClasses );

	mApp->registerPinClasses( PinClasses );

#if defined( LUA_SUPPORTED )
	LUA->luaRegisterLibrary( "ar", ARToolKitPlugin::luaOpen );

//	LUA->luaRegisterExtension( LuaBrush::luaOpen );

//	LUA->luaAddPinGet( PID_COLOUR, LuaColor::luaPinGet );
#endif

	return( INIT_OK );
}

void ARToolKitPlugin::deinitialise( void )
{
	mApp->unregisterPinClasses( PinClasses );

	mApp->unregisterNodeClasses( NodeClasses );

	mApp = 0;
}

#if defined( LUA_SUPPORTED )
int ARToolKitPlugin::luaOpen( lua_State *L )
{
	luaL_newmetatable( L, "fugio.ar" );

	/* metatable.__index = metatable */
	lua_pushvalue(L, -1);  /* duplicates the metatable */
	lua_setfield(L, -2, "__index");

	luaL_setfuncs( L, mLuaMethods, 0 );

	luaL_newlib( L, mLuaFunctions );

	return( 1 );
}

int ARToolKitPlugin::luaParamLoad( lua_State *L )
{
#if !defined( ARTOOLKIT_SUPPORTED )
	Q_UNUSED( L )

	return( 0 );
#else
	const char	*Filename = luaL_checkstring( L, 1 );

	ARParam		 Param;

	if( arParamLoad( Filename, 1, &Param ) != 0 )
	{
		return( 0 );
	}

	ARParamLT	*ParamLT = arParamLTCreate( &Param, AR_PARAM_LT_DEFAULT_OFFSET );

	if( !ParamLT )
	{
		return( 0 );
	}

	ARParamLT	**UD = (ARParamLT **)lua_newuserdata( L, sizeof( ARParamLT * ) );

	if( !UD )
	{
		return( 0 );
	}

	*UD = ParamLT;

	luaL_getmetatable( L, "ar.paramlt" );
	lua_setmetatable( L, -2 );

	return( 1 );
#endif
}

#endif
