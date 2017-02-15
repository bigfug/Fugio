#ifndef SPOUTPIN_H
#define SPOUTPIN_H

#include "opengl_includes.h"

#include <QObject>

#include <fugio/pin_control_interface.h>
#include <fugio/opengl/texture_interface.h>

#include <fugio/pincontrolbase.h>

#if defined( SPOUT_SUPPORTED )
#include <Spout.h>
#endif

using namespace fugio;

class SpoutPin : public fugio::PinControlBase, public fugio::OpenGLTextureInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::OpenGLTextureInterface )

public:
	Q_INVOKABLE explicit SpoutPin( QSharedPointer<fugio::PinInterface> pPin );

	virtual ~SpoutPin( void );

	bool receiveTexture( const QString &pName );

	const QVector<char> &name( void ) const
	{
		return( mName );
	}

	//-------------------------------------------------------------------------
	// fugio::PinControlInterface

	virtual QString toString( void ) const Q_DECL_OVERRIDE
	{
		return( QString( "%1: %2x%3" ).arg( QString( mName.data() ) ).arg( mWidth ).arg( mHeight ) );
	}

	virtual QString description( void ) const Q_DECL_OVERRIDE
	{
		return( "Spout" );
	}

	//-------------------------------------------------------------------------
	// InterfaceTexture

	virtual QVector3D textureSize( void ) const Q_DECL_OVERRIDE;

	virtual QVector3D size( void ) const Q_DECL_OVERRIDE;

	virtual quint32 srcTexId( void ) const Q_DECL_OVERRIDE;
	virtual quint32 dstTexId( void ) const Q_DECL_OVERRIDE;

	virtual quint32 target( void ) const Q_DECL_OVERRIDE;

	virtual quint32 format( void ) const Q_DECL_OVERRIDE;

	virtual quint32 internalFormat( void ) const Q_DECL_OVERRIDE;

	virtual quint32 type( void ) const Q_DECL_OVERRIDE;

	virtual int filterMin( void ) const Q_DECL_OVERRIDE;
	virtual int filterMag( void ) const Q_DECL_OVERRIDE;
	virtual int wrapS( void ) const Q_DECL_OVERRIDE;
	virtual int wrapT( void ) const Q_DECL_OVERRIDE;
	virtual int wrapR( void ) const Q_DECL_OVERRIDE;
	virtual bool genMipMaps( void ) const Q_DECL_OVERRIDE;

	virtual void setSize( qint32 pWidth, qint32 pHeight = 0, qint32 pDepth = 0 ) Q_DECL_OVERRIDE;

	virtual void setSize( const QVector3D &pSize ) Q_DECL_OVERRIDE;

	virtual void setTarget( quint32 pTarget ) Q_DECL_OVERRIDE;

	virtual void setFormat( quint32 pFormat ) Q_DECL_OVERRIDE;

	virtual void setType( quint32 pType ) Q_DECL_OVERRIDE;

	virtual void setInternalFormat( quint32 pInternalFormat ) Q_DECL_OVERRIDE;

	virtual void update( void ) Q_DECL_OVERRIDE;

	virtual void update( const unsigned char *pData, int pDataSize, int pLineSize, int pCubeFaceIndex = 0 ) Q_DECL_OVERRIDE;

	virtual void setFilter( quint32 pMin, quint32 pMag ) Q_DECL_OVERRIDE;

	virtual void setWrap( quint32 pX, quint32 pY, quint32 pZ ) Q_DECL_OVERRIDE;

	virtual void setGenMipMaps( bool pGenMipMaps ) Q_DECL_OVERRIDE;

	virtual void free( void ) Q_DECL_OVERRIDE;

	virtual QImage image( void ) Q_DECL_OVERRIDE;

	virtual void srcBind( void ) Q_DECL_OVERRIDE;
	virtual void dstBind( void ) Q_DECL_OVERRIDE;

	virtual void release( void ) Q_DECL_OVERRIDE;

	virtual quint32 fbo( bool ) Q_DECL_OVERRIDE
	{
		return( 0 );
	}

	virtual void freeFbo( void ) Q_DECL_OVERRIDE
	{

	}

	virtual quint32 fboMultiSample( int, bool ) Q_DECL_OVERRIDE
	{
		return( 0 );
	}

	virtual void swapTexture( void ) Q_DECL_OVERRIDE
	{

	}

	virtual void setDoubleBuffered( bool ) Q_DECL_OVERRIDE
	{
	}

	virtual bool isDoubleBuffered( void ) const Q_DECL_OVERRIDE
	{
		return( false );
	}

	virtual bool isDepthTexture( void ) const Q_DECL_OVERRIDE
	{
		return( false );
	}

	virtual quint32 compare() const Q_DECL_OVERRIDE
	{
		return( GL_NONE );
	}

	virtual void setCompare(quint32 pCompare) Q_DECL_OVERRIDE
	{
		Q_UNUSED( pCompare )
	}

	virtual OpenGLTextureDescription textureDescription() const Q_DECL_OVERRIDE
	{
		return( OpenGLTextureDescription() );
	}

	virtual void setTextureDescription( const OpenGLTextureDescription &pDescription ) Q_DECL_OVERRIDE
	{
		Q_UNUSED( pDescription )
	}

	//-------------------------------------------------------------------------

private:
#if defined( SPOUT_SUPPORTED )
	SpoutReceiver			 mReceiver;
#endif

	QVector<char>			 mName;
	unsigned int			 mWidth;
	unsigned int			 mHeight;
};

#endif // SPOUTPIN_H
