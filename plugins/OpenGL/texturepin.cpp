#include "texturepin.h"
#include <QSettings>
#include <QImage>

#include <QSet>
#include <QByteArray>

#include <fugio/node_interface.h>

#include "openglplugin.h"

TexturePin::TexturePin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin ), mSrcTex( Q_NULLPTR ), mDstTex( Q_NULLPTR )
{
	memset( &mTexDsc, 0, sizeof( mTexDsc ) );

	mTexDsc.mFormat = GL_RGBA;
	mTexDsc.mInternalFormat = QOpenGLTexture::RGBA;

	mTexDsc.mType = QOpenGLTexture::UInt8;

	mTexDsc.mMinFilter = GL_NEAREST;
	mTexDsc.mMagFilter = GL_NEAREST;

	mTexDsc.mWrapX = QOpenGLTexture::Repeat;
	mTexDsc.mWrapY = GL_REPEAT;
	mTexDsc.mWrapZ = GL_REPEAT;

	mTexDsc.mGenerateMipMaps = false;

	mTexDsc.mDoubleBuffered = false;

	mTexDsc.mCompare = GL_NONE;

	mDefinitionChanged = true;

	mFBOId = 0;
	mFBODepthRBId = 0;
	mFBOBoundTexId = 0;

	mFBOMSId = 0;
	mFBOMSColourRBId = 0;
	mFBOMSDepthRBId = 0;
}

TexturePin::~TexturePin( void )
{
	free();
}

QVector3D TexturePin::textureSize() const
{
	return( QVector3D( mTexDsc.mTexWidth, mTexDsc.mTexHeight, mTexDsc.mTexDepth ) );
}

QVector3D TexturePin::size() const
{
	return( QVector3D( mTexDsc.mImgWidth, mTexDsc.mImgHeight, mTexDsc.mImgDepth ) );
}

quint32 TexturePin::srcTexId() const
{
	return( mTexDsc.mDoubleBuffered ? mSrcTex->textureId() : mDstTex->textureId() );
}

quint32 TexturePin::dstTexId() const
{
	return( mDstTex->textureId() );
}

quint32 TexturePin::target() const
{
	return( mTexDsc.mTarget );
}

quint32 TexturePin::format() const
{
	return( mTexDsc.mFormat );
}

quint32 TexturePin::internalFormat() const
{
	return( mTexDsc.mInternalFormat );
}

quint32 TexturePin::type() const
{
	return( mTexDsc.mType );
}

int TexturePin::filterMin( void ) const
{
	return( mTexDsc.mMinFilter );
}

int TexturePin::filterMag( void ) const
{
	return( mTexDsc.mMagFilter );
}

int TexturePin::wrapS( void ) const
{
	return( mTexDsc.mWrapX );
}

int TexturePin::wrapT( void ) const
{
	return( mTexDsc.mWrapY );
}

int TexturePin::wrapR() const
{
	return( mTexDsc.mWrapZ );
}

bool TexturePin::genMipMaps( void ) const
{
	return( mTexDsc.mGenerateMipMaps );
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
	if( pSize != size() )
	{
		mTexDsc.mImgWidth  = pSize.x();
		mTexDsc.mImgHeight = pSize.y();
		mTexDsc.mImgDepth  = pSize.z();

		if( !hasOpenGLFeature( QOpenGLFunctions::NPOTTextures ) )
		{
			mTexDsc.mTexWidth  = roundUpToNextPowerOfTwo( mTexDsc.mImgWidth );
			mTexDsc.mTexHeight = roundUpToNextPowerOfTwo( mTexDsc.mImgHeight );
			mTexDsc.mTexDepth  = roundUpToNextPowerOfTwo( mTexDsc.mImgDepth );

			qDebug() << "NPOT" << size() << "->" << textureSize();
		}
		else
		{
			mTexDsc.mTexWidth  = mTexDsc.mImgWidth;
			mTexDsc.mTexHeight = mTexDsc.mImgHeight;
			mTexDsc.mTexDepth  = mTexDsc.mImgDepth;
		}

		mDefinitionChanged = true;
	}
}

void TexturePin::setTarget( quint32 pTarget )
{
	if( pTarget != mTexDsc.mTarget )
	{
		mTexDsc.mTarget = pTarget;

		mDefinitionChanged = true;
	}
}

void TexturePin::setFormat( quint32 pFormat )
{
	if( pFormat != mTexDsc.mFormat )
	{
		mTexDsc.mFormat = pFormat;

		mDefinitionChanged = true;
	}
}

void TexturePin::setType( QOpenGLTexture::PixelType pType )
{
	if( mTexDsc.mType != pType )
	{
		mTexDsc.mType = pType;

		mDefinitionChanged = true;
	}
}

void TexturePin::setInternalFormat( quint32 pInternalFormat )
{
	if( mTexDsc.mInternalFormat != pInternalFormat )
	{
		mTexDsc.mInternalFormat = pInternalFormat;

		mDefinitionChanged = true;
	}
}

bool TexturePin::isCompressedFormat( GLenum pFormat )
{
	switch( pFormat )
	{
		case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
		case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
		case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
		case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
			return( true );
	}

	return( false );
}

void TexturePin::update()
{
	if( !OpenGLPlugin::hasContextStatic() )
	{
		return;
	}

	initializeOpenGLFunctions();

	OPENGL_PLUGIN_DEBUG

	checkDefinition();

	if( !mTexDsc.mImgWidth )
	{
		return;
	}

	if( !mDstTex )
	{
		mDstTex = new QOpenGLTexture( QOpenGLTexture::Target( mTexDsc.mTarget ) );

		if( !mDstTex )
		{
			return;
		}

		mDstTex->setSize( mTexDsc.mTexWidth, mTexDsc.mTexHeight, mTexDsc.mTexDepth );

		if( !isCompressedFormat( mTexDsc.mInternalFormat ) )
		{
			mDstTex->allocateStorage( mTexDsc.mInternalFormat, mTexDsc.mType );
		}

		if( !isCompressedFormat( mTexDsc.mInternalFormat ) )
		{
			mDstTex->setMinMagFilters( ( mTexDsc.mMinFilter ), QOpenGLTexture::Filter( mTexDsc.mMagFilter ) );
		}
		else
		{
			mDstTex->setMinMagFilters( QOpenGLTexture::Linear, QOpenGLTexture::Linear );
		}

		mDstTex->setWrapMode( QOpenGLTexture::DirectionS, mTexDsc.mWrapX );
		mDstTex->setWrapMode( QOpenGLTexture::DirectionT, mTexDsc.mWrapY );
		mDstTex->setWrapMode( QOpenGLTexture::DirectionR, mTexDsc.mWrapZ );

		if( mTexDsc.mFormat == GL_DEPTH_COMPONENT )
		{
			if( mTexDsc.mCompare == GL_NONE )
			{
				mDstTex->setComparisonMode( QOpenGLTexture::CompareNone );
			}
			else
			{
				mDstTex->setComparisonMode( QOpenGLTexture::CompareRefToTexture );
				mDstTex->setComparisonFunction( mTexDsc.mCompare );
			}
		}

		OPENGL_DEBUG( mPin->node()->name() );

		if( !mDstTex->isStorageAllocated() && !isCompressedFormat( mTexDsc.mInternalFormat ) )
		{
			qDebug() << "Couldn't allocate texture";

			return;
		}
	}
}

void TexturePin::update( const unsigned char *pData, int pDataSize, int pLineSize, int pCubeFaceIndex )
{
	Q_UNUSED( pLineSize )

	if( !OpenGLPlugin::hasContextStatic() )
	{
		return;
	}

	OPENGL_PLUGIN_DEBUG

	update();

	if( !mTexDsc.mImgWidth || !mDstTex->isCreated() )
	{
		return;
	}

	dstBind();

	if( isCompressedFormat( mTexDsc.mInternalFormat ) )
	{
			switch( mTexDsc.mTarget )
			{
				case GL_TEXTURE_1D:
					glCompressedTexImage1D( mTexDsc.mTarget, 0, mTexDsc.mInternalFormat, mTexDsc.mImgWidth, 0, pDataSize, pData );
					break;

				case GL_TEXTURE_2D:
				case GL_TEXTURE_RECTANGLE:
					glCompressedTexImage2DARB( mTexDsc.mTarget, 0, mTexDsc.mInternalFormat, mTexDsc.mImgWidth, mTexDsc.mImgHeight, 0, pDataSize, pData );
					break;

				case GL_TEXTURE_CUBE_MAP:
					glCompressedTexImage2DARB( GL_TEXTURE_CUBE_MAP_POSITIVE_X + pCubeFaceIndex, 0, mTexDsc.mInternalFormat, mTexDsc.mImgWidth, mTexDsc.mImgHeight, 0, pDataSize, pData );
					break;

				case GL_TEXTURE_3D:
					glCompressedTexImage3D( mTexDsc.mTarget, 0, mTexDsc.mInternalFormat, mTexDsc.mImgWidth, mTexDsc.mImgHeight, mTexDsc.mImgDepth, 0, pDataSize, pData );
					break;
			}
	}
	else
	{
		glPixelStorei( GL_UNPACK_ALIGNMENT, 4 );

		OPENGL_DEBUG( mPin->node()->name() );

		switch( mTexDsc.mTarget )
		{
			case GL_TEXTURE_1D:
				glTexSubImage1D( mTexDsc.mTarget, 0, 0, mTexDsc.mImgWidth, mTexDsc.mFormat, mTexDsc.mType, pData );
				break;

			case GL_TEXTURE_2D:
			case GL_TEXTURE_RECTANGLE:
				glTexSubImage2D( mTexDsc.mTarget, 0, 0, 0, mTexDsc.mImgWidth, mTexDsc.mImgHeight, mTexDsc.mFormat, mTexDsc.mType, pData );
				break;

			case GL_TEXTURE_CUBE_MAP:
				glTexSubImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X + pCubeFaceIndex, 0, 0, 0, mTexDsc.mImgWidth, mTexDsc.mImgHeight, mTexDsc.mFormat, mTexDsc.mType, pData );
				break;

			case GL_TEXTURE_3D:
				glTexSubImage3D( mTexDsc.mTarget, 0, 0, 0, 0, mTexDsc.mImgWidth, mTexDsc.mImgHeight, mTexDsc.mImgDepth, mTexDsc.mFormat, mTexDsc.mType, pData );
				break;
		}

		OPENGL_DEBUG( mPin->node()->name() );

		glPixelStorei( GL_UNPACK_ALIGNMENT, 4 );

#if !defined( GL_ES_VERSION_2_0 )
		if( mTexDsc.mGenerateMipMaps && GLEW_ARB_framebuffer_object && ( mTexDsc.mTarget == GL_TEXTURE_2D || mTexDsc.mTarget == GL_TEXTURE_CUBE_MAP ) )
#else
		if( mTexDsc.mGenerateMipMaps && ( mTexDsc.mTarget == GL_TEXTURE_2D || mTexDsc.mTarget == GL_TEXTURE_CUBE_MAP ) )
#endif
		{
			glGenerateMipmap( mTexDsc.mTarget );
		}
	}

	release();
}

void TexturePin::setFilter( quint32 pMin, quint32 pMag )
{
	mTexDsc.mMinFilter = pMin;
	mTexDsc.mMagFilter = pMag;
}

void TexturePin::setWrap( quint32 pX, quint32 pY, quint32 pZ )
{
	mTexDsc.mWrapX = pX;
	mTexDsc.mWrapY = pY;
	mTexDsc.mWrapZ = pZ;
}

void TexturePin::setGenMipMaps( bool pGenMipMaps )
{
	mTexDsc.mGenerateMipMaps = pGenMipMaps;
}

void TexturePin::free()
{
	freeFbo();

	if( mDstTex )
	{
		delete mDstTex;

		mDstTex = Q_NULLPTR;
	}

	if( mSrcTex )
	{
		delete mSrcTex;

		mSrcTex = Q_NULLPTR;
	}

	OPENGL_PLUGIN_DEBUG
}

QImage TexturePin::image()
{
	QImage		Image( mTexDsc.mTexWidth, mTexDsc.mTexHeight, QImage::Format_ARGB32 );

	if( !mSrcTex || !mSrcTex->isCreated() )
	{
		glGetTexImage( mTexDsc.mTarget, 0, GL_RGBA, GL_UNSIGNED_BYTE, Image.bits() );
	}

	OPENGL_PLUGIN_DEBUG

	if( textureSize() != size() )
	{
		Image = Image.copy( 0, 0, mTexDsc.mImgWidth, mTexDsc.mImgHeight );
	}

	return( Image );
}

void TexturePin::srcBind()
{
	mSrcTex->bind();
}

void TexturePin::dstBind()
{
	mDstTex->bind();
}

void TexturePin::release()
{
	glBindTexture( mTexDsc.mTarget, 0 );
}

quint32 TexturePin::fbo( bool pUseDepth )
{
	checkDefinition();

	GLint		FBOCur;

	glGetIntegerv( GL_FRAMEBUFFER_BINDING, &FBOCur );

#if defined( GL_ES_VERSION_2_0 )
	if( !mFBOId && mDstTexId && mTexDsc.mTexWidth )
#else
	if( !mFBOId && mDstTexId && mTexDsc.mTexWidth && GLEW_ARB_framebuffer_object )
#endif
	{
		glGenFramebuffers( 1, &mFBOId );
		glBindFramebuffer( GL_FRAMEBUFFER, mFBOId );

		if( pUseDepth )
		{
			glGenRenderbuffers( 1, &mFBODepthRBId );
			glBindRenderbuffer( GL_RENDERBUFFER, mFBODepthRBId );

#if defined( GL_ES_VERSION_2_0 )
			glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, mTexDsc.mTexWidth, mTexDsc.mTexHeight );
#else
			glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mTexDsc.mTexWidth, mTexDsc.mTexHeight );
#endif

			glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mFBODepthRBId );
		}

		glBindFramebuffer( GL_FRAMEBUFFER, FBOCur );
	}

#if defined( GL_ES_VERSION_2_0 )
	if( mFBOId && mDstTexId && mTexDsc.mTexWidth )
#else
	if( mFBOId && mDstTexId && mDstTexId != mFBOBoundTexId && mTexDsc.mTexWidth && GLEW_ARB_framebuffer_object )
#endif
	{
		glBindFramebuffer( GL_FRAMEBUFFER, mFBOId );

		glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, mTexDsc.mTarget, mDstTexId, 0 );

		GLenum	FBOStatus = glCheckFramebufferStatus( GL_FRAMEBUFFER );

		if( FBOStatus != GL_FRAMEBUFFER_COMPLETE )
		{
			qWarning() << "FBO Status" << FBOStatus;

			freeFbo();
		}
		else
		{
			mFBOBoundTexId = mDstTexId;
		}

		glBindFramebuffer( GL_FRAMEBUFFER, FBOCur );
	}

	return( mFBOId );
}

void TexturePin::freeFbo()
{
	mFBOBoundTexId = 0;

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

	GLint		FBOCur;

	glGetIntegerv( GL_FRAMEBUFFER_BINDING, &FBOCur );

#if !defined( GL_ES_VERSION_2_0 )
	if( !mFBOMSId && mTexDsc.mTexWidth && GLEW_ARB_framebuffer_object )
	{
		GLint		FBOCur, RBCur;

		glGetIntegerv( GL_FRAMEBUFFER_BINDING, &FBOCur );
		glGetIntegerv( GL_RENDERBUFFER_BINDING, &RBCur );

		glGenFramebuffers( 1, &mFBOMSId );
		glBindFramebuffer( GL_FRAMEBUFFER, mFBOMSId );

		glGenRenderbuffers( 1, &mFBOMSColourRBId );
		glBindRenderbuffer( GL_RENDERBUFFER, mFBOMSColourRBId );

		glRenderbufferStorageMultisample( GL_RENDERBUFFER, pSamples, GL_RGBA, mTexDsc.mTexWidth, mTexDsc.mTexHeight );
		glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, mFBOMSColourRBId );

		if( pUseDepth )
		{
			glGenRenderbuffers( 1, &mFBOMSDepthRBId );
			glBindRenderbuffer( GL_RENDERBUFFER, mFBOMSDepthRBId );

			glRenderbufferStorageMultisample( GL_RENDERBUFFER, pSamples, GL_DEPTH_COMPONENT24, mTexDsc.mTexWidth, mTexDsc.mTexHeight );
			glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mFBOMSDepthRBId );
		}

		GLenum	FBOStatus = glCheckFramebufferStatus( GL_FRAMEBUFFER_EXT );

		glBindFramebuffer( GL_FRAMEBUFFER, FBOCur );
		glBindRenderbuffer( GL_RENDERBUFFER, RBCur );

		if( FBOStatus != GL_FRAMEBUFFER_COMPLETE )
		{
			qWarning() << "FBO MS Status" << FBOStatus;

			freeFbo();
		}

		glBindFramebuffer( GL_FRAMEBUFFER, FBOCur );
	}
#endif

	return( mFBOMSId );
}

void TexturePin::swapTexture()
{
	if( mTexDsc.mDoubleBuffered )
	{
		std::swap( mSrcTex, mDstTex );
	}
}

int TexturePin::sizeDimensions() const
{
	switch( mTexDsc.mTarget )
	{
#if !defined( GL_ES_VERSION_2_0 )
		case GL_TEXTURE_1D:
			return( 1 );
			break;

	case GL_TEXTURE_3D:
		return( 3 );
		break;
#endif

		case GL_TEXTURE_2D:
		case GL_TEXTURE_RECTANGLE:
		case GL_TEXTURE_CUBE_MAP:
			return( 2 );
			break;
	}

	return( 0 );
}

float TexturePin::size(int pDimension) const
{
	if( pDimension == 0 ) return( mTexDsc.mTexWidth );
	if( pDimension == 1 ) return( mTexDsc.mTexHeight );
	if( pDimension == 2 ) return( mTexDsc.mTexDepth );

	return( 0 );
}

float TexturePin::sizeWidth() const
{
	return( mTexDsc.mTexWidth );
}

float TexturePin::sizeHeight() const
{
	return( mTexDsc.mTexHeight );
}

float TexturePin::sizeDepth() const
{
	return( mTexDsc.mTexDepth );
}

QSizeF TexturePin::toSizeF() const
{
	return( QSizeF( mTexDsc.mTexWidth, mTexDsc.mTexHeight ) );
}

QVector3D TexturePin::toVector3D() const
{
	return( textureSize() );
}

qint32 TexturePin::compare() const
{
	return( mTexDsc.mCompare );
}

void TexturePin::setCompare( qint32 pCompare )
{
	if( mTexDsc.mCompare != pCompare )
	{
		mTexDsc.mCompare = pCompare;

		mDefinitionChanged = true;
	}
}
