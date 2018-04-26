#ifndef IMAGE_H
#define IMAGE_H

#include <fugio/global.h>
#include <fugio/image/image.h>

#include <QMetaType>
#include <QSize>
#include <QImage>
#include <QSharedPointer>

FUGIO_NAMESPACE_BEGIN

typedef enum ImageFormat
{
	UNKNOWN = -1,
	INTERNAL,
	RGB8,
	RGBA8,
	BGR8,
	BGRA8,
	YUYV422,
	GRAY16,
	GRAY8,
	RG32,
	DXT1,
	DXT5,
	YCoCg_DXT5,
	HSV8,
	YUV420P,
	UYVY422,
	R32S,
	R32F,
	YUVJ422P,
	NV12,
	RGB_565
} ImageFormat;

class ImageData
{
public:
	ImageData( void ) : mFormat( ImageFormat::UNKNOWN ), mInternalFormat( -1 )
	{
		memset( &mBuffer, 0, sizeof( mBuffer ) );
		memset( &mPointer, 0, sizeof( mPointer ) );
		memset( &mLineWidth, 0, sizeof( mLineWidth ) );
		memset( &mBufferSizes, 0, sizeof( mBufferSizes ) );
	}

	~ImageData( void )
	{
		for( int i = 0 ; i < PLANE_COUNT ; i++ )
		{
			if( mBuffer[ i ] )
			{
#if defined( Q_OS_WIN )
				_aligned_free( mBuffer[ i ] );
#elif defined( Q_OS_UNIX )
				free( mBuffer[ i ] );
#else
#error
#endif
			}
		}
	}

	const static int PLANE_COUNT = 8;

	mutable quint8			*mBuffer[ PLANE_COUNT ];		// allocated buffers
	quint8					*mPointer[ PLANE_COUNT ];		// pointers to external buffers
	mutable int				 mBufferSizes[ PLANE_COUNT ];
	int						 mLineWidth[ PLANE_COUNT ];
	QSize					 mSize;
	ImageFormat				 mFormat;
	int						 mInternalFormat;
};

class Image
{
public:
	static int formatPixelByteCount( ImageFormat pFormat, int pBuffer = 0 )
	{
		switch( pFormat )
		{
			case ImageFormat::RGB8:		return( 3 );
			case ImageFormat::RGBA8:	return( 4 );
			case ImageFormat::BGR8:		return( 3 );
			case ImageFormat::BGRA8:	return( 4 );
			case ImageFormat::YUV420P:	return( !pBuffer ? 2 : 1 );
			case ImageFormat::YUYV422:	return( 2 );
			case ImageFormat::UYVY422:	return( 2 );
			case ImageFormat::GRAY16:	return( 2 );
			case ImageFormat::GRAY8:	return( 1 );
			case ImageFormat::RG32:		return( 4 );
			case ImageFormat::HSV8:		return( 3 );
			case ImageFormat::R32S:		return( 4 );
			case ImageFormat::R32F:		return( 4 );
			case ImageFormat::NV12:		return( 1 );

			default:			break;
		}

		return( 0 );
	}

	static QImage::Format toQImageFormat( ImageFormat pFormat )
	{
		switch( pFormat )
		{
			case ImageFormat::RGB8:		return( QImage::Format_RGB888 );
			case ImageFormat::RGBA8:	return( QImage::Format_ARGB32 );
			case ImageFormat::BGRA8:	return( QImage::Format_ARGB32 );
#if QT_VERSION >= QT_VERSION_CHECK( 5, 5, 0 )
			case ImageFormat::GRAY8:	return( QImage::Format_Grayscale8 );
#endif
			default:			break;
		}

		return( QImage::Format_Invalid );
	}

	static ImageFormat fromQImageFormat( QImage::Format pFormat )
	{
		switch( pFormat )
		{
			case QImage::Format_RGB888:					return( ImageFormat::RGB8 );
			case QImage::Format_ARGB32:					return( ImageFormat::RGBA8 );
			case QImage::Format_ARGB32_Premultiplied:	return( ImageFormat::RGBA8 );
#if QT_VERSION >= QT_VERSION_CHECK( 5, 5, 0 )
			case QImage::Format_Grayscale8:				return( ImageFormat::GRAY8 );
#endif
			default:									break;
		}

		return( ImageFormat::UNKNOWN );
	}

	Image( void ) : mData( new ImageData() )
	{
	}

	inline QSize size( void ) const
	{
		return( mData->mSize );
	}

	inline int width( void ) const
	{
		return( mData->mSize.width() );
	}

	inline int height( void ) const
	{
		return( mData->mSize.height() );
	}

	inline int lineSize( int pIndex ) const
	{
		Q_ASSERT( pIndex >= 0 && pIndex < ImageData::PLANE_COUNT );

		return( mData->mLineWidth[ pIndex ] );
	}

	inline const int *lineSizes( void ) const
	{
		return( mData->mLineWidth );
	}

	void setLineSize( int pIndex, int pLineSize )
	{
		mData->mLineWidth[ pIndex ] = pLineSize;
	}

	void setLineSizes( const int pLineSize[] )
	{
		for( int i = 0 ; i < ImageData::PLANE_COUNT ; i++ )
		{
			mData->mLineWidth[ i ] = pLineSize[ i ];
		}
	}

	inline void setFormat( ImageFormat pFormat )
	{
		mData->mFormat = pFormat;
	}

	inline ImageFormat format( void ) const
	{
		return( mData->mFormat );
	}

	inline bool isEmpty( void ) const
	{
		return( mData->mSize.isEmpty() );
	}

	inline bool isValid( void ) const
	{
		return( !isEmpty() && format() != ImageFormat::UNKNOWN );
	}

	inline void setSize( const QSize &pSize )
	{
		mData->mSize = pSize;
	}

	inline void setSize( int pWidth, int pHeight )
	{
		mData->mSize.setWidth( pWidth );
		mData->mSize.setHeight( pHeight );
	}

	inline int internalFormat( void ) const
	{
		return( mData->mInternalFormat );
	}

	inline void setInternalFormat( int pInternalFormat )
	{
		mData->mInternalFormat = pInternalFormat;
	}

	inline int bufferSize( int pIndex ) const
	{
		int      S = mData->mSize.height() >= 0 ? lineSize( pIndex ) * mData->mSize.height() : 0;

		return( !pIndex ? S : S / 2  );
	}

	void unsetBuffers( void )
	{
		for( int i = 0 ; i < ImageData::PLANE_COUNT ; i++ )
		{
			mData->mPointer[ i ] = 0;
			mData->mLineWidth[ i ] = 0;
		}
	}

	inline void setBufferSize( int pIndex, int pBufSze )
	{
		mData->mBufferSizes[ pIndex ] = pBufSze;
	}

	inline quint8 *buffer( int pIndex )
	{
		return( mData->mPointer[ pIndex ] ? mData->mPointer[ pIndex ] : internalBuffer( pIndex ) );
	}

	inline const quint8 *buffer( int pIndex ) const
	{
		return( mData->mPointer[ pIndex ] ? mData->mPointer[ pIndex ] : internalBuffer( pIndex ) );
	}

	inline const quint8 * const *buffers() const
	{
		const ImageData	*IM = mData.data();

		return( IM->mPointer[ 0 ] ? IM->mPointer : IM->mBuffer );
	}

	inline void setBuffer( int pIndex, quint8 *pBuffer )
	{
		mData->mPointer[ pIndex ] = pBuffer;
	}

	void setBuffers( quint8 * const pBuffer[] )
	{
		for( int i = 0 ; i < ImageData::PLANE_COUNT ; i++ )
		{
			mData->mPointer[ i ] = pBuffer[ i ];
		}
	}

	quint8 *internalBuffer( int pIndex )
	{
		Q_ASSERT( pIndex >= 0 && pIndex < ImageData::PLANE_COUNT );

		const int	BufSiz = bufferSize( pIndex ); // mLineWidth[ pIndex ] * mImageSize.height();

		if( mData->mBufferSizes[ pIndex ] != BufSiz )
		{
	#if defined( Q_OS_WIN )
			if( ( mData->mBuffer[ pIndex ] = reinterpret_cast<quint8 *>( _aligned_realloc( mData->mBuffer[ pIndex ], BufSiz, 16 ) ) ) )
	#elif defined( Q_OS_UNIX )
			if( mData->mBuffer[ pIndex ] )
			{
				free( mData->mBuffer[ pIndex ] );

				mData->mBuffer[ pIndex ] = 0;
				mData->mBufferSizes[ pIndex ] = 0;
			}

			if( posix_memalign( (void **)&mData->mBuffer[ pIndex ], 16, BufSiz ) == 0 )
	#endif
			{
				setBufferSize( pIndex, BufSiz );
			}
		}

		return( mData->mBuffer[ pIndex ] );
	}

	quint8 *internalBuffer( int pIndex ) const
	{
		Q_ASSERT( pIndex >= 0 && pIndex < ImageData::PLANE_COUNT );

		const int	BufSiz = bufferSize( pIndex ); //mLineWidth[ pIndex ] * mImageSize.height();

		if( mData->mBufferSizes[ pIndex ] != BufSiz )
		{
			if( !BufSiz )
			{
				if( mData->mBuffer[ pIndex ] )
				{
	#if defined( Q_OS_WIN )
					_aligned_free( mData->mBuffer[ pIndex ] );
	#elif defined( Q_OS_UNIX )
					free( mData->mBuffer[ pIndex ] );
	#endif

					mData->mBuffer[ pIndex ] = 0;
				}
			}
			else if( !mData->mBuffer[ pIndex ] )
			{
	#if defined( Q_OS_WIN )
				mData->mBuffer[ pIndex ] = reinterpret_cast<quint8 *>( _aligned_malloc( BufSiz, 16 ) );
	#elif defined( Q_OS_UNIX )
				posix_memalign( (void **)&mData->mBuffer[ pIndex ], 16, BufSiz );
	#endif
			}
	#if defined( Q_OS_WIN )
			else if( ( mData->mBuffer[ pIndex ] = reinterpret_cast<quint8 *>( _aligned_realloc( mData->mBuffer[ pIndex ], BufSiz, 16 ) ) ) )
			{
			}
	#elif defined( Q_OS_UNIX )
			else
			{
				if( mData->mBuffer[ pIndex ] )
				{
					free( mData->mBuffer[ pIndex ] );

					mData->mBuffer[ pIndex ] = 0;
					mData->mBufferSizes[ pIndex ] = 0;
				}

				if( posix_memalign( (void **)&mData->mBuffer[ pIndex ], 16, BufSiz ) == 0 )
				{

				}
			}
	#endif

			mData->mBufferSizes[ pIndex ] = BufSiz;
		}

		return( mData->mBuffer[ pIndex ] );
	}

	quint8 **internalBuffers()
	{
		for( int i = 0 ; i < ImageData::PLANE_COUNT ; i++ )
		{
			internalBuffer( i );
		}

		return( mData->mBuffer );
	}

	inline quint8 **internalBuffers( void ) const
	{
		for( int i = 0 ; i < ImageData::PLANE_COUNT ; i++ )
		{
			internalBuffer( i );
		}

		return( mData->mBuffer );
	}

	QImage image( void ) const
	{
		static QVector<QRgb> GrayTable8;

		if( GrayTable8.isEmpty() )
		{
			for( int i = 0; i < 256; i++ ) GrayTable8.push_back( qRgb( i,i,i ) );
		}

		QImage::Format		Format = QImage::Format_ARGB32;

		switch( mData->mFormat )
		{
			case ImageFormat::GRAY8:
				Format = QImage::Format_Indexed8;
				break;

			case ImageFormat::BGRA8:
				Format = QImage::Format_ARGB32;
				break;

			case ImageFormat::RGB8:
				Format = QImage::Format_RGB888;
				break;

			case ImageFormat::RGBA8:
				Format = QImage::Format_ARGB32;
				break;

			default:
				return( QImage() );
		}

		QImage	IM( buffer( 0 ), mData->mSize.width(), mData->mSize.height(), lineSize( 0 ), Format );

		if( Format == QImage::Format_Indexed8 )
		{
			IM.setColorTable( GrayTable8 );
		}

		return( IM );
	}

	void detach( void )
	{
		mData.reset( new ImageData() );
	}

private:
	QSharedPointer<ImageData>		mData;
};

FUGIO_NAMESPACE_END

Q_DECLARE_METATYPE( fugio::Image )

#endif // IMAGE_H
