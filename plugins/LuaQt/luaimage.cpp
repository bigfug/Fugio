#include "luaimage.h"

#include <fugio/node_interface.h>
#include <fugio/node_control_interface.h>
#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>
#include <fugio/image/image.h>
#include <fugio/context_interface.h>

#include "luaqtplugin.h"
#include "luarectf.h"
#include "luasizef.h"
#include "luapointf.h"
#include "luacolor.h"

const char *LuaImage::ImageUserData::TypeName = "qt.image";

#if defined( LUA_SUPPORTED )

const luaL_Reg LuaImage::mLuaInstance[] =
{
	{ 0, 0 }
};

const luaL_Reg LuaImage::mLuaMethods[] =
{
	{ "__gc",				LuaImage::luaDelete },
	{ "isValid",			LuaImage::luaIsValid },
	{ "pixelColor",			LuaImage::luaPixelColor },
	{ "rect",				LuaImage::luaRect },
	{ "setPixelColor",		LuaImage::luaSetPixelColor },
	{ "setSize",			LuaImage::luaSetSize },
	{ "size",				LuaImage::luaSize },
	{ 0, 0 }
};

int LuaImage::luaOpen( lua_State *L )
{
	luaL_newmetatable( L, ImageUserData::TypeName );

	lua_pushvalue( L, -1 );
	lua_setfield( L, -2, "__index" );

	luaL_setfuncs( L, mLuaMethods, 0 );

	luaL_newlib( L, mLuaInstance );

	return( 1 );
}

int LuaImage::luaNew( lua_State *L )
{
	int			w = luaL_checkinteger( L, 2 );
	int			h = luaL_checkinteger( L, 3 );

	return( pushimage( L, QImage( w, h, QImage::Format_ARGB32_Premultiplied ), QUuid() ) );
}

int LuaImage::luaPinGet( const QUuid &pPinLocalId, lua_State *L )
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
		return( luaL_error( L, "No image pin" ) );
	}

	fugio::VariantInterface					*SrcImg = qobject_cast<fugio::VariantInterface *>( PinSrc->control()->qobject() );

	if( !SrcImg )
	{
		return( luaL_error( L, "Can't access image" ) );
	}

	return( pushimage( L, SrcImg, Pin->direction() == PIN_INPUT ? QUuid() : Pin->globalId() ) );
}

int LuaImage::luaDelete( lua_State *L )
{
	ImageUserData		*IUD = checkimagedata( L );

	if( IUD && IUD->mImage )
	{
		delete IUD->mImage;
	}

	return( 0 );
}

int LuaImage::luaIsValid(lua_State *L)
{
	ImageUserData		*IUD = checkimagedata( L );

	lua_pushboolean( L, IUD->mImage != nullptr );

	return( 1 );
}

int LuaImage::luaPixelColor( lua_State *L )
{
	ImageUserData		*IUD = checkimagedata( L );
	QPoint				 P;

	luaL_argcheck( L, IUD->mImage != nullptr, 1, "image is not valid" );

	if( LuaPointF::isPointF( L, 2 ) )
	{
		P = LuaPointF::checkpointf( L, 2 ).toPoint();
	}
	else
	{
		int			x = luaL_checkinteger( L, 2 );
		int			y = luaL_checkinteger( L, 3 );

		P = QPoint( x, y );
	}

#if QT_VERSION >= QT_VERSION_CHECK( 5, 6, 0 )
	LuaColor::pushcolor( L, IUD->mImage->pixelColor( P ) );
#else
	LuaColor::pushcolor( L, QColor( IUD->mImage->pixel( P ) ) );
#endif

	return( 1 );
}

int LuaImage::luaRect( lua_State *L )
{
	ImageUserData		*IUD = checkimagedata( L );

	luaL_argcheck( L, IUD->mImage != nullptr, 1, "image is not valid" );

	LuaRectF::pushrectf( L, QRectF( QPoint(), IUD->mImage->size() ) );

	return( 1 );
}

int LuaImage::luaSetPixelColor( lua_State *L )
{
	ImageUserData			*IUD = checkimagedata( L );

	luaL_argcheck( L, IUD->mImage != nullptr, 1, "image is not valid" );

	QPoint					 P;
	QColor					 C;

	if( LuaPointF::isPointF( L, 2 ) )
	{
		P = LuaPointF::checkpointf( L, 2 ).toPoint();

		C = LuaColor::checkcolor( L, 3 );
	}
	else
	{
		int			x = luaL_checkinteger( L, 2 );
		int			y = luaL_checkinteger( L, 3 );

		P = QPoint( x, y );

		C = LuaColor::checkcolor( L, 4 );
	}

#if QT_VERSION >= QT_VERSION_CHECK( 5, 6, 0 )
	IUD->mImage->setPixelColor( P, C );
#else
	IUD->mImage->setPixel( P, C.rgba() );
#endif
	return( 0 );
}

int LuaImage::luaSetSize( lua_State *L )
{
	ImageUserData			*IUD = checkimagedata( L );
	QSize					 S;

	if( LuaSizeF::isSizeF( L, 2 ) )
	{
		S = LuaSizeF::checksizef( L, 2 ).toSize();
	}
	else
	{
		int			x = luaL_checkinteger( L, 2 );
		int			y = luaL_checkinteger( L, 3 );

		S = QSize( x, y );
	}

	if( S.width() <= 0 || S.height() <= 0 )
	{
		luaL_error( L, "size is invalid" );
	}

	if( IUD->mImgInf )
	{
		fugio::Image	DstImg = IUD->mImgInf->variant().value<fugio::Image>();

		DstImg.setFormat( fugio::ImageFormat::BGRA8 );
		DstImg.setSize( S.width(), S.height() );
		DstImg.setLineSize( 0, S.width() * 4 );

		IUD->updateImage();
	}
	else if( IUD->mImage )
	{
		IUD->mImage = new QImage( S, IUD->mImage->format() );
	}

	return( 0 );
}

int LuaImage::luaSize( lua_State *L )
{
	ImageUserData		*IUD = checkimagedata( L );

	luaL_argcheck( L, IUD->mImage != nullptr, 1, "image is not valid" );

	LuaSizeF::pushsizef( L, IUD->mImage->size() );

	return( 1 );
}

#endif
