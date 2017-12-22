#ifndef IMAGE_H
#define IMAGE_H

#include <fugio/global.h>
#include <fugio/image/image_interface.h>

#include <QMetaType>

FUGIO_NAMESPACE_BEGIN

class ImageData
{
public:
	ImageData( void ) : mFormat( fugio::ImageInterface::FORMAT_UNKNOWN ), mInternalFormat( -1 )
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
	const quint8			*mPointer[ PLANE_COUNT ];		// pointers to external buffers
	mutable int				 mBufferSizes[ PLANE_COUNT ];
	int						 mLineWidth[ PLANE_COUNT ];
	QSize					 mSize;
	ImageInterface::Format	 mFormat;
	int						 mInternalFormat;
};

class Image
{
public:
	Image( void )
		: mData( new ImageData() )
	{
	}

	Image( const Image &pOther )
		: mData( pOther.mData )
	{

	}

//	Image( Image &&pOther )
//		: mData( pOther.mData )
//	{

//	}

	~Image( void )
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

	inline void setFormat( ImageInterface::Format pFormat )
	{
		mData->mFormat = pFormat;
	}

	inline ImageInterface::Format format( void ) const
	{
		return( mData->mFormat );
	}

	inline bool isEmpty( void ) const
	{
		return( mData->mSize.isEmpty() );
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

	inline const quint8 *buffer( int pIndex )
	{
		return( mData->mPointer[ pIndex ] ? mData->mPointer[ pIndex ] : internalBuffer( pIndex ) );
	}

	inline const quint8 *buffer( int pIndex ) const
	{
		return( mData->mPointer[ pIndex ] ? mData->mPointer[ pIndex ] : internalBuffer( pIndex ) );
	}

	inline const quint8 * const *buffers() const
	{
		return( mData->mPointer[ 0 ] ? mData->mPointer : mData->mBuffer );
	}

	inline void setBuffer( int pIndex, const quint8 *pBuffer )
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
			if( ( mBuffer[ pIndex ] = reinterpret_cast<quint8 *>( _aligned_realloc( mBuffer[ pIndex ], BufSiz, 16 ) ) ) )
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
					_aligned_free( mBuffer[ pIndex ] );
	#elif defined( Q_OS_UNIX )
					free( mData->mBuffer[ pIndex ] );
	#endif

					mData->mBuffer[ pIndex ] = 0;
				}
			}
			else if( !mData->mBuffer[ pIndex ] )
			{
	#if defined( Q_OS_WIN )
				mBuffer[ pIndex ] = reinterpret_cast<quint8 *>( _aligned_malloc( BufSiz, 16 ) );
	#elif defined( Q_OS_UNIX )
				posix_memalign( (void **)&mData->mBuffer[ pIndex ], 16, BufSiz );
	#endif
			}
	#if defined( Q_OS_WIN )
			else if( ( mBuffer[ pIndex ] = reinterpret_cast<quint8 *>( _aligned_realloc( mBuffer[ pIndex ], BufSiz, 16 ) ) ) )
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

		QImage::Format		ImageFormat = QImage::Format_ARGB32;

		switch( mData->mFormat )
		{
			case ImageInterface::FORMAT_GRAY8:
				ImageFormat = QImage::Format_Indexed8;
				break;

			case ImageInterface::FORMAT_BGRA8:
				ImageFormat = QImage::Format_ARGB32;
				break;

			case ImageInterface::FORMAT_RGB8:
				ImageFormat = QImage::Format_RGB888;
				break;

			case ImageInterface::FORMAT_RGBA8:
				ImageFormat = QImage::Format_ARGB32;
				break;

			default:
				return( QImage() );
		}

		QImage	IM( buffer( 0 ), mData->mSize.width(), mData->mSize.height(), lineSize( 0 ), ImageFormat );

		if( ImageFormat ==  QImage::Format_Indexed8 )
		{
			IM.setColorTable( GrayTable8 );
		}

		return( IM );
	}

private:
	QSharedPointer<ImageData>		mData;
};

FUGIO_NAMESPACE_END

Q_DECLARE_METATYPE( fugio::Image )

#endif // IMAGE_H
