#ifndef TEXTURE_H
#define TEXTURE_H

#include "opengl_includes.h"

#include <QObject>
#include <QOpenGLTexture>

#include <fugio/pin_control_interface.h>
#include <fugio/opengl/texture_interface.h>
#include <fugio/core/size_interface.h>

#include <fugio/opengl/uuid.h>

#include <fugio/pincontrolbase.h>

using namespace fugio;

class TexturePin : public fugio::PinControlBase, public fugio::OpenGLTextureInterface, public fugio::SizeInterface, protected QOpenGLFunctions
{
	Q_OBJECT
	Q_INTERFACES( fugio::OpenGLTextureInterface fugio::SizeInterface )

public:
	Q_INVOKABLE explicit TexturePin( QSharedPointer<fugio::PinInterface> pPin );

	virtual ~TexturePin( void );

	//-------------------------------------------------------------------------
	// fugio::PinControlInterface

	virtual QString toString( void ) const Q_DECL_OVERRIDE
	{
		return( QString( "%1" ).arg( mDstTex ? mDstTex->textureId() : 0 ) );
	}

	virtual QString description( void ) const Q_DECL_OVERRIDE
	{
		return( "Texture" );
	}

	//-------------------------------------------------------------------------
	// InterfaceTexture

	virtual QVector3D textureSize( void ) const Q_DECL_OVERRIDE;

	virtual QVector3D size( void ) const Q_DECL_OVERRIDE;

	virtual void setDoubleBuffered( bool pDoubleBuffered ) Q_DECL_OVERRIDE
	{
		mTexDsc.mDoubleBuffered = pDoubleBuffered;
	}

	virtual bool isDoubleBuffered( void ) const Q_DECL_OVERRIDE
	{
		return( mTexDsc.mDoubleBuffered );
	}

	virtual bool isDepthTexture( void ) const Q_DECL_OVERRIDE
	{
		switch( mTexDsc.mInternalFormat )
		{
			case QOpenGLTexture::D16:
			case QOpenGLTexture::D24:
			case QOpenGLTexture::D24S8:
			case QOpenGLTexture::D32:
			case QOpenGLTexture::D32F:
			case QOpenGLTexture::D32FS8X24:
			case QOpenGLTexture::DepthFormat:
				return( true );

			default:
				break;
		}

		return( false );
	}

	virtual quint32 srcTexId( void ) const Q_DECL_OVERRIDE;
	virtual quint32 dstTexId( void ) const Q_DECL_OVERRIDE;

	virtual QOpenGLTexture::Target target( void ) const Q_DECL_OVERRIDE;

	virtual QOpenGLTexture::PixelFormat format( void ) const Q_DECL_OVERRIDE;

	virtual QOpenGLTexture::TextureFormat internalFormat( void ) const Q_DECL_OVERRIDE;

	virtual QOpenGLTexture::PixelType type( void ) const Q_DECL_OVERRIDE;

	virtual QOpenGLTexture::Filter filterMin( void ) const Q_DECL_OVERRIDE;
	virtual QOpenGLTexture::Filter filterMag( void ) const Q_DECL_OVERRIDE;
	virtual QOpenGLTexture::WrapMode wrapS( void ) const Q_DECL_OVERRIDE;
	virtual QOpenGLTexture::WrapMode wrapT( void ) const Q_DECL_OVERRIDE;
	virtual QOpenGLTexture::WrapMode wrapR( void ) const Q_DECL_OVERRIDE;
	virtual bool genMipMaps( void ) const Q_DECL_OVERRIDE;

	virtual void setSize( qint32 pWidth, qint32 pHeight, qint32 pDepth ) Q_DECL_OVERRIDE;

	virtual void setSize( const QVector3D &pSize ) Q_DECL_OVERRIDE;

	virtual void setTarget( QOpenGLTexture::Target pTarget ) Q_DECL_OVERRIDE;

	virtual void setFormat( QOpenGLTexture::PixelFormat pFormat ) Q_DECL_OVERRIDE;

	virtual void setType( QOpenGLTexture::PixelType pType ) Q_DECL_OVERRIDE;

	virtual void setInternalFormat( QOpenGLTexture::TextureFormat pInternalFormat ) Q_DECL_OVERRIDE;

	virtual void update( void ) Q_DECL_OVERRIDE;

	virtual void update( const unsigned char *pData, int pDataSize, int pLineSize, int pCubeFaceIndex = 0 ) Q_DECL_OVERRIDE;

	//virtual QOpenGLFramebufferObject *fbo( const QOpenGLFramebufferObjectFormat &pFormat ) Q_DECL_OVERRIDE;

	virtual void setFilter( QOpenGLTexture::Filter pMin, QOpenGLTexture::Filter pMag ) Q_DECL_OVERRIDE;

	virtual void setWrap( QOpenGLTexture::WrapMode pX, QOpenGLTexture::WrapMode pY, QOpenGLTexture::WrapMode pZ ) Q_DECL_OVERRIDE;

	virtual void setGenMipMaps( bool pGenMipMaps ) Q_DECL_OVERRIDE;

	virtual void free( void ) Q_DECL_OVERRIDE;

	virtual QImage image( void ) Q_DECL_OVERRIDE;

	virtual void srcBind( void ) Q_DECL_OVERRIDE;
	virtual void dstBind( void ) Q_DECL_OVERRIDE;

	virtual void release( void ) Q_DECL_OVERRIDE;

	virtual quint32 fbo( bool pUseDepth = false ) Q_DECL_OVERRIDE;

	virtual void freeFbo( void ) Q_DECL_OVERRIDE;

	virtual quint32 fboMultiSample( int pSamples, bool pUseDepth = false ) Q_DECL_OVERRIDE;

	virtual void swapTexture( void ) Q_DECL_OVERRIDE;

	virtual QOpenGLTexture::ComparisonFunction compare() const Q_DECL_OVERRIDE;
	virtual void setCompare( QOpenGLTexture::ComparisonFunction pCompare ) Q_DECL_OVERRIDE;

	virtual OpenGLTextureDescription textureDescription() const Q_DECL_OVERRIDE
	{
		return( mTexDsc );
	}

	virtual void setTextureDescription( const fugio::OpenGLTextureDescription &pDescription ) Q_DECL_OVERRIDE
	{
		if( memcmp( &mTexDsc, &pDescription, sizeof( OpenGLTextureDescription ) ) != 0 )
		{
			memcpy( &mTexDsc, &pDescription, sizeof( OpenGLTextureDescription ) );

			mDefinitionChanged = true;
		}
	}

	virtual QOpenGLTexture *srcTex() Q_DECL_OVERRIDE
	{
		return( mSrcTex );
	}

	virtual QOpenGLTexture *dstTex() Q_DECL_OVERRIDE
	{
		return( mDstTex );
	}

	//-------------------------------------------------------------------------

	// SizeInterface interface
public:
	virtual int sizeDimensions() const Q_DECL_OVERRIDE;
	virtual float size(int pDimension) const Q_DECL_OVERRIDE;
	virtual float sizeWidth() const Q_DECL_OVERRIDE;
	virtual float sizeHeight() const Q_DECL_OVERRIDE;
	virtual float sizeDepth() const Q_DECL_OVERRIDE;
	virtual QSizeF toSizeF() const Q_DECL_OVERRIDE;
	virtual QVector3D toVector3D() const Q_DECL_OVERRIDE;

protected:
	static bool isCompressedFormat( QOpenGLTexture::TextureFormat pFormat );

	void checkDefinition();

private:
	OpenGLTextureDescription		mTexDsc;

	bool			mDefinitionChanged;

	QOpenGLTexture	*mSrcTex;
	QOpenGLTexture	*mDstTex;

	GLuint			mFBOId;
	GLuint			mFBODepthRBId;
	GLuint			mFBOBoundTexId;

	GLuint			mFBOMSId;
	GLuint			mFBOMSColourRBId;
	GLuint			mFBOMSDepthRBId;
};

#endif // TEXTURE_H
