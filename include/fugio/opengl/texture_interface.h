#ifndef TEXTUREINTERFACE__H
#define TEXTUREINTERFACE__H

#include <QSize>
#include <QUuid>
#include <QVector>
#include <QVector2D>
#include <QVector3D>

#include <fugio/global.h>

class QImage;

FUGIO_NAMESPACE_BEGIN

class OpenGLTextureInterface
{
public:
	virtual ~OpenGLTextureInterface( void ) {}

	virtual QVector3D textureSize( void ) = 0;

	virtual QVector3D size( void ) = 0;

	virtual void setDoubleBuffered( bool pDoubleBuffered ) = 0;

	virtual bool isDoubleBuffered( void ) const = 0;

	virtual bool isDepthTexture( void ) const = 0;

	virtual void swapTexture( void ) = 0;

	virtual quint32 srcTexId( void ) const = 0;

	virtual quint32 dstTexId( void ) const = 0;

	virtual quint32 target( void ) = 0;

	virtual quint32 format( void ) = 0;

	virtual quint32 internalFormat( void ) = 0;

	virtual quint32 type( void ) = 0;

	virtual quint32 compare( void ) const = 0;

    virtual int filterMin( void ) const = 0;
    virtual int filterMag( void ) const = 0;
    virtual int wrapS( void ) const = 0;
    virtual int wrapT( void ) const = 0;
    virtual int wrapR( void ) const = 0;
    virtual bool genMipMaps( void ) const = 0;

	virtual void setCompare( quint32 pCompare ) = 0;

	virtual void setTarget( quint32 pTarget ) = 0;

	virtual void setSize( qint32 pWidth, qint32 pHeight = 0, qint32 pDepth = 0 ) = 0;

	virtual void setSize( const QVector3D &pSize ) = 0;

	virtual void setFormat( quint32 pFormat ) = 0;

	virtual void setType( quint32 pType ) = 0;

	virtual void setInternalFormat( quint32 pInternalFormat ) = 0;

    virtual void update( const unsigned char *pData, int pDataSize, int pCubeFaceIndex = 0 ) = 0;

	virtual void setFilter( quint32 pMin, quint32 pMag ) = 0;

	virtual void setWrap( quint32 pX, quint32 pY, quint32 pZ ) = 0;

    virtual void setGenMipMaps( bool pGenMipMaps ) = 0;

	virtual quint32 fbo( bool pUseDepth = false ) = 0;

	virtual void freeFbo( void ) = 0;

	virtual quint32 fboMultiSample( int pSamples, bool pUseDepth = false ) = 0;

    virtual void free( void ) = 0;

    virtual QImage image( void ) = 0;

	virtual void srcBind( void ) = 0;
	virtual void dstBind( void ) = 0;

    virtual void release( void ) = 0;
};

FUGIO_NAMESPACE_END

Q_DECLARE_INTERFACE( fugio::OpenGLTextureInterface, "com.bigfug.fugio.opengl.texture/1.0" )

#endif // TEXTUREINTERFACE__H
