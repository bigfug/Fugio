#ifndef IMAGE_H
#define IMAGE_H

#include <fugio/global.h>
#include <fugio/image/image_interface.h>

#include <QMetaType>

FUGIO_NAMESPACE_BEGIN

class Image
{
public:
	const static int PLANE_COUNT = 8;

	Image( void )
		: mFormat( fugio::ImageInterface::FORMAT_UNKNOWN ), mInternalFormat( -1 )
	{
		memset( &mBuffer, 0, sizeof( mBuffer ) );
		memset( &mPointer, 0, sizeof( mPointer ) );
		memset( &mLineWidth, 0, sizeof( mLineWidth ) );
		memset( &mBufferSizes, 0, sizeof( mBufferSizes ) );
	}

//	Image( const Image &pOther )
//		: mFormat( pOther.mFormat ), mInternalFormat( pOther.mInternalFormat )
//	{

//	}

//	Image( Image &&pOther ) noexcept
//	{

//	}

	~Image( void )
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

	inline QSize size( void ) const
	{
		return( mSize );
	}

	inline int width( void ) const
	{
		return( mSize.width() );
	}

	inline int height( void ) const
	{
		return( mSize.height() );
	}

	inline int lineSize( int pIndex ) const
	{
		Q_ASSERT( pIndex >= 0 && pIndex < PLANE_COUNT );

		return( mLineWidth[ pIndex ] );
	}

	inline const int *const lineSizes( void ) const
	{
		return( mLineWidth );
	}

	void setLineSize( int pIndex, int pLineSize )
	{
		mLineWidth[ pIndex ] = pLineSize;
	}

	void setLineSizes( const int pLineSize[] )
	{
		for( int i = 0 ; i < PLANE_COUNT ; i++ )
		{
			mLineWidth[ i ] = pLineSize[ i ];
		}
	}

	inline void setFormat( ImageInterface::Format pFormat )
	{
		mFormat = pFormat;
	}

	inline ImageInterface::Format format( void ) const
	{
		return( mFormat );
	}

	inline bool isEmpty( void ) const
	{
		return( mSize.isEmpty() );
	}

	inline void setSize( const QSize &pSize )
	{
		mSize = pSize;
	}

	inline void setSize( int pWidth, int pHeight )
	{
		mSize.setWidth( pWidth );
		mSize.setHeight( pHeight );
	}

	inline int internalFormat( void ) const
	{
		return( mInternalFormat );
	}

	inline void setInternalFormat( int pInternalFormat )
	{
		mInternalFormat = pInternalFormat;
	}

	inline int bufferSize( int pIndex ) const
	{
		return( mBufferSizes[ pIndex ] );
	}

	void unsetBuffers( void )
	{
		for( int i = 0 ; i < PLANE_COUNT ; i++ )
		{
			mPointer[ i ] = 0;
			mLineWidth[ i ] = 0;
		}
	}

	inline void setBufferSize( int pIndex, int pBufSze )
	{
		mBufferSizes[ pIndex ] = pBufSze;
	}

	inline const quint8 *buffer( int pIndex )
	{
		return( mPointer[ pIndex ] ? mPointer[ pIndex ] : internalBuffer( pIndex ) );
	}

	inline const quint8 *buffer( int pIndex ) const
	{
		return( mPointer[ pIndex ] ? mPointer[ pIndex ] : internalBuffer( pIndex ) );
	}

	inline const quint8 * const *buffers() const
	{
		return( mPointer[ 0 ] ? mPointer : mBuffer );
	}

	inline void setBuffer( int pIndex, const quint8 *pBuffer )
	{
		mPointer[ pIndex ] = pBuffer;
	}

	void setBuffers( quint8 * const pBuffer[] )
	{
		for( int i = 0 ; i < PLANE_COUNT ; i++ )
		{
			mPointer[ i ] = pBuffer[ i ];
		}
	}

	quint8 *internalBuffer( int pIndex )
	{
		Q_ASSERT( pIndex >= 0 && pIndex < PLANE_COUNT );

		const int	BufSiz = bufferSize( pIndex ); // mLineWidth[ pIndex ] * mImageSize.height();

		if( bufferSize( pIndex ) != BufSiz )
		{
	#if defined( Q_OS_WIN )
			if( ( mBuffer[ pIndex ] = reinterpret_cast<quint8 *>( _aligned_realloc( mBuffer[ pIndex ], BufSiz, 16 ) ) ) )
	#elif defined( Q_OS_UNIX )
			if( mBuffer[ pIndex ] )
			{
				free( mBuffer[ pIndex ] );

				mBuffer[ pIndex ] = 0;
				mBufferSizes[ pIndex ] = 0;
			}

			if( posix_memalign( (void **)&mBuffer[ pIndex ], 16, BufSiz ) == 0 )
	#endif
			{
				setBufferSize( pIndex, BufSiz );
			}
		}

		return( mBuffer[ pIndex ] );
	}

	quint8 *internalBuffer( int pIndex ) const
	{
		Q_ASSERT( pIndex >= 0 && pIndex < PLANE_COUNT );

		const int	BufSiz = bufferSize( pIndex ); //mLineWidth[ pIndex ] * mImageSize.height();

		if( mBufferSizes[ pIndex ] != BufSiz )
		{
			if( !BufSiz )
			{
				if( mBuffer[ pIndex ] )
				{
	#if defined( Q_OS_WIN )
					_aligned_free( mBuffer[ pIndex ] );
	#elif defined( Q_OS_UNIX )
					free( mBuffer[ pIndex ] );
	#endif

					mBuffer[ pIndex ] = 0;
				}
			}
			else if( !mBuffer[ pIndex ] )
			{
	#if defined( Q_OS_WIN )
				mBuffer[ pIndex ] = reinterpret_cast<quint8 *>( _aligned_malloc( BufSiz, 16 ) );
	#elif defined( Q_OS_UNIX )
				posix_memalign( (void **)&mBuffer[ pIndex ], 16, BufSiz );
	#endif
			}
	#if defined( Q_OS_WIN )
			else if( ( mBuffer[ pIndex ] = reinterpret_cast<quint8 *>( _aligned_realloc( mBuffer[ pIndex ], BufSiz, 16 ) ) ) )
			{
			}
	#elif defined( Q_OS_UNIX )
			else
			{
				if( mBuffer[ pIndex ] )
				{
					free( mBuffer[ pIndex ] );

					mBuffer[ pIndex ] = 0;
					mBufferSizes[ pIndex ] = 0;
				}

				if( posix_memalign( (void **)&mBuffer[ pIndex ], 16, BufSiz ) == 0 )
				{

				}
			}
	#endif

			mBufferSizes[ pIndex ] = BufSiz;
		}

		return( mBuffer[ pIndex ] );
	}

	quint8 **internalBuffers()
	{
		for( int i = 0 ; i < PLANE_COUNT ; i++ )
		{
			internalBuffer( i );
		}

		return( mBuffer );
	}

	inline quint8 **internalBuffers( void ) const
	{
		for( int i = 0 ; i < PLANE_COUNT ; i++ )
		{
			internalBuffer( i );
		}

		return( mBuffer );
	}

private:
	mutable quint8			*mBuffer[ PLANE_COUNT ];		// allocated buffers
	const quint8			*mPointer[ PLANE_COUNT ];		// pointers to external buffers
	mutable int				 mBufferSizes[ PLANE_COUNT ];
	int						 mLineWidth[ PLANE_COUNT ];
	QSize					 mSize;
	ImageInterface::Format	 mFormat;
	int						 mInternalFormat;
};

FUGIO_NAMESPACE_END

Q_DECLARE_METATYPE( fugio::Image )

#endif // IMAGE_H
