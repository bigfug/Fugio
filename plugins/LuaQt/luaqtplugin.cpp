#include "luaqtplugin.h"

#include <fugio/global_interface.h>
#include <fugio/global_signals.h>

#include <fugio/nodecontrolbase.h>

#include <fugio/lua/uuid.h>
#include <fugio/image/uuid.h>
#include <fugio/luaqt/uuid.h>
#include <fugio/colour/uuid.h>
#include <fugio/math/uuid.h>
#include <fugio/core/uuid.h>

#include <fugio/image/image_interface.h>

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

QList<QUuid>	NodeControlBase::PID_UUID;

fugio::GlobalInterface		*LuaQtPlugin::mApp = 0;
LuaQtPlugin					*LuaQtPlugin::mInstance = 0;

ClassEntry	NodeClasses[] =
{
	ClassEntry()
};

ClassEntry PinClasses[] =
{
	ClassEntry()
};

const luaL_Reg LuaQtPlugin::mLuaFunctions[] =
{
#if defined( LUA_SUPPORTED )
	{ "painter", LuaPainter::luaNew },
	{ "brush", LuaBrush::luaNew },
	{ "color", LuaColor::luaNew },
	{ "font", LuaFont::luaNew },
	{ "fontmetrics", LuaFontMetrics::luaNew },
	{ "gradient", LuaGradient::luaNew },
	{ "image", LuaImage::luaNew },
	{ "jsonarray", LuaJsonArray::luaNew },
	{ "jsondocument", LuaJsonDocument::luaNew },
	{ "jsonobject", LuaJsonObject::luaNew },
	{ "matrix4x4", LuaMatrix4x4::luaNew },
	{ "pen", LuaPen::luaNew },
	{ "point", LuaPointF::luaNew },
	{ "quaternion", LuaQuaternion::luaNew },
	{ "rect", LuaRectF::luaNew },
	{ "size", LuaSizeF::luaNew },
	{ "transform", LuaTransform::luaNew },
	{ "vector3d", LuaVector3D::luaNew },
#endif
	{ 0, 0 }
};

const luaL_Reg LuaQtPlugin::mLuaMethods[] =
{
	{ 0, 0 }
};

LuaInterface *LuaQtPlugin::lua()
{
	return( qobject_cast<LuaInterface *>( mApp->findInterface( IID_LUA ) ) );
}

PluginInterface::InitResult LuaQtPlugin::initialise( fugio::GlobalInterface *pApp, bool pLastChance )
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
	LUA->luaRegisterLibrary( "qt", LuaQtPlugin::luaOpen );

	LUA->luaRegisterExtension( LuaBrush::luaOpen );
	LUA->luaRegisterExtension( LuaColor::luaOpen );
	LUA->luaRegisterExtension( LuaFont::luaOpen );
	LUA->luaRegisterExtension( LuaFontMetrics::luaOpen );
	LUA->luaRegisterExtension( LuaGradient::luaOpen );
	LUA->luaRegisterExtension( LuaImage::luaOpen );
	LUA->luaRegisterExtension( LuaJsonArray::luaOpen );
	LUA->luaRegisterExtension( LuaJsonDocument::luaOpen );
	LUA->luaRegisterExtension( LuaJsonObject::luaOpen );
	LUA->luaRegisterExtension( LuaPainter::luaOpen );
	LUA->luaRegisterExtension( LuaPen::luaOpen );
	LUA->luaRegisterExtension( LuaPointF::luaOpen );
	LUA->luaRegisterExtension( LuaSizeF::luaOpen );
	LUA->luaRegisterExtension( LuaTransform::luaOpen );
	LUA->luaRegisterExtension( LuaRectF::luaOpen );
	LUA->luaRegisterExtension( LuaMatrix4x4::luaOpen );
	LUA->luaRegisterExtension( LuaVector3D::luaOpen );
	LUA->luaRegisterExtension( LuaQuaternion::luaOpen );

	LUA->luaAddPinGet( PID_COLOUR, LuaColor::luaPinGet );
	LUA->luaAddPinGet( PID_IMAGE, LuaImage::luaPinGet );
	LUA->luaAddPinGet( PID_MATRIX4, LuaMatrix4x4::luaPinGet );
	LUA->luaAddPinGet( PID_POINT, LuaPointF::luaPinGet );
	LUA->luaAddPinGet( PID_QUATERNION, LuaQuaternion::luaPinGet );
	LUA->luaAddPinGet( PID_RECT, LuaRectF::luaPinGet );
	LUA->luaAddPinGet( PID_SIZE, LuaSizeF::luaPinGet );
	LUA->luaAddPinGet( PID_VECTOR3, LuaVector3D::luaPinGet );
#endif

	return( INIT_OK );
}

void LuaQtPlugin::deinitialise( void )
{
	mApp->unregisterPinClasses( PinClasses );

	mApp->unregisterNodeClasses( NodeClasses );

	mApp = 0;
}

#if defined( LUA_SUPPORTED )
int LuaQtPlugin::luaOpen( lua_State *L )
{
	luaL_newmetatable( L, "fugio.qt" );

	/* metatable.__index = metatable */
	lua_pushvalue(L, -1);  /* duplicates the metatable */
	lua_setfield(L, -2, "__index");

	luaL_setfuncs( L, mLuaMethods, 0 );

	luaL_newlib( L, mLuaFunctions );

	return( 1 );
}
#endif

