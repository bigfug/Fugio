#include "texturecubenode.h"

#include <fugio/core/uuid.h>
#include <fugio/context_interface.h>
#include <fugio/image/image_interface.h>

#include <QVector>

#include "openglplugin.h"

TextureCubeNode::TextureCubeNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	mPinImgTop    = pinInput( "Top (+Y)" );
	mPinImgBottom = pinInput( "Bottom (-Y)" );
	mPinImgLeft   = pinInput( "Left (-X)" );
	mPinImgRight  = pinInput( "Right (+X)" );
	mPinImgFront  = pinInput( "Front (+Z)" );
	mPinImgBack   = pinInput( "Back (-Z)" );

	mTexDst = pinOutput<OpenGLTextureInterface *>( "Texture", mPinTexDst, PID_OPENGL_TEXTURE );

	mTexDst->setTarget( GL_TEXTURE_CUBE_MAP );

	mPinTexDst->setDescription( tr( "The destination OpenGL Texture to copy the source texture into" ) );
}

void TextureCubeNode::processTexture( QSharedPointer<fugio::PinInterface> pPin, int pCubeMapIndex )
{
	fugio::ImageInterface							*I = input<fugio::ImageInterface *>( pPin );

	if( I == 0 || I->size().isEmpty() )
	{
		return;
	}

	QVector3D		TexSze = mTexDst->size();
	QVector3D		S      = QVector3D( I->size().width(), I->size().height(), 0 );

	if( TexSze != S )
	{
		mTexDst->free();

		mTexDst->setType( GL_UNSIGNED_BYTE );

		switch( I->format() )
		{
#if !defined( GL_ES_VERSION_2_0 )
			case fugio::ImageInterface::FORMAT_BGR8:
				mTexDst->setFormat( GL_BGR );

				mTexDst->setInternalFormat( GL_RGB8 );
				break;

			case fugio::ImageInterface::FORMAT_RGB8:
				mTexDst->setFormat( GL_RGB );

				mTexDst->setInternalFormat( GL_RGB8 );
				break;

			case fugio::ImageInterface::FORMAT_RGBA8:
				mTexDst->setFormat( GL_RGBA );

				mTexDst->setInternalFormat( GL_RGBA8 );
				break;

			case fugio::ImageInterface::FORMAT_BGRA8:
				mTexDst->setFormat( GL_BGRA );

				mTexDst->setInternalFormat( GL_RGBA8 );
				break;

			case fugio::ImageInterface::FORMAT_GRAY16:
				mTexDst->setType( GL_UNSIGNED_SHORT );

//				if( QOpenGLContext::currentContext()->format().version().first >= 3 )
//				{
//					mTexDst->setFormat( GL_RED );

//					mTexDst->setInternalFormat( GL_R16 );
//				}
//				else
				{
					mTexDst->setFormat( GL_LUMINANCE );

					mTexDst->setInternalFormat( GL_LUMINANCE16 );
				}
				break;

			case fugio::ImageInterface::FORMAT_GRAY8:
//				if( QOpenGLContext::currentContext()->format().version().first >= 3 )
//				{
//					mTexDst->setFormat( GL_RED );

//					mTexDst->setInternalFormat( GL_R8 );
//				}
//				else
				{
					mTexDst->setFormat( GL_LUMINANCE );

					mTexDst->setInternalFormat( GL_LUMINANCE8 );
				}
				break;

			case fugio::ImageInterface::FORMAT_YUYV422:
				mTexDst->setFormat( GL_RG );

				mTexDst->setInternalFormat( GL_RG8 );
				break;

			case fugio::ImageInterface::FORMAT_RG32:
				mTexDst->setFormat( GL_RG );

				mTexDst->setType( GL_FLOAT );

				mTexDst->setInternalFormat( GL_RG32F );
				break;
#endif

			default:
				return;
		}

		mTexDst->setFilter( GL_LINEAR, GL_LINEAR );

		mTexDst->setWrap( GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE );

		mTexDst->setSize( S );
	}

	OPENGL_PLUGIN_DEBUG

	mTexDst->update( I->buffer( 0 ), I->bufferSize( 0 ), I->lineSize( 0 ), pCubeMapIndex );

	OPENGL_PLUGIN_DEBUG
}

void TextureCubeNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	if( !OpenGLPlugin::hasContextStatic() )
	{
		return;
	}

	OPENGL_PLUGIN_DEBUG

#define CUBE_MAP_POSITIVE_X    0
#define CUBE_MAP_NEGATIVE_X    1
#define CUBE_MAP_POSITIVE_Y    2
#define CUBE_MAP_NEGATIVE_Y    3
#define CUBE_MAP_POSITIVE_Z    4
#define CUBE_MAP_NEGATIVE_Z    5

	processTexture( mPinImgTop,    CUBE_MAP_POSITIVE_Y );
	processTexture( mPinImgBottom, CUBE_MAP_NEGATIVE_Y );
	processTexture( mPinImgLeft,   CUBE_MAP_NEGATIVE_X );
	processTexture( mPinImgRight,  CUBE_MAP_POSITIVE_X );
	processTexture( mPinImgFront,  CUBE_MAP_NEGATIVE_Z );
	processTexture( mPinImgBack,   CUBE_MAP_POSITIVE_Z );

	if( mTexDst->dstTexId() )
	{
		pinUpdated( mPinTexDst );
	}
	else
	{
		mTexDst->setSize( 0 );
	}
}


bool TextureCubeNode::initialise()
{
	if( !fugio::NodeControlBase::initialise() )
	{
		return( false );
	}

	if( !OpenGLPlugin::hasContextStatic() )
	{
		return( false );
	}

	return( true );
}
