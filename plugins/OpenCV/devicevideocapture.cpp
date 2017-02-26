#include "devicevideocapture.h"

#include <QMutexLocker>

QVector<DeviceVideoCapture *>	 DeviceVideoCapture::mDeviceList;

void DeviceVideoCapture::deviceInitialise()
{
	mDeviceList.resize( 10 );
}

void DeviceVideoCapture::deviceDeinitialise()
{

}

DeviceVideoCapture *DeviceVideoCapture::newDevice( int pCameraIndex )
{
	if( !mDeviceList[ pCameraIndex ] )
	{
		mDeviceList[ pCameraIndex ] = new DeviceVideoCapture( pCameraIndex );
	}

	return( mDeviceList[ pCameraIndex ] );
}

void DeviceVideoCapture::delDevice( DeviceVideoCapture *pDelDev )
{
	mDeviceList[ pDelDev->cameraIndex() ] = 0;

	pDelDev->shutdown();

	pDelDev->deleteLater();
}

//-----------------------------------------------------------------------------

DeviceVideoCapture::DeviceVideoCapture( int pCameraIndex )
	: mCameraIndex( pCameraIndex ), mFrmNum( 0 )
{
}

DeviceVideoCapture::~DeviceVideoCapture()
{
}

void DeviceVideoCapture::shutdown()
{
	requestInterruption();

	while( isRunning() )
	{
		mFrmMtx.lock();

		wait( 2000 );

		mFrmMtx.unlock();
	}
}

#if defined( OPENCV_SUPPORTED )
cv::Mat DeviceVideoCapture::lockFrame()
{
	mFrmMtx.lock();

	return( mMatImg );
}

void DeviceVideoCapture::freeFrame()
{
	mFrmMtx.unlock();
}

cv::Mat DeviceVideoCapture::frame()
{
	QMutexLocker	LCK( &mFrmMtx );

	return( mMatImg.clone() );
}
#endif

void DeviceVideoCapture::run()
{
	try
	{
#if defined( OPENCV_SUPPORTED )
		if( ( mVidCap = new cv::VideoCapture( mCameraIndex ) ) == nullptr )
		{
			return;
		}

		if( !mVidCap->isOpened() )
		{
			return;
		}

		while( !isInterruptionRequested() && mVidCap->grab() )
		{
			mFrmMtx.lock();

			if( mVidCap->retrieve( mMatImg ) )
			{
				mFrmNum++;
			}

			mFrmMtx.unlock();
		}

		delete mVidCap;

		mVidCap = 0;
#endif
	}
	catch( ... )
	{

	}
}
