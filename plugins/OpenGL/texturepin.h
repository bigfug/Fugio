#ifndef TEXTURE_H
#define TEXTURE_H

#include "opengl_includes.h"

#include <QObject>

#include <fugio/pin_control_interface.h>
#include <fugio/opengl/texture_interface.h>
#include <fugio/core/size_interface.h>

#include <fugio/opengl/uuid.h>

#include <fugio/pincontrolbase.h>

using namespace fugio;

class TexturePin : public fugio::PinControlBase, public fugio::OpenGLTextureInterface, public fugio::SizeInterface
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
		return( QString( "%1" ).arg( mDstTexId ) );
	}

	virtual QString description( void ) const Q_DECL_OVERRIDE
	{
		return( "Texture" );
	}

	virtual void loadSettings( QSettings & ) Q_DECL_OVERRIDE;
	virtual void saveSettings( QSettings & ) const Q_DECL_OVERRIDE;

	//-------------------------------------------------------------------------
	// InterfaceTexture

	virtual QVector3D textureSize( void ) Q_DECL_OVERRIDE;

	virtual QVector3D size( void ) Q_DECL_OVERRIDE;

	virtual void setDoubleBuffered( bool pDoubleBuffered ) Q_DECL_OVERRIDE
	{
		mDoubleBuffered = pDoubleBuffered;
	}

	virtual bool isDoubleBuffered( void ) const Q_DECL_OVERRIDE
	{
		return( mDoubleBuffered );
	}

	virtual bool isDepthTexture( void ) const Q_DECL_OVERRIDE
	{
		switch( mTextureInternalFormat )
		{
			case GL_DEPTH_COMPONENT:
			case GL_DEPTH_COMPONENT16:
			case GL_DEPTH_COMPONENT24:
			case GL_DEPTH_COMPONENT32:
			case GL_DEPTH_COMPONENT32F:
				return( true );
		}

		return( false );
	}

	virtual quint32 srcTexId( void ) const Q_DECL_OVERRIDE;
	virtual quint32 dstTexId( void ) const Q_DECL_OVERRIDE;

	virtual quint32 target( void ) Q_DECL_OVERRIDE;

	virtual quint32 format( void ) Q_DECL_OVERRIDE;

	virtual quint32 internalFormat( void ) Q_DECL_OVERRIDE;

	virtual quint32 type( void ) Q_DECL_OVERRIDE;

	virtual int filterMin( void ) const Q_DECL_OVERRIDE;
	virtual int filterMag( void ) const Q_DECL_OVERRIDE;
	virtual int wrapS( void ) const Q_DECL_OVERRIDE;
	virtual int wrapT( void ) const Q_DECL_OVERRIDE;
	virtual int wrapR( void ) const Q_DECL_OVERRIDE;
	virtual bool genMipMaps( void ) const Q_DECL_OVERRIDE;

	virtual void setSize( qint32 pWidth, qint32 pHeight, qint32 pDepth ) Q_DECL_OVERRIDE;

	virtual void setSize( const QVector3D &pSize ) Q_DECL_OVERRIDE;

	virtual void setTarget( quint32 pTarget ) Q_DECL_OVERRIDE;

	virtual void setFormat( quint32 pFormat ) Q_DECL_OVERRIDE;

	virtual void setType( quint32 pType ) Q_DECL_OVERRIDE;

	virtual void setInternalFormat( quint32 pInternalFormat ) Q_DECL_OVERRIDE;

	virtual void update( void ) Q_DECL_OVERRIDE;

	virtual void update( const unsigned char *pData, int pDataSize, int pLineSize, int pCubeFaceIndex = 0 ) Q_DECL_OVERRIDE;

	//virtual QOpenGLFramebufferObject *fbo( const QOpenGLFramebufferObjectFormat &pFormat ) Q_DECL_OVERRIDE;

	virtual void setFilter( quint32 pMin, quint32 pMag ) Q_DECL_OVERRIDE;

	virtual void setWrap( quint32 pX, quint32 pY, quint32 pZ ) Q_DECL_OVERRIDE;

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

	virtual quint32 compare() const Q_DECL_OVERRIDE;
	virtual void setCompare(quint32 pCompare) Q_DECL_OVERRIDE;

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
	static bool isCompressedFormat( GLenum pFormat );

	void checkDefinition();

private:
	GLenum			mTextureFormat;
	GLenum			mTextureInternalFormat;

	QVector3D		mTextureSize;

	QVector3D		mImageSize;

	GLuint			mSrcTexId;
	GLuint			mDstTexId;

	quint32			mTarget;
	GLenum			mTextureType;

	GLenum			mTextureMinFilter;
	GLenum			mTextureMagFilter;

	GLint			mTextureWrapX;
	GLint			mTextureWrapY;
	GLint			mTextureWrapZ;

	bool			mDefinitionChanged;
	bool			mGenerateMipMaps;

	GLuint			mFBOId;
	GLuint			mFBODepthRBId;
	GLuint			mFBOBoundTexId;

	GLuint			mFBOMSId;
	GLuint			mFBOMSColourRBId;
	GLuint			mFBOMSDepthRBId;

	bool			mDoubleBuffered;

	GLint			mCompare;
};

#endif // TEXTURE_H
