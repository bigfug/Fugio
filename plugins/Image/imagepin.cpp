#include <stdlib.h>

#if defined( Q_OS_UNIX )
#include <malloc/malloc.h>
#endif

#include "imagepin.h"
#include <QSettings>
#include <QImage>

ImagePin::ImagePin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin )
{
}

ImagePin::~ImagePin()
{
}

QString ImagePin::toString() const
{
	QStringList		ValLst;

	if( mImage.isEmpty() )
	{
		ValLst << QString( "Empty Image" );
	}
	else
	{
		ValLst << QString( "%1x%2" ).arg( mImage.width() ).arg( mImage.height() );
	}

	QString		FmtStr = tr( "Format" );
	QString		FmtVal = "Unknown";

	switch( mImage.format() )
	{
		case FORMAT_UNKNOWN:		FmtVal = "FORMAT_UNKNOWN";		break;
		case FORMAT_INTERNAL:		FmtVal = "FORMAT_INTERNAL";		break;
		case FORMAT_RGB8:			FmtVal = "FORMAT_RGB8";			break;
		case FORMAT_RGBA8:			FmtVal = "FORMAT_RGBA8";		break;
		case FORMAT_BGR8:			FmtVal = "FORMAT_BGR8";			break;
		case FORMAT_BGRA8:			FmtVal = "FORMAT_BGRA8";		break;
		case FORMAT_YUYV422:		FmtVal = "FORMAT_YUYV422";		break;
		case FORMAT_UYVY422:		FmtVal = "FORMAT_UYVY422";		break;
		case FORMAT_YUV420P:		FmtVal = "FORMAT_YUV420P";		break;
		case FORMAT_GRAY16:			FmtVal = "FORMAT_GRAY16";		break;
		case FORMAT_GRAY8:			FmtVal = "FORMAT_GRAY8";		break;
		case FORMAT_RG32:			FmtVal = "FORMAT_RG32";			break;
		case FORMAT_DXT1:			FmtVal = "FORMAT_DXT1";			break;
		case FORMAT_DXT5:			FmtVal = "FORMAT_DXT5";			break;
		case FORMAT_YCoCg_DXT5:		FmtVal = "FORMAT_YCoCg_DXT5";	break;
		case FORMAT_HSV8:			FmtVal = "FORMAT_HSV8";			break;
		case FORMAT_R32S:			FmtVal = "FORMAT_R32S";			break;
		case FORMAT_R32F:			FmtVal = "FORMAT_R32F";			break;
		case FORMAT_NV12:			FmtVal = "FORMAT_NV12";			break;

		default:
			FmtStr = "Unknown";
			break;
	}

	ValLst << QString( "%1: %2" ).arg( FmtStr ).arg( FmtVal );

	return( ValLst.join( '\n' ) );
}

quint8 *ImagePin::internalBuffer( int pIndex )
{
	Q_ASSERT( pIndex >= 0 && pIndex < PLANE_COUNT );

	return( mImage.internalBuffer( pIndex ) );
}

quint8 *ImagePin::internalBuffer( int pIndex ) const
{
	Q_ASSERT( pIndex >= 0 && pIndex < PLANE_COUNT );

	return( mImage.internalBuffer( pIndex ) );
}

quint8 **ImagePin::internalBuffers()
{
	return( mImage.internalBuffers() );
}

quint8 **ImagePin::internalBuffers() const
{
	return( mImage.internalBuffers() );
}

const quint8 *ImagePin::buffer( int pIndex )
{
	Q_ASSERT( pIndex >= 0 && pIndex < PLANE_COUNT );

	return( mImage.buffer( pIndex ) );
}

const quint8 *ImagePin::buffer( int pIndex ) const
{
	Q_ASSERT( pIndex >= 0 && pIndex < PLANE_COUNT );

	return( mImage.buffer( pIndex ) );
}

const quint8 * const *ImagePin::buffers() const
{
	return( mImage.buffers() );
}

int ImagePin::bufferSize( int pIndex ) const
{
	int			S = mImage.height() >= 0 ? mImage.lineSize( pIndex ) * mImage.height() : 0;

	return( !pIndex ? S : S / 2  );
}

void ImagePin::unsetBuffers()
{
	mImage.unsetBuffers();
}

void ImagePin::setBuffer( int pIndex, const quint8 *pBuffer )
{
	Q_ASSERT( pIndex >= 0 && pIndex < PLANE_COUNT );

	mImage.setBuffer( pIndex, pBuffer );
}

void ImagePin::setBuffers( quint8 * const pBuffer[] )
{
	mImage.setBuffers( pBuffer );
}

void ImagePin::setSize( quint32 pWidth, quint32 pHeight )
{
	mImage.setSize( pWidth, pHeight );
}

void ImagePin::setFormat( fugio::ImageInterface::Format pFormat )
{
	mImage.setFormat( pFormat );
}

void ImagePin::setLineSize( int pIndex, int pLineSize )
{
	Q_ASSERT( pIndex >= 0 && pIndex < PLANE_COUNT );

	mImage.setLineSize( pIndex, pLineSize );
}

void ImagePin::setLineSizes( const int pLineSize[] )
{
	mImage.setLineSizes( pLineSize );
}

QImage ImagePin::image( void ) const
{
	return( mImage.image() );
}

bool ImagePin::isValid() const
{
	return( !mImage.isEmpty() && mImage.format() != fugio::ImageInterface::FORMAT_UNKNOWN );
}

int ImagePin::sizeDimensions() const
{
	return( 2 );
}

float ImagePin::size(int pDimension) const
{
	if( pDimension == 0 ) return( mImage.width() );
	if( pDimension == 1 ) return( mImage.height() );

	return( 0 );
}

float ImagePin::sizeWidth() const
{
	return( mImage.width() );
}

float ImagePin::sizeHeight() const
{
	return( mImage.height() );
}

float ImagePin::sizeDepth() const
{
	return( 0 );
}

QSizeF ImagePin::toSizeF() const
{
	return( mImage.size() );
}

QVector3D ImagePin::toVector3D() const
{
	return( QVector3D( mImage.width(), mImage.height(), 0 ) );
}

void ImagePin::setVariant( const QVariant &pValue )
{
	if( QMetaType::Type( pValue.type() ) == QMetaType::type( "fugio::Image" ) )
	{
		mImage = pValue.value<fugio::Image>();
	}
}

QVariant ImagePin::variant() const
{
	QVariant	V;

	V.setValue( mImage );

	return( V );
}

void ImagePin::setFromBaseVariant( const QVariant &pValue )
{
	if( QMetaType::Type( pValue.type() ) == QMetaType::type( "fugio::Image" ) )
	{
		mImage = pValue.value<fugio::Image>();
	}
}

QVariant ImagePin::baseVariant() const
{
	QVariant	V;

	V.setValue( mImage );

	return( V );
}
