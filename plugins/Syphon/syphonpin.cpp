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

QVector3D SyphonPin::textureSize()
{
#if defined( SYPHON_SUPPORTED )
	return( QVector3D( mReceiver.textureWidth(), mReceiver.textureHeight(), 0 ) );
#else
	return( QVector3D() );
#endif
}

QVector3D SyphonPin::size()
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

quint32 SyphonPin::target()
{
#if defined( SYPHON_SUPPORTED )
	return( GL_TEXTURE_RECTANGLE );
#else
	return( 0 );
#endif
}

quint32 SyphonPin::format()
{
#if defined( SYPHON_SUPPORTED )
	return( GL_BGRA );
#else
	return( 0 );
#endif
}

quint32 SyphonPin::internalFormat()
{
#if defined( SYPHON_SUPPORTED )
	return( GL_RGBA8 );
#else
	return( 0 );
#endif
}

quint32 SyphonPin::type()
{
	return( GL_UNSIGNED_BYTE );
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

void SyphonPin::setTarget( quint32 )
{
}

void SyphonPin::setFormat( quint32 )
{
}

void SyphonPin::setType( quint32 )
{
}

void SyphonPin::setInternalFormat( quint32 )
{
}

void SyphonPin::update( const unsigned char *, int, int )
{
}

void SyphonPin::setFilter( quint32, quint32 )
{
}

void SyphonPin::setWrap( quint32, quint32, quint32 )
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

