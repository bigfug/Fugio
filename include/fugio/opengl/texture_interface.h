#ifndef TEXTUREINTERFACE__H
#define TEXTUREINTERFACE__H

#include <QSize>
#include <QUuid>
#include <QVector>
#include <QVector2D>
#include <QVector3D>
#include <QOpenGLTexture>

#include <fugio/global.h>

class QImage;

FUGIO_NAMESPACE_BEGIN

typedef enum OpenGLTextureOrigin
{
	Unknown,
	TopLeft,
	BottomLeft
} OpenGLTextureOrigin;

typedef struct OpenGLTextureDescription
{
	QOpenGLTexture::PixelFormat			mFormat;
	QOpenGLTexture::TextureFormat		mInternalFormat;

	GLsizei			mTexWidth;
	GLsizei			mTexHeight;
	GLsizei			mTexDepth;

	GLsizei			mImgWidth;
	GLsizei			mImgHeight;
	GLsizei			mImgDepth;

	QOpenGLTexture::Target				mTarget;
	QOpenGLTexture::PixelType			mType;

	QOpenGLTexture::Filter				mMinFilter;
	QOpenGLTexture::Filter				mMagFilter;

	QOpenGLTexture::WrapMode			mWrapX;
	QOpenGLTexture::WrapMode			mWrapY;
	QOpenGLTexture::WrapMode			mWrapZ;

	bool			mGenerateMipMaps;

	bool			mDoubleBuffered;

	QOpenGLTexture::ComparisonFunction	mCompare;

	OpenGLTextureOrigin					mOrigin;
} OpenGLTextureDescription;

class OpenGLTextureInterface
{
public:
	virtual ~OpenGLTextureInterface( void ) {}

	virtual QVector3D textureSize( void ) const = 0;

	virtual QVector3D size( void ) const = 0;

	virtual void setDoubleBuffered( bool pDoubleBuffered ) = 0;

	virtual bool isDoubleBuffered( void ) const = 0;

	virtual bool isDepthTexture( void ) const = 0;

	virtual void swapTexture( void ) = 0;

	virtual quint32 srcTexId( void ) const = 0;

	virtual quint32 dstTexId( void ) const = 0;

	virtual QOpenGLTexture::Target target( void ) const = 0;

	virtual QOpenGLTexture::PixelFormat format( void ) const = 0;

	virtual QOpenGLTexture::TextureFormat internalFormat( void ) const = 0;

	virtual QOpenGLTexture::PixelType type( void ) const = 0;

	virtual QOpenGLTexture::ComparisonFunction compare( void ) const = 0;

	virtual QOpenGLTexture::Filter filterMin( void ) const = 0;
	virtual QOpenGLTexture::Filter filterMag( void ) const = 0;
	virtual QOpenGLTexture::WrapMode wrapS( void ) const = 0;
	virtual QOpenGLTexture::WrapMode wrapT( void ) const = 0;
	virtual QOpenGLTexture::WrapMode wrapR( void ) const = 0;

	virtual bool genMipMaps( void ) const = 0;

	virtual void setCompare( QOpenGLTexture::ComparisonFunction pCompare ) = 0;

	virtual void setTarget( QOpenGLTexture::Target pTarget ) = 0;

	virtual void setSize( qint32 pWidth, qint32 pHeight = 0, qint32 pDepth = 0 ) = 0;

	virtual void setSize( const QVector3D &pSize ) = 0;

	virtual void setFormat( QOpenGLTexture::PixelFormat pFormat ) = 0;

	virtual void setType( QOpenGLTexture::PixelType pType ) = 0;

	virtual void setInternalFormat( QOpenGLTexture::TextureFormat pInternalFormat ) = 0;

	virtual void update( void ) = 0;

	virtual void update( const unsigned char *pData, int pDataSize, int pLineSize, int pCubeFaceIndex = 0 ) = 0;

	virtual void setFilter( QOpenGLTexture::Filter pMin, QOpenGLTexture::Filter pMag ) = 0;

	virtual void setWrap( QOpenGLTexture::WrapMode pX, QOpenGLTexture::WrapMode pY, QOpenGLTexture::WrapMode pZ ) = 0;

	virtual void setGenMipMaps( bool pGenMipMaps ) = 0;

	virtual quint32 fbo( bool pUseDepth = false ) = 0;

	virtual void freeFbo( void ) = 0;

	virtual quint32 fboMultiSample( int pSamples, bool pUseDepth = false ) = 0;

	virtual void free( void ) = 0;

	virtual QImage image( void ) = 0;

	virtual void srcBind( void ) = 0;
	virtual void dstBind( void ) = 0;

	virtual void release( void ) = 0;

	virtual OpenGLTextureDescription textureDescription( void ) const = 0;

	virtual void setTextureDescription( const OpenGLTextureDescription &pDescription ) = 0;

	virtual QOpenGLTexture *srcTex( void ) = 0;
	virtual QOpenGLTexture *dstTex( void ) = 0;

	virtual OpenGLTextureOrigin origin( void ) const = 0;

	virtual void setOrigin( OpenGLTextureOrigin pOrigin ) = 0;
};

FUGIO_NAMESPACE_END

Q_DECLARE_INTERFACE( fugio::OpenGLTextureInterface, "com.bigfug.fugio.opengl.texture/1.0" )

#endif // TEXTUREINTERFACE__H
