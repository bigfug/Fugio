#include "luaqtplugin.h"

#include <fugio/global_interface.h>
#include <fugio/global_signals.h>

#include <fugio/nodecontrolbase.h>

#include <fugio/lua/uuid.h>
#include <fugio/image/uuid.h>
#include <fugio/luaqt/uuid.h>
#include <fugio/colour/uuid.h>

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
	{ "painter", LuaPainter::luaNew },
	{ "brush", LuaBrush::luaNew },
	{ "color", LuaColor::luaNew },
	{ "font", LuaFont::luaNew },
	{ "fontmetrics", LuaFontMetrics::luaNew },
	{ "gradient", LuaGradient::luaNew },
	{ "image", LuaImage::luaNew },
	{ "pen", LuaPen::luaNew },
	{ "point", LuaPointF::luaNew },
	{ "rect", LuaRectF::luaNew },
	{ "size", LuaSizeF::luaNew },
	{ "transform", LuaTransform::luaNew },
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

PluginInterface::InitResult LuaQtPlugin::initialise( fugio::GlobalInterface *pApp )
{
	mApp = pApp;

	LuaInterface		*LUA = lua();

	if( !LUA )
	{
		return( INIT_DEFER );
	}

	mApp->registerNodeClasses( NodeClasses );

	mApp->registerPinClasses( PinClasses );

//	LUA->luaAddPinFunction( PID_IMAGE, "resize", LuaQtPlugin::luaImageResize );
//	LUA->luaAddPinFunction( PID_IMAGE, "rect", LuaQtPlugin::luaImageRect );
//	LUA->luaAddPinFunction( PID_IMAGE, "size", LuaQtPlugin::luaImageSize );

	LUA->luaRegisterLibrary( "qt", LuaQtPlugin::luaOpen );

	LUA->luaRegisterExtension( LuaBrush::luaOpen );
	LUA->luaRegisterExtension( LuaColor::luaOpen );
	LUA->luaRegisterExtension( LuaFont::luaOpen );
	LUA->luaRegisterExtension( LuaFontMetrics::luaOpen );
	LUA->luaRegisterExtension( LuaGradient::luaOpen );
	LUA->luaRegisterExtension( LuaImage::luaOpen );
	LUA->luaRegisterExtension( LuaPainter::luaOpen );
	LUA->luaRegisterExtension( LuaPen::luaOpen );
	LUA->luaRegisterExtension( LuaPointF::luaOpen );
	LUA->luaRegisterExtension( LuaSizeF::luaOpen );
	LUA->luaRegisterExtension( LuaTransform::luaOpen );
	LUA->luaRegisterExtension( LuaRectF::luaOpen );

	LUA->luaAddPinGet( PID_COLOUR, LuaColor::luaPinGet );
	LUA->luaAddPinGet( PID_IMAGE, LuaImage::luaPinGet );

	return( INIT_OK );
}

void LuaQtPlugin::deinitialise( void )
{
	mApp->unregisterPinClasses( PinClasses );

	mApp->unregisterNodeClasses( NodeClasses );

	mApp = 0;
}

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

/*
int LuaQtPlugin::luaImageRect( lua_State *L )
{
	LuaInterface		*Lua  = LuaQtPlugin::lua();
	NodeInterface		*Node = Lua->node( L );
	PinInterface		*Pin  = Node->findPinByGlobalId( Lua->checkpin( L, 1 ) ).data();

	if( !Node || !Pin )
	{
		return( 0 );
	}

	QSharedPointer<fugio::PinControlInterface>	PinCtl;

	if( Pin->direction() == PIN_OUTPUT )
	{
		PinCtl = Pin->control();
	}
	else if( Pin->isConnected() )
	{
		PinCtl = Pin->connectedPin()->control();
	}

	if( !PinCtl )
	{
		return( 0 );
	}

	fugio::ImageInterface		*Image = qobject_cast<fugio::ImageInterface *>( PinCtl->qobject() );

	if( !Image )
	{
		return( luaL_error( L, "Source is not an image" ) );
	}

	LuaRectF::pushrectf( L, QRectF( QPoint(), Image->size() ) );

	return( 1 );
}

int LuaQtPlugin::luaImageResize( lua_State *L )
{
	LuaInterface		*Lua  = LuaQtPlugin::lua();
	NodeInterface		*Node = Lua->node( L );
	PinInterface		*Pin  = Node->findPinByGlobalId( Lua->checkpin( L, 1 ) ).data();

	if( !Node || !Pin || !Pin->hasControl() )
	{
		return( 0 );
	}

	fugio::ImageInterface		*Image = qobject_cast<fugio::ImageInterface *>( Pin->control()->qobject() );

	if( !Image )
	{
		return( luaL_error( L, "Source is not an image" ) );
	}

	int			w = luaL_checkinteger( L, 2 );
	int			h = luaL_checkinteger( L, 3 );

	if( Image->width() != w || Image->height() != h )
	{
		Image->setFormat( fugio::ImageInterface::FORMAT_RGBA8 );
		Image->setSize( w, h );
		Image->setLineSize( 0, w * 4 );

		Node->context()->pinUpdated( Node->findPinByGlobalId( Pin->globalId() ) );
	}

	return( 0 );
}

int LuaQtPlugin::luaImageSize(lua_State *L)
{
	LuaInterface		*Lua  = LuaQtPlugin::lua();
	NodeInterface		*Node = Lua->node( L );
	PinInterface		*Pin  = Node->findPinByGlobalId( Lua->checkpin( L, 1 ) ).data();

	if( !Node || !Pin )
	{
		return( 0 );
	}

	QSharedPointer<fugio::PinControlInterface>	PinCtl;

	if( Pin->direction() == PIN_OUTPUT )
	{
		PinCtl = Pin->control();
	}
	else if( Pin->isConnected() )
	{
		PinCtl = Pin->connectedPin()->control();
	}

	if( !PinCtl )
	{
		return( 0 );
	}

	fugio::ImageInterface		*Image = qobject_cast<fugio::ImageInterface *>( PinCtl->qobject() );

	if( !Image )
	{
		return( luaL_error( L, "Source is not an image" ) );
	}

	LuaSizeF::pushsizef( L, Image->size() );

	return( 1 );
}

int LuaQtPlugin::luaGetImage( const QUuid &pPinId, lua_State *L )
{
	LuaInterface							*Lua  = LuaQtPlugin::lua();
	NodeInterface							*Node = Lua->node( L );
	QSharedPointer<fugio::PinInterface>		 Pin = Node->findPinByGlobalId( pPinId );


	return( 0 );
}

*/
