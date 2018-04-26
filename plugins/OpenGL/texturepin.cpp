#include "texturepin.h"
#include <QSettings>
#include <QImage>

#include <QSet>
#include <QByteArray>

#include <QOpenGLFunctions_2_0>

#if QT_VERSION >= QT_VERSION_CHECK( 5, 6, 0 )
#include <QOpenGLExtraFunctions>
#endif

#include <fugio/node_interface.h>

#include "openglplugin.h"

TexturePin::TexturePin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin ), mSrcTex( Q_NULLPTR ), mDstTex( Q_NULLPTR )
{
	memset( &mTexDsc, 0, sizeof( mTexDsc ) );

	mTexDsc.mFormat         = QOpenGLTexture::RGBA;
	mTexDsc.mInternalFormat = QOpenGLTexture::RGBA8_UNorm;

	mTexDsc.mType = QOpenGLTexture::UInt8;

	mTexDsc.mMinFilter = QOpenGLTexture::Nearest;
	mTexDsc.mMagFilter = QOpenGLTexture::Nearest;

	mTexDsc.mWrapX = QOpenGLTexture::Repeat;
	mTexDsc.mWrapY = QOpenGLTexture::Repeat;
	mTexDsc.mWrapZ = QOpenGLTexture::Repeat;

	mTexDsc.mGenerateMipMaps = false;

	mTexDsc.mDoubleBuffered = false;

	mTexDsc.mCompare = QOpenGLTexture::CompareNever;

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
	return( mTexDsc.mDoubleBuffered ? ( mSrcTex ? mSrcTex->textureId() : 0 ) : ( mDstTex ? mDstTex->textureId() : 0 ) );
}

quint32 TexturePin::dstTexId() const
{
	return( mDstTex ? mDstTex->textureId() : 0 );
}

QOpenGLTexture::Target TexturePin::target() const
{
	return( mTexDsc.mTarget );
}

QOpenGLTexture::PixelFormat TexturePin::format() const
{
	return( mTexDsc.mFormat );
}

QOpenGLTexture::TextureFormat TexturePin::internalFormat() const
{
	return( mTexDsc.mInternalFormat );
}

QOpenGLTexture::PixelType TexturePin::type() const
{
	return( mTexDsc.mType );
}

QOpenGLTexture::Filter TexturePin::filterMin( void ) const
{
	return( mTexDsc.mMinFilter );
}

QOpenGLTexture::Filter TexturePin::filterMag( void ) const
{
	return( mTexDsc.mMagFilter );
}

QOpenGLTexture::WrapMode TexturePin::wrapS( void ) const
{
	return( mTexDsc.mWrapX );
}

QOpenGLTexture::WrapMode TexturePin::wrapT( void ) const
{
	return( mTexDsc.mWrapY );
}

QOpenGLTexture::WrapMode TexturePin::wrapR() const
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
	initializeOpenGLFunctions();

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

void TexturePin::setTarget( QOpenGLTexture::Target pTarget )
{
	if( pTarget != mTexDsc.mTarget )
	{
		mTexDsc.mTarget = pTarget;

		mDefinitionChanged = true;
	}
}

void TexturePin::setFormat( QOpenGLTexture::PixelFormat pFormat )
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

void TexturePin::setInternalFormat( QOpenGLTexture::TextureFormat pInternalFormat )
{
	if( mTexDsc.mInternalFormat != pInternalFormat )
	{
		mTexDsc.mInternalFormat = pInternalFormat;

		mDefinitionChanged = true;
	}
}

bool TexturePin::isCompressedFormat( QOpenGLTexture::TextureFormat pFormat )
{
	switch( pFormat )
	{
		case QOpenGLTexture::RGB_DXT1:
		case QOpenGLTexture::RGBA_DXT1:
		case QOpenGLTexture::RGBA_DXT3:
		case QOpenGLTexture::RGBA_DXT5:

		case QOpenGLTexture::R_ATI1N_UNorm:
		case QOpenGLTexture::R_ATI1N_SNorm:
		case QOpenGLTexture::RG_ATI2N_UNorm:
		case QOpenGLTexture::RG_ATI2N_SNorm:
		case QOpenGLTexture::RGB_BP_UNSIGNED_FLOAT:
		case QOpenGLTexture::RGB_BP_SIGNED_FLOAT:
		case QOpenGLTexture::RGB_BP_UNorm:
		case QOpenGLTexture::R11_EAC_UNorm:
		case QOpenGLTexture::R11_EAC_SNorm:
			return( true );

		default:
			break;
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

	if( mDstTex )
	{
		return;
	}

	if( !( mDstTex = new QOpenGLTexture( QOpenGLTexture::Target( mTexDsc.mTarget ) ) ) )
	{
		return;
	}

	if( mDstTex->target() == QOpenGLTexture::TargetBuffer )
	{
		return;
	}

	if( !mTexDsc.mImgWidth )
	{
		return;
	}

	switch( sizeDimensions() )
	{
		case 1:
			mDstTex->setSize( mTexDsc.mTexWidth );
			break;

		case 2:
			mDstTex->setSize( mTexDsc.mTexWidth, mTexDsc.mTexHeight );
			break;

		case 3:
			mDstTex->setSize( mTexDsc.mTexWidth, mTexDsc.mTexHeight, mTexDsc.mTexDepth );
			break;
	}

	mDstTex->setFormat( mTexDsc.mInternalFormat );

	if( !isCompressedFormat( mTexDsc.mInternalFormat ) )
	{
		mDstTex->setAutoMipMapGenerationEnabled( mTexDsc.mGenerateMipMaps );

		mDstTex->allocateStorage( mTexDsc.mFormat, mTexDsc.mType );

		if( !mDstTex->isStorageAllocated() )
		{
			qDebug() << "Couldn't allocate texture";

			return;
		}

		mDstTex->setMinMagFilters( mTexDsc.mMinFilter, mTexDsc.mMagFilter );
	}
	else
	{
		mDstTex->setMinMagFilters( QOpenGLTexture::Linear, QOpenGLTexture::Linear );
	}

	switch( sizeDimensions() )
	{
		case 3:
			mDstTex->setWrapMode( QOpenGLTexture::DirectionR, mTexDsc.mWrapZ );
		case 2:
			mDstTex->setWrapMode( QOpenGLTexture::DirectionT, mTexDsc.mWrapY );
		case 1:
			mDstTex->setWrapMode( QOpenGLTexture::DirectionS, mTexDsc.mWrapX );
	}

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
	}
}

void TexturePin::update( const unsigned char *pData, int pDataSize, int pLineSize, int pCubeFaceIndex )
{
	Q_UNUSED( pLineSize )

	if( !OpenGLPlugin::hasContextStatic() )
	{
		return;
	}

	initializeOpenGLFunctions();

#if defined( QOPENGLEXTRAFUNCTIONS_H )
	QOpenGLExtraFunctions	*GLEX = QOpenGLContext::currentContext()->extraFunctions();
#endif

	OPENGL_PLUGIN_DEBUG

	update();

	if( !mTexDsc.mImgWidth || !mDstTex->isCreated() )
	{
		return;
	}

	if( mDstTex->target() == QOpenGLTexture::TargetBuffer )
	{
		return;
	}

//	dstBind();

	if( isCompressedFormat( mTexDsc.mInternalFormat ) )
	{
		mDstTex->setCompressedData( pDataSize, pData );

//			switch( mTexDsc.mTarget )
//			{
////				case GL_TEXTURE_1D:
////					if( GLEX )
////					{
////						GL20->glCompressedTexImage1D( mTexDsc.mTarget, 0, mTexDsc.mInternalFormat, mTexDsc.mImgWidth, 0, pDataSize, pData );
////					}
////					break;

//				case GL_TEXTURE_2D:
//				case QOpenGLTexture::TargetRectangle:
//					glCompressedTexImage2D( mTexDsc.mTarget, 0, mTexDsc.mInternalFormat, mTexDsc.mImgWidth, mTexDsc.mImgHeight, 0, pDataSize, pData );
//					break;

//				case GL_TEXTURE_CUBE_MAP:
//					glCompressedTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X + pCubeFaceIndex, 0, mTexDsc.mInternalFormat, mTexDsc.mImgWidth, mTexDsc.mImgHeight, 0, pDataSize, pData );
//					break;

//				case GL_TEXTURE_3D:
//					if( GLEX )
//					{
//						GLEX->glCompressedTexImage3D( mTexDsc.mTarget, 0, mTexDsc.mInternalFormat, mTexDsc.mImgWidth, mTexDsc.mImgHeight, mTexDsc.mImgDepth, 0, pDataSize, pData );
//					}
//					break;
//			}
	}
	else
	{
		mDstTex->setData( mTexDsc.mFormat, mTexDsc.mType, pData );

//		glPixelStorei( GL_UNPACK_ALIGNMENT, 4 );

//		OPENGL_DEBUG( mPin->node()->name() );

//		switch( mTexDsc.mTarget )
//		{
////			case GL_TEXTURE_1D:
////				if( GL20 )
////				{
////					glTexSubImage1D( mTexDsc.mTarget, 0, 0, mTexDsc.mImgWidth, mTexDsc.mFormat, mTexDsc.mType, pData );
////				}
////				break;

//			case GL_TEXTURE_2D:
//			case QOpenGLTexture::TargetRectangle:
//				glTexSubImage2D( mTexDsc.mTarget, 0, 0, 0, mTexDsc.mImgWidth, mTexDsc.mImgHeight, mTexDsc.mFormat, mTexDsc.mType, pData );
//				break;

//			case GL_TEXTURE_CUBE_MAP:
//				glTexSubImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X + pCubeFaceIndex, 0, 0, 0, mTexDsc.mImgWidth, mTexDsc.mImgHeight, mTexDsc.mFormat, mTexDsc.mType, pData );
//				break;

//			case GL_TEXTURE_3D:
//				if( GLEX )
//				{
//					GLEX->glTexSubImage3D( mTexDsc.mTarget, 0, 0, 0, 0, mTexDsc.mImgWidth, mTexDsc.mImgHeight, mTexDsc.mImgDepth, mTexDsc.mFormat, mTexDsc.mType, pData );
//				}
//				break;
//		}

//		OPENGL_DEBUG( mPin->node()->name() );

//		glPixelStorei( GL_UNPACK_ALIGNMENT, 4 );

//		if( mTexDsc.mGenerateMipMaps && ( mTexDsc.mTarget == GL_TEXTURE_2D || mTexDsc.mTarget == GL_TEXTURE_CUBE_MAP ) )
//		{
//			glGenerateMipmap( mTexDsc.mTarget );
//		}
	}

//	release();

	if( isDoubleBuffered() )
	{
		if( dstTexId() && !srcTexId() )
		{
			swapTexture();

			update();
		}
	}
}

void TexturePin::setFilter( QOpenGLTexture::Filter pMin, QOpenGLTexture::Filter pMag )
{
	mTexDsc.mMinFilter = pMin;
	mTexDsc.mMagFilter = pMag;
}

void TexturePin::setWrap( QOpenGLTexture::WrapMode pX, QOpenGLTexture::WrapMode pY, QOpenGLTexture::WrapMode pZ )
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
#if !defined( QT_OPENGL_ES_2 )
		QOpenGLFunctions_2_0	*GL20 = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_2_0>();

		if( GL20 && !GL20->initializeOpenGLFunctions() )
		{
			GL20 = Q_NULLPTR;
		}

		if( GL20 )
		{
			GL20->glGetTexImage( mTexDsc.mTarget, 0, GL_RGBA, GL_UNSIGNED_BYTE, Image.bits() );
		}
#endif
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
	if( mSrcTex )
	{
		if( mSrcTex->isCreated() )
		{
			mSrcTex->bind();
		}
	}
	else if( mDstTex )
	{
		if( mDstTex->isCreated() )
		{
			mDstTex->bind();
		}
	}
}

void TexturePin::dstBind()
{
	if( mDstTex )
	{
		mDstTex->bind();
	}
}

void TexturePin::release()
{
	initializeOpenGLFunctions();

	glBindTexture( mTexDsc.mTarget, 0 );
}

quint32 TexturePin::fbo( bool pUseDepth )
{
	checkDefinition();

	GLint		FBOCur;

	glGetIntegerv( GL_FRAMEBUFFER_BINDING, &FBOCur );

	if( !mFBOId && mDstTex && mTexDsc.mTexWidth )
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

	if( mFBOId && mDstTex && mDstTex->textureId() != mFBOBoundTexId && mTexDsc.mTexWidth )
	{
		glBindFramebuffer( GL_FRAMEBUFFER, mFBOId );

		glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, mTexDsc.mTarget, mDstTex->textureId(), 0 );

		GLenum	FBOStatus = glCheckFramebufferStatus( GL_FRAMEBUFFER );

		if( FBOStatus != GL_FRAMEBUFFER_COMPLETE )
		{
			qWarning() << "FBO Status" << FBOStatus;

			freeFbo();
		}
		else
		{
			mFBOBoundTexId = mDstTex->textureId();
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

#if defined( QOPENGLEXTRAFUNCTIONS_H )
	QOpenGLExtraFunctions	*GLEX = QOpenGLContext::currentContext()->extraFunctions();
#endif

	GLint		FBOCur;

	glGetIntegerv( GL_FRAMEBUFFER_BINDING, &FBOCur );

	if( !mFBOMSId && mTexDsc.mTexWidth )
	{
		GLint		FBOCur, RBCur;

		glGetIntegerv( GL_FRAMEBUFFER_BINDING, &FBOCur );
		glGetIntegerv( GL_RENDERBUFFER_BINDING, &RBCur );

		glGenFramebuffers( 1, &mFBOMSId );
		glBindFramebuffer( GL_FRAMEBUFFER, mFBOMSId );

		glGenRenderbuffers( 1, &mFBOMSColourRBId );
		glBindRenderbuffer( GL_RENDERBUFFER, mFBOMSColourRBId );

#if defined( QOPENGLEXTRAFUNCTIONS_H )
		if( GLEX )
		{
			GLEX->glRenderbufferStorageMultisample( GL_RENDERBUFFER, pSamples, GL_RGBA, mTexDsc.mTexWidth, mTexDsc.mTexHeight );
		}
#endif

		glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, mFBOMSColourRBId );

		if( pUseDepth )
		{
			glGenRenderbuffers( 1, &mFBOMSDepthRBId );
			glBindRenderbuffer( GL_RENDERBUFFER, mFBOMSDepthRBId );

#if defined( QOPENGLEXTRAFUNCTIONS_H )
			if( GLEX )
			{
				GLEX->glRenderbufferStorageMultisample( GL_RENDERBUFFER, pSamples, GL_DEPTH_COMPONENT24, mTexDsc.mTexWidth, mTexDsc.mTexHeight );
			}
#endif

			glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mFBOMSDepthRBId );
		}

		GLenum	FBOStatus = glCheckFramebufferStatus( GL_FRAMEBUFFER );

		glBindFramebuffer( GL_FRAMEBUFFER, FBOCur );
		glBindRenderbuffer( GL_RENDERBUFFER, RBCur );

		if( FBOStatus != GL_FRAMEBUFFER_COMPLETE )
		{
			qWarning() << "FBO MS Status" << FBOStatus;

			freeFbo();
		}

		glBindFramebuffer( GL_FRAMEBUFFER, FBOCur );
	}

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
		case QOpenGLTexture::Target1D:
		case QOpenGLTexture::TargetBuffer:
			return( 1 );
			break;

		case QOpenGLTexture::Target3D:
			return( 3 );
			break;

		case QOpenGLTexture::Target2D:
		case QOpenGLTexture::TargetRectangle:
		case QOpenGLTexture::TargetCubeMap:
			return( 2 );
			break;

		default:
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

QOpenGLTexture::ComparisonFunction TexturePin::compare() const
{
	return( mTexDsc.mCompare );
}

void TexturePin::setCompare( QOpenGLTexture::ComparisonFunction pCompare )
{
	if( mTexDsc.mCompare != pCompare )
	{
		mTexDsc.mCompare = pCompare;

		mDefinitionChanged = true;
	}
}
