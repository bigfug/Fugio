#include <stdlib.h>

#if defined( Q_OS_UNIX )
#include <malloc/malloc.h>
#endif

#include "imagepin.h"
#include <QSettings>
#include <QImage>

ImagePin::ImagePin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin ), mImageFormat( FORMAT_UNKNOWN ), mImageInternalFormat( -1 )
{
	memset( &mImageBuffer, 0, sizeof( mImageBuffer ) );
	memset( &mImagePointer, 0, sizeof( mImagePointer ) );
	memset( &mLineWidth, 0, sizeof( mLineWidth ) );
	memset( &mBufferSizes, 0, sizeof( mBufferSizes ) );
}

ImagePin::~ImagePin()
{
	for( int i = 0 ; i < 8 ; i++ )
	{
		if( mImageBuffer[ i ] )
		{
#if defined( Q_OS_WIN )
			_aligned_free( mImageBuffer[ i ] );
#elif defined( Q_OS_UNIX )
			free( mImageBuffer[ i ] );
#else
#error
#endif
			mImageBuffer[ i ] = 0;
			mBufferSizes[ i ] = 0;
		}
	}
}

QString ImagePin::toString() const
{
	QStringList		ValLst;

	if( mImageSize.isEmpty() )
	{
		ValLst << QString( "Empty Image" );
	}
	else
	{
		ValLst << QString( "%1x%2" ).arg( mImageSize.width() ).arg( mImageSize.height() );
	}

	QString		FmtStr = tr( "Format" );
	QString		FmtVal = "Unknown";

	switch( mImageFormat )
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

	const int	BufSiz = bufferSize( pIndex ); // mLineWidth[ pIndex ] * mImageSize.height();

	if( mBufferSizes[ pIndex ] != BufSiz )
	{
#if defined( Q_OS_WIN )
		if( ( mImageBuffer[ pIndex ] = reinterpret_cast<quint8 *>( _aligned_realloc( mImageBuffer[ pIndex ], BufSiz, 16 ) ) ) )
#elif defined( Q_OS_UNIX )
		if( mImageBuffer[ pIndex ] )
		{
			free( mImageBuffer[ pIndex ] );

			mImageBuffer[ pIndex ] = 0;
			mBufferSizes[ pIndex ] = 0;
		}

		if( posix_memalign( (void **)&mImageBuffer[ pIndex ], 16, BufSiz ) == 0 )
#endif
		{
			mBufferSizes[ pIndex ] = BufSiz;
		}
	}

	return( mImageBuffer[ pIndex ] );
}

quint8 *ImagePin::internalBuffer( int pIndex ) const
{
	Q_ASSERT( pIndex >= 0 && pIndex < PLANE_COUNT );

	const int	BufSiz = bufferSize( pIndex ); //mLineWidth[ pIndex ] * mImageSize.height();

	if( mBufferSizes[ pIndex ] != BufSiz )
	{
		if( !BufSiz )
		{
			if( mImageBuffer[ pIndex ] )
			{
#if defined( Q_OS_WIN )
				_aligned_free( mImageBuffer[ pIndex ] );
#elif defined( Q_OS_UNIX )
				free( mImageBuffer[ pIndex ] );
#endif

				mImageBuffer[ pIndex ] = 0;
			}
		}
		else if( !mImageBuffer[ pIndex ] )
		{
#if defined( Q_OS_WIN )
			mImageBuffer[ pIndex ] = reinterpret_cast<quint8 *>( _aligned_malloc( BufSiz, 16 ) );
#elif defined( Q_OS_UNIX )
			posix_memalign( (void **)&mImageBuffer[ pIndex ], 16, BufSiz );
#endif
		}
#if defined( Q_OS_WIN )
		else if( ( mImageBuffer[ pIndex ] = reinterpret_cast<quint8 *>( _aligned_realloc( mImageBuffer[ pIndex ], BufSiz, 16 ) ) ) )
		{
		}
#elif defined( Q_OS_UNIX )
		else
		{
			if( mImageBuffer[ pIndex ] )
			{
				free( mImageBuffer[ pIndex ] );

				mImageBuffer[ pIndex ] = 0;
				mBufferSizes[ pIndex ] = 0;
			}

			if( posix_memalign( (void **)&mImageBuffer[ pIndex ], 16, BufSiz ) == 0 )
			{

			}
		}
#endif

		mBufferSizes[ pIndex ] = BufSiz;
	}

	return( mImageBuffer[ pIndex ] );
}

quint8 **ImagePin::internalBuffers()
{
	for( int i = 0 ; i < PLANE_COUNT ; i++ )
	{
		internalBuffer( i );
	}

	return( mImageBuffer );
}

quint8 **ImagePin::internalBuffers() const
{
	for( int i = 0 ; i < PLANE_COUNT ; i++ )
	{
		internalBuffer( i );
	}

	return( mImageBuffer );
}

const quint8 *ImagePin::buffer( int pIndex )
{
	Q_ASSERT( pIndex >= 0 && pIndex < PLANE_COUNT );

	if( mImagePointer[ pIndex ] )
	{
		return( mImagePointer[ pIndex ] );
	}

	return( internalBuffer( pIndex ) );
}

const quint8 *ImagePin::buffer( int pIndex ) const
{
	Q_ASSERT( pIndex >= 0 && pIndex < PLANE_COUNT );

	if( mImagePointer[ pIndex ] )
	{
		return( mImagePointer[ pIndex ] );
	}

	return( internalBuffer( pIndex ) );
}

//const quint8 * const *ImagePin::buffers()
//{
//    return( mImagePointer[ 0 ] ? mImagePointer : mImageBuffer );
//}

const quint8 * const *ImagePin::buffers() const
{
	return( mImagePointer[ 0 ] ? mImagePointer : mImageBuffer );
}

int ImagePin::bufferSize( int pIndex ) const
{
	int			S = mImageSize.height() >= 0 ? mLineWidth[ pIndex ] * mImageSize.height() : 0;

	return( !pIndex ? S : S / 2  );
}

void ImagePin::unsetBuffers()
{
	for( int i = 0 ; i < PLANE_COUNT ; i++ )
	{
		mImagePointer[ i ] = 0;
		mLineWidth[ i ] = 0;
	}
}

void ImagePin::setBuffer( int pIndex, const quint8 *pBuffer )
{
	Q_ASSERT( pIndex >= 0 && pIndex < PLANE_COUNT );

	mImagePointer[ pIndex ] = pBuffer;
}

void ImagePin::setBuffers( quint8 * const pBuffer[] )
{
	for( int i = 0 ; i < PLANE_COUNT ; i++ )
	{
		mImagePointer[ i ] = pBuffer[ i ];
	}
}

void ImagePin::setSize( quint32 pWidth, quint32 pHeight )
{
	mImageSize = QSize( pWidth, pHeight );
}

void ImagePin::setFormat( fugio::ImageInterface::Format pFormat )
{
	mImageFormat = pFormat;
}

void ImagePin::setLineSize( int pIndex, int pLineSize )
{
	Q_ASSERT( pIndex >= 0 && pIndex < PLANE_COUNT );

	mLineWidth[ pIndex ] = pLineSize;
}

void ImagePin::setLineSizes( const int pLineSize[] )
{
	for( int i = 0 ; i < PLANE_COUNT ; i++ )
	{
		mLineWidth[ i ] = pLineSize[ i ];
	}
}

QImage ImagePin::image( void ) const
{
	static QVector<QRgb> GrayTable8;

	if( GrayTable8.isEmpty() )
	{
		for( int i = 0; i < 256; i++ ) GrayTable8.push_back( qRgb( i,i,i ) );
	}

	QImage::Format		ImageFormat = QImage::Format_ARGB32;

	switch( mImageFormat )
	{
		case fugio::ImageInterface::FORMAT_GRAY8:
			ImageFormat = QImage::Format_Indexed8;
			break;

		case fugio::ImageInterface::FORMAT_BGRA8:
			ImageFormat = QImage::Format_RGB32;
			break;

		case fugio::ImageInterface::FORMAT_RGB8:
			ImageFormat = QImage::Format_RGB888;
			break;

		case fugio::ImageInterface::FORMAT_RGBA8:
			ImageFormat = QImage::Format_ARGB32;
			break;

		default:
			return( QImage() );
	}

	QImage	IM( buffer( 0 ), mImageSize.width(), mImageSize.height(), lineSize( 0 ), ImageFormat );

	if( ImageFormat ==  QImage::Format_Indexed8 )
	{
		IM.setColorTable( GrayTable8 );
	}

	return( IM );
}

bool ImagePin::isValid() const
{
	return( !mImageSize.isEmpty() && mImageFormat != fugio::ImageInterface::FORMAT_UNKNOWN );
}

int ImagePin::sizeDimensions() const
{
	return( 2 );
}

float ImagePin::size(int pDimension) const
{
	if( pDimension == 0 ) return( mImageSize.width() );
	if( pDimension == 1 ) return( mImageSize.height() );

	return( 0 );
}

float ImagePin::sizeWidth() const
{
	return( mImageSize.width() );
}

float ImagePin::sizeHeight() const
{
	return( mImageSize.height() );
}

float ImagePin::sizeDepth() const
{
	return( 0 );
}

QSizeF ImagePin::toSizeF() const
{
	return( mImageSize );
}

QVector3D ImagePin::toVector3D() const
{
	return( QVector3D( mImageSize.width(), mImageSize.height(), 0 ) );
}
