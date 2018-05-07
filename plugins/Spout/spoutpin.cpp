#include "spoutpin.h"

#include <QSettings>
#include <QImage>

#include <QSet>
#include <QByteArray>

SpoutPin::SpoutPin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin ), mWidth( 0 ), mHeight( 0 )
{
	mName.resize( 256 );
}

SpoutPin::~SpoutPin( void )
{
#if defined( SPOUT_SUPPORTED )
	mReceiver.ReleaseReceiver();
#endif
}

bool SpoutPin::receiveTexture( const QString &pName )
{
#if defined( SPOUT_SUPPORTED )
	if( pName.isEmpty() )
	{
		mName[ 0 ] = 0x00;
	}
	else if( strcmp( mName.data(), pName.toLatin1().constData() ) != 0 )
	{
		mReceiver.ReleaseReceiver();

		strcpy( mName.data(), pName.toLatin1().constData() );
	}

	return( mReceiver.ReceiveTexture( mName.data(), mWidth, mHeight ) );
#else
	Q_UNUSED( pName )

	return( false );
#endif
}

QVector3D SpoutPin::textureSize() const
{
	return( QVector3D( mWidth, mHeight, 0 ) );
}

QVector3D SpoutPin::size() const
{
	return( QVector3D( mWidth, mHeight, 0 ) );
}

quint32 SpoutPin::srcTexId( void ) const
{
#if defined( SPOUT_SUPPORTED )
	return( -1 );
#else
	return( 0 );
#endif
}

quint32 SpoutPin::dstTexId( void ) const
{
	return( 0 );
}

QOpenGLTexture::Target SpoutPin::target() const
{
	return( QOpenGLTexture::Target2D );
}

QOpenGLTexture::PixelFormat SpoutPin::format() const
{
	return( QOpenGLTexture::BGRA );
}

QOpenGLTexture::TextureFormat SpoutPin::internalFormat() const
{
	return( QOpenGLTexture::RGBA8_UNorm );
}

QOpenGLTexture::PixelType SpoutPin::type() const
{
	return( QOpenGLTexture::UInt8 );
}

QOpenGLTexture::Filter SpoutPin::filterMin( void ) const
{
	return( QOpenGLTexture::Linear );
}

QOpenGLTexture::Filter SpoutPin::filterMag( void ) const
{
	return( QOpenGLTexture::Linear );
}

QOpenGLTexture::WrapMode SpoutPin::wrapS( void ) const
{
	return( QOpenGLTexture::ClampToEdge );
}

QOpenGLTexture::WrapMode SpoutPin::wrapT( void ) const
{
	return( QOpenGLTexture::ClampToEdge );
}

QOpenGLTexture::WrapMode SpoutPin::wrapR( void ) const
{
	return( QOpenGLTexture::ClampToEdge );
}

bool SpoutPin::genMipMaps( void ) const
{
	return( false );
}

void SpoutPin::free()
{
#if defined( SPOUT_SUPPORTED )
	mReceiver.ReleaseReceiver();
#endif
}

QImage SpoutPin::image()
{
	return( QImage() );
}

void SpoutPin::srcBind()
{
#if defined( SPOUT_SUPPORTED )
	mReceiver.BindSharedTexture();
#endif
}

void SpoutPin::release()
{
#if defined( SPOUT_SUPPORTED )
	mReceiver.UnBindSharedTexture();
#endif
}
