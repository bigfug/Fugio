#ifndef VIDEOCAPTUREDEVICE_H
#define VIDEOCAPTUREDEVICE_H

#include <QObject>
#include <QThread>
#include <QSharedData>
#include <QDateTime>
#include <QDebug>

#if defined( VIDEOCAPTURE_SUPPORTED )
#include <videocapture/Capture.h>
#endif

class VideoFrameData : public QSharedData
{
public:
	VideoFrameData( void )
		: mWidth( -1 ), mHeight( -1 ), mTimeStamp( -1 )
	{
		for( int i = 0 ; i < 3 ; i++ )
		{
			plane[ i ] = 0;
		}
	}

#if defined( VIDEOCAPTURE_SUPPORTED )
	VideoFrameData( int pWidth, int pHeight, qint64 pTimeStamp, const ca::PixelBuffer &pPixelBuffer )
		: mWidth( pWidth ), mHeight( pHeight ), mTimeStamp( pTimeStamp )
	{
		for( int i = 0 ; i < 3 ; i++ )
		{
			size_t		PlaneSize = pPixelBuffer.stride[ i ] * pPixelBuffer.height[ i ];

			if( PlaneSize )
			{
				plane[ i ] = new uint8_t[ PlaneSize ];

				memcpy( plane[ i ], pPixelBuffer.plane[ i ], PlaneSize );
			}
			else
			{
				plane[ i ] = nullptr;
			}

			stride[ i ] = pPixelBuffer.stride[ i ];
			width[ i ] = pPixelBuffer.width[ i ];
			height[ i ] = pPixelBuffer.height[ i ];
			offset[ i ] = pPixelBuffer.offset[ i ];
		}

		nbytes = pPixelBuffer.nbytes;
		pixel_format = pPixelBuffer.pixel_format;
	}
#endif

	VideoFrameData( const VideoFrameData &other )
		: QSharedData( other ), nbytes( other.nbytes ), pixel_format( other.pixel_format ),
		  mWidth( other.mWidth ), mHeight( other.mHeight ), mTimeStamp( other.mTimeStamp )
	{
		for( int i = 0 ; i < 3 ; i++ )
		{
			plane[ i ] = other.plane[ i ];
			stride[ i ] = other.stride[ i ];
			width[ i ] = other.width[ i ];
			height[ i ] = other.height[ i ];
			offset[ i ] = other.offset[ i ];
		}
	}

	~VideoFrameData()
	{
		for( int i = 0 ; i < 3 ; i++ )
		{
			if( plane[ i ] )
			{
				delete plane[ i ];
			}
		}
	}

public:
	uint8_t		*plane[3];                                                              /* Pointers to the pixel data; when we're a planar format all members are set, if packets only plane[0] */
	size_t		 stride[3];                                                               /* The number of bytes you should jump per row when reading the pixel data. Note that some buffer may have extra bytse at the end for memory alignment. */
	size_t		 width[3];                                                                /* The width; when planar each plane will have it's own value; otherwise only the first index is set. */
	size_t		 height[3];                                                               /* The height; when planar each plane will have it's own value; otherwise only the first index is set. */
	size_t		 offset[3];                                                               /* When the data is planar but packed, these contains the byte offsets from the first byte / plane. e.g. you can use this with YUV420P. */
	size_t		 nbytes;                                                                  /* The total number of bytes that make up the frame. This doesn't have to be one continuous array when the data is planar. */
	int			 pixel_format;                                                               /* The pixel format of the buffer; e.g. CA_YUYV422, CA_UYVY422, CA_JPEG_OPENDML, etc.. */

	int					mWidth;
	int					mHeight;
	qint64				mTimeStamp;
};

class VideoFrame
{
public:
	VideoFrame()
	{
		d = new VideoFrameData();
	}

#if defined( VIDEOCAPTURE_SUPPORTED )
	VideoFrame( int pWidth, int pHeight, qint64 pTimeStamp, const ca::PixelBuffer &pPixelBuffer )
	{
		d = new VideoFrameData( pWidth, pHeight, pTimeStamp, pPixelBuffer );
	}
#endif

	VideoFrame(const VideoFrame &other)
		: d (other.d)
	{
	}

	int width( void ) const
	{
		return( d->mWidth );
	}

	int height( void ) const
	{
		return( d->mHeight );
	}

	int pixelFormat( void ) const
	{
		return( d->pixel_format );
	}

	qint64 timestamp( void ) const
	{
		return( d->mTimeStamp );
	}

	size_t stride( int pIndex ) const
	{
		return( d->stride[ pIndex ] );
	}

	uint8_t *plane( int pIndex ) const
	{
		return( d->plane[ pIndex ] );
	}

private:
	QSharedDataPointer<VideoFrameData> d;
};

Q_DECLARE_METATYPE( VideoFrame )

class VideoCaptureWorker : public QThread
{
	Q_OBJECT

public:
	VideoCaptureWorker( int pDeviceIndex, int pFormatIndex )
		: mDeviceIndex( pDeviceIndex ), mFormatIndex( pFormatIndex )
	#if defined( VIDEOCAPTURE_SUPPORTED )
		, mCapture( &VideoCaptureWorker::frameCallbackStatic, this )
	#endif
	{

	}

	int deviceIndex( void ) const
	{
		return( mDeviceIndex );
	}

	int formatIndex( void ) const
	{
		return( mFormatIndex );
	}

	void run( void ) Q_DECL_OVERRIDE;

private:
#if defined( VIDEOCAPTURE_SUPPORTED )
	static void frameCallbackStatic( ca::PixelBuffer &pBuffer )
	{
		static_cast<VideoCaptureWorker *>( pBuffer.user )->frameCallback( pBuffer );
	}

	void frameCallback( ca::PixelBuffer &pBuffer )
	{
		emit frameReady( VideoFrame( mDevCap.width, mDevCap.height, QDateTime::currentMSecsSinceEpoch(), pBuffer ) );
	}
#endif

signals:
	void frameReady( VideoFrame pVideoFrame );

private:
	const int									 mDeviceIndex;
	const int									 mFormatIndex;

#if defined( VIDEOCAPTURE_SUPPORTED )
	ca::Capture									 mCapture;
	ca::PixelBuffer								 mPrvDat;
	ca::Capability								 mDevCap;
#endif
};

class VideoCaptureDevice : public QObject
{
	Q_OBJECT

public:
	explicit VideoCaptureDevice( int pDeviceIndex, int pFormatIndex, QObject *parent = nullptr );

	virtual ~VideoCaptureDevice( void );

	int deviceIndex( void ) const
	{
		return( mWorker->deviceIndex() );
	}

	int formatIndex( void ) const
	{
		return( mWorker->formatIndex() );
	}

	VideoFrame frame( void ) const
	{
		return( mVideoFrame );
	}

	qint64 timestamp( void ) const
	{
		return( mVideoFrame.timestamp() );
	}

signals:
	void frameUpdated( void );

protected slots:
	void handleFrame( VideoFrame pVideoFrame );

private:
	VideoCaptureWorker		*mWorker;
	VideoFrame				 mVideoFrame;
};

#endif // VIDEOCAPTUREDEVICE_H
