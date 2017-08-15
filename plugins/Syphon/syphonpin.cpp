#include "syphonpin.h"

#include <QSettings>
#include <QImage>

#include <QSet>
#include <QByteArray>

SyphonPin::SyphonPin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin )
{
}

SyphonPin::~SyphonPin( void )
{
}

#if defined( SYPHON_SUPPORTED )
void SyphonPin::updateFromReceiver()
{
	mName = QString::fromStdString( mReceiver.serverName() );
	mSize = QVector3D( mReceiver.textureWidth(), mReceiver.textureHeight(), 0 );
}
#endif

QVector3D SyphonPin::textureSize() const
{
#if defined( SYPHON_SUPPORTED )
	return( QVector3D( mReceiver.textureWidth(), mReceiver.textureHeight(), 0 ) );
#else
	return( QVector3D() );
#endif
}

QVector3D SyphonPin::size() const
{
#if defined( SYPHON_SUPPORTED )
	return( QVector3D( mReceiver.textureWidth(), mReceiver.textureHeight(), 0 ) );
#else
	return( QVector3D() );
#endif
}

quint32 SyphonPin::srcTexId() const
{
#if defined( SYPHON_SUPPORTED )
	return( mReceiver.textureId() );
#else
	return( 0 );
#endif
}

quint32 SyphonPin::dstTexId() const
{
	return( 0 );
}

QOpenGLTexture::Target SyphonPin::target() const
{
	return( QOpenGLTexture::TargetRectangle );
}

QOpenGLTexture::PixelFormat SyphonPin::format() const
{
	return( QOpenGLTexture::BGRA );
}

QOpenGLTexture::TextureFormat SyphonPin::internalFormat() const
{
	return( QOpenGLTexture::RGBA8_UNorm );
}

QOpenGLTexture::PixelType SyphonPin::type() const
{
	return( QOpenGLTexture::UInt8 );
}

int SyphonPin::filterMin( void ) const
{
	return( GL_LINEAR );
}

int SyphonPin::filterMag( void ) const
{
	return( GL_LINEAR );
}

int SyphonPin::wrapS( void ) const
{
#if defined( SYPHON_SUPPORTED )
	return( GL_CLAMP );
#else
	return( 0 );
#endif
}

int SyphonPin::wrapT( void ) const
{
#if defined( SYPHON_SUPPORTED )
	return( GL_CLAMP );
#else
	return( 0 );
#endif
}

int SyphonPin::wrapR( void ) const
{
#if defined( SYPHON_SUPPORTED )
	return( GL_CLAMP );
#else
	return( 0 );
#endif
}

bool SyphonPin::genMipMaps( void ) const
{
	return( false );
}

void SyphonPin::setSize( qint32, qint32, qint32 )
{

}

void SyphonPin::setSize( const QVector3D & )
{

}

void SyphonPin::setTarget( QOpenGLTexture::Target )
{
}

void SyphonPin::setFormat( QOpenGLTexture::PixelFormat )
{
}

void SyphonPin::setType( QOpenGLTexture::PixelType )
{
}

void SyphonPin::setInternalFormat( QOpenGLTexture::TextureFormat )
{
}

void SyphonPin::update()
{

}

void SyphonPin::update( const unsigned char *, int, int, int )
{
}

void SyphonPin::setFilter( QOpenGLTexture::Filter, QOpenGLTexture::Filter )
{
}

void SyphonPin::setWrap( QOpenGLTexture::WrapMode, QOpenGLTexture::WrapMode, QOpenGLTexture::WrapMode )
{
}

void SyphonPin::setGenMipMaps( bool )
{
}

void SyphonPin::free()
{
}

QImage SyphonPin::image()
{
	QImage		Image;

	return( Image );
}

void SyphonPin::srcBind()
{
#if defined( SYPHON_SUPPORTED )
	mReceiver.bind();
#endif
}

void SyphonPin::dstBind()
{

}

void SyphonPin::release()
{
#if defined( SYPHON_SUPPORTED )
	mReceiver.release();
#endif
}
