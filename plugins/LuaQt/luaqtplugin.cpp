#include "luaqtplugin.h"

#include <QTranslator>
#include <QApplication>

#include <fugio/global_interface.h>
#include <fugio/global_signals.h>

#include <fugio/nodecontrolbase.h>

#include <fugio/lua/uuid.h>
#include <fugio/image/uuid.h>
#include <fugio/luaqt/uuid.h>
#include <fugio/colour/uuid.h>
#include <fugio/math/uuid.h>
#include <fugio/core/uuid.h>
#include <fugio/json/uuid.h>

#include <fugio/image/image.h>

#include "luapainter.h"
#include "luapen.h"
#include "luacolor.h"
#include "luabrush.h"
#include "luarectf.h"
#include "luapointf.h"
#include "luasizef.h"
#include "luafont.h"
#include "luafontmetrics.h"
#include "luagradient.h"
#include "luaimage.h"
#include "luatransform.h"
#include "luamatrix4x4.h"
#include "luajsondocument.h"
#include "luajsonarray.h"
#include "luajsonobject.h"
#include "luavector3.h"
#include "luaquaternion.h"
#include "lualine.h"
#include "luapolygon.h"
#include "luamatrix.h"
#include "luabytearray.h"

QList<QUuid>	NodeControlBase::PID_UUID;

fugio::GlobalInterface		*LuaQtPlugin::mApp = Q_NULLPTR;
LuaQtPlugin					*LuaQtPlugin::mInstance = Q_NULLPTR;

QVector<luaL_Reg>			 LuaQtPlugin::mLuaFunctions;
QVector<luaL_Reg>			 LuaQtPlugin::mLuaMethods;

LuaQtPlugin::LuaQtPlugin()
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

LuaInterface *LuaQtPlugin::lua()
{
	return( qobject_cast<LuaInterface *>( mApp->findInterface( IID_LUA ) ) );
}

void LuaQtPlugin::addLuaFunction( const char *pName, lua_CFunction pFunction )
{
	luaL_Reg	LR{ pName, pFunction };

	mLuaFunctions << LR;
}

void LuaQtPlugin::addLuaFunction( luaL_Reg R )
{
	mLuaFunctions << R;
}

PluginInterface::InitResult LuaQtPlugin::initialise( fugio::GlobalInterface *pApp, bool pLastChance )
{
	mApp = pApp;

	LuaInterface		*LUA = lua();

	if( !LUA )
	{
		return( pLastChance ? INIT_FAILED : INIT_DEFER );
	}

#if defined( LUA_SUPPORTED )
	LUA->luaRegisterLibrary( "qt", LuaQtPlugin::luaOpen );

	//--------------------------------------------------------------------------

	LuaBrush::registerExtension( LUA );
	LuaByteArray::registerExtension( LUA );
	LuaColor::registerExtension( LUA );
	LuaFont::registerExtension( LUA );
	LuaFontMetrics::registerExtension( LUA );
	LuaGradient::registerExtension( LUA );
	LuaImage::registerExtension( LUA );
	LuaJsonArray::registerExtension( LUA );
	LuaJsonDocument::registerExtension( LUA );
	LuaJsonObject::registerExtension( LUA );
	LuaLine::registerExtension( LUA );
	LuaMatrix::registerExtension( LUA );
	LuaMatrix4x4::registerExtension( LUA );
	LuaPainter::registerExtension( LUA );
	LuaPen::registerExtension( LUA );
	LuaPointF::registerExtension( LUA );
	LuaPolygon::registerExtension( LUA );
	LuaQuaternion::registerExtension( LUA );
	LuaRectF::registerExtension( LUA );
	LuaSizeF::registerExtension( LUA );
	LuaTransform::registerExtension( LUA );
	LuaVector3D::registerExtension( LUA );

	//--------------------------------------------------------------------------

	luaL_Reg	LR{ Q_NULLPTR, Q_NULLPTR };

	mLuaFunctions << LR;
	mLuaMethods   << LR;
#endif

	return( INIT_OK );
}

void LuaQtPlugin::deinitialise( void )
{
	mApp = Q_NULLPTR;
}

#if defined( LUA_SUPPORTED )
int LuaQtPlugin::luaOpen( lua_State *L )
{
	luaL_newmetatable( L, "fugio.qt" );

	/* metatable.__index = metatable */
//	lua_pushvalue(L, -1);  /* duplicates the metatable */
//	lua_setfield(L, -2, "__index");

	luaL_setfuncs( L, mLuaMethods.constData(), 0 );

	luaL_newlib( L, mLuaFunctions.constData() );

	return( 1 );
}
#endif

