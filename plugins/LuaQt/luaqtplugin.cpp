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

QList<QUuid>	NodeControlBase::PID_UUID;

fugio::GlobalInterface		*LuaQtPlugin::mApp = Q_NULLPTR;
LuaQtPlugin					*LuaQtPlugin::mInstance = Q_NULLPTR;

QVector<luaL_Reg>			 LuaQtPlugin::mLuaFunctions;
QVector<luaL_Reg>			 LuaQtPlugin::mLuaMethods;

//const luaL_Reg LuaQtPlugin::mLuaFunctions[] =
//{
//#if defined( LUA_SUPPORTED )
//	{ "painter", LuaPainter::luaNew },
//	{ "color", LuaColor::luaNew },
//	{ "font", LuaFont::luaNew },
//	{ "fontmetrics", LuaFontMetrics::luaNew },
//	{ "gradient", LuaGradient::luaNew },
//	{ "image", LuaImage::luaNew },
//	{ "jsonarray", LuaJsonArray::luaNew },
//	{ "jsondocument", LuaJsonDocument::luaNew },
//	{ "jsonobject", LuaJsonObject::luaNew },
//	{ "line", LuaLine::luaNew },
//	{ "matrix4x4", LuaMatrix4x4::luaNewQt },
//	{ "pen", LuaPen::luaNew },
//	{ "point", LuaPointF::luaNew },
////	{ "quaternion", LuaQuaternion::luaNew },
//	{ "rect", LuaRectF::luaNew },
//	{ "size", LuaSizeF::luaNew },
//	{ "transform", LuaTransform::luaNew },
//	{ "vector3d", LuaVector3D::luaNewQt },
//#endif
//	{ Q_NULLPTR, Q_NULLPTR }
//};

//const luaL_Reg LuaQtPlugin::mLuaMethods[] =
//{
//	{ Q_NULLPTR, Q_NULLPTR }
//};

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

	LuaBrush::registerExtension( LUA );
	LuaColor::registerExtension( LUA );
	LuaFont::registerExtension( LUA );

	LUA->luaRegisterExtension( LuaFontMetrics::luaOpen );
	LUA->luaRegisterExtension( LuaGradient::luaOpen );
	LUA->luaRegisterExtension( LuaImage::luaOpen );
	LUA->luaRegisterExtension( LuaJsonArray::luaOpen );
	LUA->luaRegisterExtension( LuaJsonDocument::luaOpen );
	LUA->luaRegisterExtension( LuaJsonObject::luaOpen );
	LUA->luaRegisterExtension( LuaPainter::luaOpen );
	LUA->luaRegisterExtension( LuaPen::luaOpen );
	LUA->luaRegisterExtension( LuaPointF::luaOpen );
	LUA->luaRegisterExtension( LuaQuaternion::luaOpen );
	LUA->luaRegisterExtension( LuaSizeF::luaOpen );
	LUA->luaRegisterExtension( LuaTransform::luaOpen );
	LUA->luaRegisterExtension( LuaRectF::luaOpen );
	LUA->luaRegisterExtension( LuaLine::luaOpen );
	LUA->luaRegisterExtension( LuaMatrix4x4::luaOpen );
	LUA->luaRegisterExtension( LuaVector3D::luaOpen );

	LUA->luaAddPinGet( PID_IMAGE, LuaImage::luaPinGet );
	LUA->luaAddPinGet( PID_JSON, LuaJsonDocument::luaPinGet );
	LUA->luaAddPinGet( PID_MATRIX4, LuaMatrix4x4::luaPinGet );
	LUA->luaAddPinGet( PID_POINT, LuaPointF::luaPinGet );
	LUA->luaAddPinGet( PID_QUATERNION, LuaQuaternion::luaPinGet );
	LUA->luaAddPinGet( PID_RECT, LuaRectF::luaPinGet );
	LUA->luaAddPinGet( PID_SIZE, LuaSizeF::luaPinGet );
	LUA->luaAddPinGet( PID_VECTOR3, LuaVector3D::luaPinGet );
	LUA->luaAddPinGet( PID_LINE, LuaLine::luaPinGet );

	LUA->luaAddPinSet( PID_MATRIX4, LuaMatrix4x4::luaPinSet );
	LUA->luaAddPinSet( PID_QUATERNION, LuaQuaternion::luaPinSet );
	LUA->luaAddPinSet( PID_VECTOR3, LuaVector3D::luaPinSet );
	LUA->luaAddPinSet( PID_LINE, LuaLine::luaPinSet );

	LUA->luaAddPushVariantFunction( QMetaType::QLineF, LuaLine::pushVariant );
	LUA->luaAddPushVariantFunction( QMetaType::QPointF, LuaPointF::pushVariant );
	LUA->luaAddPushVariantFunction( QMetaType::QVector3D, LuaVector3D::pushVariant );
	LUA->luaAddPushVariantFunction( QMetaType::QJsonDocument, LuaJsonDocument::pushVariant );

	LUA->luaAddPopVariantFunction( LuaLine::mTypeName, LuaLine::popVariant );
	LUA->luaAddPopVariantFunction( LuaPointF::mTypeName, LuaPointF::popVariant );
	LUA->luaAddPopVariantFunction( LuaVector3D::mTypeName, LuaVector3D::popVariant );

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

