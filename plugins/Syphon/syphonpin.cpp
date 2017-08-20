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

QOpenGLTexture::Filter SyphonPin::filterMin( void ) const
{
	return( QOpenGLTexture::Linear );
}

QOpenGLTexture::Filter SyphonPin::filterMag( void ) const
{
	return( QOpenGLTexture::Linear );
}

QOpenGLTexture::WrapMode SyphonPin::wrapS( void ) const
{
	return( QOpenGLTexture::ClampToEdge );
}

QOpenGLTexture::WrapMode SyphonPin::wrapT( void ) const
{
	return( QOpenGLTexture::ClampToEdge );
}

QOpenGLTexture::WrapMode SyphonPin::wrapR( void ) const
{
	return( QOpenGLTexture::ClampToEdge );
}

bool SyphonPin::genMipMaps( void ) const
{
	return( false );
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
