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

	}

#if defined( VIDEOCAPTURE_SUPPORTED )
	VideoFrameData( int pWidth, int pHeight, qint64 pTimeStamp, const ca::PixelBuffer &pPixelBuffer )
		: mWidth( pWidth ), mHeight( pHeight ), mTimeStamp( pTimeStamp )
	{
		mPixelBuffer.setup( pWidth, pHeight, pPixelBuffer.pixel_format );

		for( int i = 0 ; i < 3 ; i++ )
		{
			if( !mPixelBuffer.plane[ i ] || !pPixelBuffer.plane[ i ] )
			{
				continue;
			}

			memcpy( mPixelBuffer.plane[ i ], pPixelBuffer.plane[ i ], mPixelBuffer.stride[ i ] * mPixelBuffer.height[ i ] );
		}
	}
#endif

	VideoFrameData( const VideoFrameData &other )
		: QSharedData( other ), mWidth( other.mWidth ), mHeight( other.mHeight ), mTimeStamp( other.mTimeStamp )
	{
#if defined( VIDEOCAPTURE_SUPPORTED )
		mPixelBuffer.setup( mWidth, mHeight, other.mPixelBuffer.pixel_format );

		for( int i = 0 ; i < 3 ; i++ )
		{
			if( !mPixelBuffer.plane[ i ] || !other.mPixelBuffer.plane[ i ] )
			{
				continue;
			}

			memcpy( mPixelBuffer.plane[ i ], other.mPixelBuffer.plane[ i ], mPixelBuffer.stride[ i ] * mPixelBuffer.height[ i ] );
		}
#endif
	}

	~VideoFrameData()
	{
	}

public:
#if defined( VIDEOCAPTURE_SUPPORTED )
	ca::PixelBuffer		mPixelBuffer;
#endif

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
#if defined( VIDEOCAPTURE_SUPPORTED )
		return( d->mPixelBuffer.pixel_format );
#else
		return( -1 );
#endif
	}

	qint64 timestamp( void ) const
	{
		return( d->mTimeStamp );
	}

	const ca::PixelBuffer &pixelBuffer( void ) const
	{
		return( d->mPixelBuffer );
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
