#include "texturepin.h"
#include <QSettings>
#include <QImage>

#include <QSet>
#include <QByteArray>

#include <fugio/node_interface.h>

#include "openglplugin.h"

// reduces #if/#endif clutter

#if !defined( GL_TEXTURE_RECTANGLE )
#define GL_TEXTURE_RECTANGLE 0x84F5
#endif

TexturePin::TexturePin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin )
{
	mTextureFormat = GL_RGBA;
	mTextureInternalFormat = GL_RGBA;

	mTextureSize = QVector3D();

	mImageSize = QVector3D();

	mSrcTexId = 0;
	mDstTexId = 0;
	mTarget = 0;
	mTextureType = GL_UNSIGNED_BYTE;

	mTextureMinFilter = GL_NEAREST;
	mTextureMagFilter = GL_NEAREST;

	mTextureWrapX = GL_REPEAT;
	mTextureWrapY = GL_REPEAT;
	mTextureWrapZ = GL_REPEAT;

	mDefinitionChanged = true;

	mGenerateMipMaps = false;

	mFBOId = 0;
	mFBODepthRBId = 0;
	mFBOMSId = 0;
	mFBOMSColourRBId = 0;
	mFBOMSDepthRBId = 0;

	mDoubleBuffered = false;
}

TexturePin::~TexturePin( void )
{
	free();
}

void TexturePin::loadSettings( QSettings &pSettings )
{
	Q_UNUSED( pSettings )
}

void TexturePin::saveSettings( QSettings &pSettings ) const
{
	Q_UNUSED( pSettings )
}

QVector3D TexturePin::textureSize()
{
	return( mTextureSize );
}

QVector3D TexturePin::size()
{
	return( mImageSize );
}

quint32 TexturePin::srcTexId() const
{
	return( mDoubleBuffered ? mSrcTexId : mDstTexId );
}

quint32 TexturePin::dstTexId() const
{
	return( mDstTexId );
}

quint32 TexturePin::target()
{
	return( mTarget );
}

quint32 TexturePin::format()
{
	return( mTextureFormat );
}

quint32 TexturePin::internalFormat()
{
	return( mTextureInternalFormat );
}

quint32 TexturePin::type()
{
	return( mTextureType );
}

int TexturePin::filterMin( void ) const
{
	return( mTextureMinFilter );
}

int TexturePin::filterMag( void ) const
{
	return( mTextureMagFilter );
}

int TexturePin::wrapS( void ) const
{
	return( mTextureWrapX );
}

int TexturePin::wrapT( void ) const
{
	return( mTextureWrapY );
}

int TexturePin::wrapR() const
{
	return( mTextureWrapZ );
}

bool TexturePin::genMipMaps( void ) const
{
	return( mGenerateMipMaps );
}

void TexturePin::setSize( qint32 pWidth, qint32 pHeight, qint32 pDepth )
{
	setSize( QVector3D( pWidth, pHeight, pDepth ) );
}

unsigned int roundUpToNextPowerOfTwo(unsigned int x)
{
	x--;
	x |= x >> 1;  // handle  2 bit numbers
	x |= x >> 2;  // handle  4 bit numbers
	x |= x >> 4;  // handle  8 bit numbers
	x |= x >> 8;  // handle 16 bit numbers
	x |= x >> 16; // handle 32 bit numbers
	x++;

	return x;
}

void TexturePin::setSize( const QVector3D &pSize )
{
	if( pSize != mImageSize )
	{
		mImageSize = pSize;

		bool		TextureNPOT = false;

#if !defined( Q_OS_RASPBERRY_PI )
		if( GLEW_ARB_texture_non_power_of_two || GLEW_ARB_texture_storage || GLEW_VERSION_2_0 )
		{
			TextureNPOT = true;
		}
#endif

		if( !TextureNPOT )
		{
			mTextureSize.setX( roundUpToNextPowerOfTwo( mImageSize.x() ) );
			mTextureSize.setY( roundUpToNextPowerOfTwo( mImageSize.y() ) );
			mTextureSize.setZ( roundUpToNextPowerOfTwo( mImageSize.z() ) );

			qDebug() << "NPOT" << mImageSize << "->" << mTextureSize;
		}
		else
		{
			mTextureSize = mImageSize;
		}

		mDefinitionChanged = true;
	}
}

void TexturePin::setTarget(quint32 pTarget)
{
	if( pTarget != mTarget )
	{
		mTarget = pTarget;

		mDefinitionChanged = true;
	}
}

void TexturePin::setFormat( quint32 pFormat )
{
	if( pFormat != mTextureFormat )
	{
		mTextureFormat = pFormat;

		mDefinitionChanged = true;
	}
}

void TexturePin::setType( quint32 pType )
{
	if( mTextureType != pType )
	{
		mTextureType = pType;

		mDefinitionChanged = true;
	}
}

void TexturePin::setInternalFormat( quint32 pInternalFormat )
{
	if( mTextureInternalFormat != pInternalFormat )
	{
		mTextureInternalFormat = pInternalFormat;

		mDefinitionChanged = true;
	}
}

bool TexturePin::isCompressedFormat( GLenum pFormat )
{
#if defined( GL_EXT_texture_compression_s3tc )
	switch( pFormat )
	{
		case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
		case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
		case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
		case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
			return( true );
	}
#endif

	return( false );
}

void TexturePin::update( const unsigned char *pData, int pDataSize, int pCubeFaceIndex )
{
	if( !OpenGLPlugin::hasContextStatic() )
	{
		return;
	}

	OPENGL_PLUGIN_DEBUG

	checkDefinition();

	if( mImageSize.isNull() )
	{
		return;
	}

	if( !mDstTexId )
	{
		bool		TextureNPOT      = false;
		bool		TextureAllocated = false;

#if !defined( Q_OS_RASPBERRY_PI )
		if( GLEW_ARB_texture_non_power_of_two || GLEW_ARB_texture_storage || GLEW_VERSION_2_0 )
		{
			TextureNPOT = true;
		}
#endif

		glGenTextures( 1, &mDstTexId );

		if( !mDstTexId )
		{
			return;
		}

		dstBind();

		OPENGL_DEBUG( mPin->node()->name() );

#if !defined( Q_OS_RASPBERRY_PI )
		if( TextureNPOT && !isCompressedFormat( mTextureInternalFormat ) && GLEW_VERSION_4_2 )
		{
			switch( mTarget )
			{
				case GL_TEXTURE_1D:
					{
						glTexStorage1D( mTarget, 1, mTextureInternalFormat, mTextureSize.x() );

						TextureAllocated = true;
					}
				break;

				case GL_TEXTURE_2D:
				case GL_TEXTURE_RECTANGLE:
					{
						glTexStorage2D( mTarget, 1, mTextureInternalFormat, mTextureSize.x(), mTextureSize.y() );

						TextureAllocated = true;
					}
					break;

				case GL_TEXTURE_3D:
					{
						glTexStorage3D( mTarget, 1, mTextureInternalFormat, mTextureSize.x(), mTextureSize.y(), mTextureSize.z() );

						TextureAllocated = true;
					}
					break;
			}

			OPENGL_DEBUG( mPin->node()->name() );
		}
#endif

		if( !TextureAllocated && !isCompressedFormat( mTextureInternalFormat ) )
		{
			switch( mTarget )
			{
#if !defined( GL_ES_VERSION_2_0 )
				case GL_TEXTURE_1D:
					{
						glTexImage1D( mTarget, 0, mTextureInternalFormat, mTextureSize.x(), 0, mTextureFormat, mTextureType, 0 );

						TextureAllocated = true;
					}
					break;
#endif

				case GL_TEXTURE_2D:
				case GL_TEXTURE_RECTANGLE:
					{
						glTexImage2D( mTarget, 0, mTextureInternalFormat, mTextureSize.x(), mTextureSize.y(), 0, mTextureFormat, mTextureType, 0 );

						TextureAllocated = true;
					}
					break;

				case GL_TEXTURE_CUBE_MAP:
					{
						for( int i = 0 ; i < 6 ; i++ )
						{
							glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, mTextureInternalFormat, mTextureSize.x(), mTextureSize.y(), 0, mTextureFormat, mTextureType, 0 );
						}

						TextureAllocated = true;
					}
					break;

#if defined( GLEW_VERSION_1_2 )
				case GL_TEXTURE_3D:
					{
						glTexImage3D( mTarget, 0, mTextureInternalFormat, mTextureSize.x(), mTextureSize.y(), mTextureSize.z(), 0, mTextureFormat, mTextureType, 0 );

						TextureAllocated = true;
					}
					break;
#endif
			}

			OPENGL_DEBUG( mPin->node()->name() );
		}

		if( !isCompressedFormat( mTextureInternalFormat ) )
		{
			glTexParameteri( mTarget, GL_TEXTURE_MIN_FILTER, mTextureMinFilter );
			glTexParameteri( mTarget, GL_TEXTURE_MAG_FILTER, mTextureMagFilter );
		}
		else
		{
			glTexParameteri( mTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
			glTexParameteri( mTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		}

		OPENGL_DEBUG( mPin->node()->name() );

		glTexParameteri( mTarget, GL_TEXTURE_WRAP_S, mTextureWrapX );
		glTexParameteri( mTarget, GL_TEXTURE_WRAP_T, mTextureWrapY );

		if( GLEW_VERSION_1_2 )
		{
			glTexParameteri( mTarget, GL_TEXTURE_WRAP_R, mTextureWrapZ );
		}

		OPENGL_DEBUG( mPin->node()->name() );

		release();

		OPENGL_DEBUG( mPin->node()->name() );

		if( !TextureAllocated && !isCompressedFormat( mTextureInternalFormat ) )
		{
			qDebug() << "Couldn't allocate texture";

			return;
		}
	}

	if( pData )
	{
		dstBind();

		if( isCompressedFormat( mTextureInternalFormat ) )
		{
#if defined( GL_VERSION_1_3 )
				switch( mTarget )
				{
					case GL_TEXTURE_1D:
						glCompressedTexImage1D( mTarget, 0, mTextureInternalFormat, mImageSize.x(), 0, pDataSize, pData );
						break;

					case GL_TEXTURE_2D:
					case GL_TEXTURE_RECTANGLE:
						glCompressedTexImage2DARB( mTarget, 0, mTextureInternalFormat, mImageSize.x(), mImageSize.y(), 0, pDataSize, pData );
						break;

					case GL_TEXTURE_CUBE_MAP:
						glCompressedTexImage2DARB( GL_TEXTURE_CUBE_MAP_POSITIVE_X + pCubeFaceIndex, 0, mTextureInternalFormat, mImageSize.x(), mImageSize.y(), 0, pDataSize, pData );
						break;

					case GL_TEXTURE_3D:
						glCompressedTexImage3D( mTarget, 0, mTextureInternalFormat, mImageSize.x(), mImageSize.y(), mImageSize.z(), 0, pDataSize, pData );
						break;
				}
#endif
		}
		else
		{
			glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

			OPENGL_DEBUG( mPin->node()->name() );

			switch( mTarget )
			{
#if !defined( GL_ES_VERSION_2_0 )
				case GL_TEXTURE_1D:
					glTexSubImage1D( mTarget, 0, 0, mImageSize.x(), mTextureFormat, mTextureType, pData );
					break;
#endif

				case GL_TEXTURE_2D:
				case GL_TEXTURE_RECTANGLE:
					glTexSubImage2D( mTarget, 0, 0, 0, mImageSize.x(), mImageSize.y(), mTextureFormat, mTextureType, pData );
					break;

#if !defined( GL_ES_VERSION_2_0 )
				case GL_TEXTURE_CUBE_MAP:
					glTexSubImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X + pCubeFaceIndex, 0, 0, 0, mImageSize.x(), mImageSize.y(), mTextureFormat, mTextureType, pData );
					break;

				case GL_TEXTURE_3D:
					glTexSubImage3D( mTarget, 0, 0, 0, 0, mImageSize.x(), mImageSize.y(), mImageSize.z(), mTextureFormat, mTextureType, pData );
					break;
#endif
			}

			OPENGL_DEBUG( mPin->node()->name() );

			glPixelStorei( GL_UNPACK_ALIGNMENT, 4 );

#if !defined( GL_ES_VERSION_2_0 )
			if( mGenerateMipMaps && GLEW_ARB_framebuffer_object && ( mTarget == GL_TEXTURE_2D || mTarget == GL_TEXTURE_CUBE_MAP ) )
#else
			if( mGenerateMipMaps && ( mTarget == GL_TEXTURE_2D || mTarget == GL_TEXTURE_CUBE_MAP ) )
#endif
			{
				glGenerateMipmap( mTarget );
			}
		}

		release();
	}
}

void TexturePin::setFilter( quint32 pMin, quint32 pMag )
{
	mTextureMinFilter = pMin;
	mTextureMagFilter = pMag;
}

void TexturePin::setWrap( quint32 pX, quint32 pY, quint32 pZ )
{
	mTextureWrapX = pX;
	mTextureWrapY = pY;
	mTextureWrapZ = pZ;
}

void TexturePin::setGenMipMaps( bool pGenMipMaps )
{
	mGenerateMipMaps = pGenMipMaps;
}

void TexturePin::free()
{
	freeFbo();

	if( mDstTexId )
	{
		glDeleteTextures( 1, &mDstTexId );

		mDstTexId = 0;
	}

	if( mSrcTexId )
	{
		glDeleteTextures( 1, &mSrcTexId );

		mSrcTexId = 0;
	}

	OPENGL_PLUGIN_DEBUG
}

QImage TexturePin::image()
{
	QImage		Image( mTextureSize.x(), mTextureSize.y(), QImage::Format_ARGB32 );

	if( !mSrcTexId )
	{
#if !defined( GL_ES_VERSION_2_0 )
		glGetTexImage( mTarget, 0, GL_RGBA, GL_UNSIGNED_BYTE, Image.bits() );
#endif
	}

	OPENGL_PLUGIN_DEBUG

	if( mTextureSize != mImageSize )
	{
		Image = Image.copy( 0, 0, mImageSize.x(), mImageSize.y() );
	}

	return( Image );
}

void TexturePin::srcBind()
{
	glBindTexture( mTarget, srcTexId() );
}

void TexturePin::dstBind()
{
	glBindTexture( mTarget, dstTexId() );
}

void TexturePin::release()
{
	glBindTexture( mTarget, 0 );
}

quint32 TexturePin::fbo( bool pUseDepth )
{
	checkDefinition();

	GLint		FBOCur;

	glGetIntegerv( GL_FRAMEBUFFER_BINDING, &FBOCur );

	if( !mFBOId && mDstTexId && !mTextureSize.isNull() && GLEW_ARB_framebuffer_object )
	{
		glGenFramebuffers( 1, &mFBOId );
		glBindFramebuffer( GL_FRAMEBUFFER, mFBOId );

		if( pUseDepth )
		{
			glGenRenderbuffers( 1, &mFBODepthRBId );
			glBindRenderbuffer( GL_RENDERBUFFER, mFBODepthRBId );
			glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mTextureSize.x(), mTextureSize.y() );

			glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mFBODepthRBId );
		}
	}

	if( mFBOId && mDstTexId && !mTextureSize.isNull() && GLEW_ARB_framebuffer_object )
	{
		glBindFramebuffer( GL_FRAMEBUFFER, mFBOId );

		glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, mTarget, mDstTexId, 0 );

		GLenum	FBOStatus = glCheckFramebufferStatus( GL_FRAMEBUFFER );

		if( FBOStatus != GL_FRAMEBUFFER_COMPLETE )
		{
			qWarning() << "FBO Status" << FBOStatus;

			freeFbo();
		}
	}

	glBindFramebuffer( GL_FRAMEBUFFER, FBOCur );

	return( mFBOId );
}

void TexturePin::freeFbo()
{
	if( mFBOId )
	{
		glDeleteFramebuffers( 1, &mFBOId );

		mFBOId = 0;
	}

	if( mFBODepthRBId )
	{
		glDeleteRenderbuffers( 1, &mFBODepthRBId );

		mFBODepthRBId = 0;
	}

	if( mFBOMSId )
	{
		glDeleteFramebuffers( 1, &mFBOMSId );

		mFBOMSId = 0;
	}

	if( mFBOMSColourRBId )
	{
		glDeleteRenderbuffers( 1, &mFBOMSColourRBId );

		mFBOMSColourRBId = 0;
	}

	if( mFBOMSDepthRBId )
	{
		glDeleteRenderbuffers( 1, &mFBOMSDepthRBId );

		mFBOMSDepthRBId = 0;
	}
}

void TexturePin::checkDefinition()
{
	if( mDefinitionChanged )
	{
		free();

		mDefinitionChanged = false;
	}
}

quint32 TexturePin::fboMultiSample( int pSamples, bool pUseDepth )
{
	checkDefinition();

	if( !mFBOMSId && !mTextureSize.isNull() && GLEW_ARB_framebuffer_object )
	{
		GLint		FBOCur, RBCur;

		glGetIntegerv( GL_FRAMEBUFFER_BINDING, &FBOCur );
		glGetIntegerv( GL_RENDERBUFFER_BINDING, &RBCur );

		glGenFramebuffers( 1, &mFBOMSId );
		glBindFramebuffer( GL_FRAMEBUFFER, mFBOMSId );

		glGenRenderbuffers( 1, &mFBOMSColourRBId );
		glBindRenderbuffer( GL_RENDERBUFFER, mFBOMSColourRBId );

		glRenderbufferStorageMultisample( GL_RENDERBUFFER, pSamples, GL_RGBA, mTextureSize.x(), mTextureSize.y() );
		glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, mFBOMSColourRBId );

		if( pUseDepth )
		{
			glGenRenderbuffers( 1, &mFBOMSDepthRBId );
			glBindRenderbuffer( GL_RENDERBUFFER, mFBOMSDepthRBId );

			glRenderbufferStorageMultisample( GL_RENDERBUFFER, pSamples, GL_DEPTH_COMPONENT24, mTextureSize.x(), mTextureSize.y() );
			glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mFBOMSDepthRBId );
		}

		GLenum	FBOStatus = glCheckFramebufferStatusEXT( GL_FRAMEBUFFER_EXT );

		glBindFramebuffer( GL_FRAMEBUFFER, FBOCur );
		glBindRenderbuffer( GL_RENDERBUFFER, RBCur );

		if( FBOStatus != GL_FRAMEBUFFER_COMPLETE )
		{
			qWarning() << "FBO MS Status" << FBOStatus;

			freeFbo();
		}
	}

	return( mFBOMSId );
}

void TexturePin::swapTexture()
{
	if( mDoubleBuffered )
	{
		std::swap( mSrcTexId, mDstTexId );
	}
}

int TexturePin::sizeDimensions() const
{
	switch( mTarget )
	{
		case GL_TEXTURE_1D:
			return( 1 );
			break;

		case GL_TEXTURE_2D:
		case GL_TEXTURE_RECTANGLE:
		case GL_TEXTURE_CUBE_MAP:
			return( 2 );
			break;

		case GL_TEXTURE_3D:
			return( 3 );
			break;
	}

	return( 0 );
}

float TexturePin::size(int pDimension) const
{
	if( pDimension == 0 ) return( mTextureSize.x() );
	if( pDimension == 1 ) return( mTextureSize.y() );
	if( pDimension == 2 ) return( mTextureSize.z() );

	return( 0 );
}

float TexturePin::sizeWidth() const
{
	return( mTextureSize.x() );
}

float TexturePin::sizeHeight() const
{
	return( mTextureSize.y() );
}

float TexturePin::sizeDepth() const
{
	return( mTextureSize.z() );
}

QSizeF TexturePin::toSizeF() const
{
	return( QSizeF( mTextureSize.x(), mTextureSize.y() ) );
}

QVector3D TexturePin::toVector3D() const
{
	return( mTextureSize );
}
