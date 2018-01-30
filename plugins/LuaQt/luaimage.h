#ifndef LUAIMAGE_H
#define LUAIMAGE_H

#if defined( LUA_SUPPORTED )
#include <lua.hpp>
#endif

#include <QImage>
#include <QUuid>

#include <fugio/image/image.h>
#include <fugio/core/variant_interface.h>

class LuaImage
{
private:
	typedef struct ImageUserData
	{
		static const char *TypeName;

		fugio::VariantInterface		*mImgInf;
		QImage						*mImage;
		QUuid						 mPinId;	// only set if this is a writable image

		inline bool isReadOnly( void ) const
		{
			return( mPinId.isNull() );
		}

		void updateImage( void )
		{
			if( !mImgInf )
			{
				return;
			}

			if( mImage )
			{
				delete mImage;

				mImage = nullptr;
			}

			fugio::Image	ImgDat = mImgInf->variant().value<fugio::Image>();

			if( ImgDat.isValid() )
			{
				QImage::Format		ImgFmt = fugio::Image::toQImageFormat( ImgDat.format() );

				if( ImgFmt != QImage::Format_Invalid )
				{
					if( isReadOnly() )
					{
						mImage = new QImage( ImgDat.buffer( 0 ), ImgDat.width(), ImgDat.height(), ImgDat.lineSize( 0 ), ImgFmt );
					}
					else
					{
						mImage = new QImage( ImgDat.internalBuffer( 0 ), ImgDat.width(), ImgDat.height(), ImgDat.lineSize( 0 ), ImgFmt );

						memset( ImgDat.internalBuffer( 0 ), 0, ImgDat.lineSize( 0 ) * ImgDat.height() );
					}
				}
			}
		}
	} ImageUserData;

public:
	LuaImage( void ) {}

	virtual ~LuaImage( void ) {}

#if defined( LUA_SUPPORTED )
	static int luaOpen( lua_State *L );

	static int luaNew( lua_State *L );

	static int luaPinGet( const QUuid &pPinLocalId, lua_State *L );

	static int pushimage( lua_State *L, fugio::VariantInterface *pImgInf, QUuid pPinId )
	{
		ImageUserData	*UD = (ImageUserData *)lua_newuserdata( L, sizeof( ImageUserData ) );

		if( !UD )
		{
			return( 0 );
		}

		luaL_getmetatable( L, ImageUserData::TypeName );
		lua_setmetatable( L, -2 );

		UD->mImgInf = pImgInf;
		UD->mPinId  = pPinId;
		UD->mImage  = nullptr;

		UD->updateImage();

		return( 1 );
	}

	static int pushimage( lua_State *L, const QImage &pImage, QUuid pPinId )
	{
		ImageUserData	*UD = (ImageUserData *)lua_newuserdata( L, sizeof( ImageUserData ) );

		if( !UD )
		{
			return( 0 );
		}

		luaL_getmetatable( L, ImageUserData::TypeName );
		lua_setmetatable( L, -2 );

		UD->mImgInf = nullptr;
		UD->mImage  = new QImage( pImage );
		UD->mPinId  = pPinId;

		return( 1 );
	}

	static QImage *checkimage( lua_State *L, int i = 1 )
	{
		ImageUserData		*ud = checkimagedata( L, i );

		return( ud->mImage );
	}

	static QImage *checkwritableimage( lua_State *L, int i = 1 )
	{
		ImageUserData		*ud = checkimagedata( L, i );

		luaL_argcheck( L, !ud->isReadOnly(), i, "image is read-only" );

		return( ud->mImage );
	}

	static bool isImage( lua_State *L, int i = 1 )
	{
		return( luaL_testudata( L, i, ImageUserData::TypeName ) != nullptr );
	}

private:
	static ImageUserData *checkimagedata( lua_State *L, int i = 1 )
	{
		ImageUserData *IUD = static_cast<ImageUserData *>( luaL_checkudata( L, i, ImageUserData::TypeName ) );

		luaL_argcheck( L, IUD != NULL, i, "image expected" );

		return( IUD );
	}

	static int luaDelete( lua_State *L );

	static int luaIsValid( lua_State *L );
	static int luaPixelColor( lua_State *L );
	static int luaRect( lua_State *L );
	static int luaSetPixelColor( lua_State *L );
	static int luaSetSize( lua_State *L );
	static int luaSize( lua_State *L );

private:
	static const luaL_Reg					mLuaInstance[];
	static const luaL_Reg					mLuaMethods[];
#endif
};

#endif // LUAIMAGE_H
