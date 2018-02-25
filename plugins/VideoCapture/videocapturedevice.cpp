#include "videocapturedevice.h"

#include <fugio/global_interface.h>

#include "videocaptureplugin.h"

VideoCaptureDevice::VideoCaptureDevice( int pDeviceIndex, int pFormatIndex, QObject *parent )
	: QObject( parent )
{
	qRegisterMetaType<VideoFrame>( "VideoFrame" );

	mWorker = new VideoCaptureWorker( pDeviceIndex, pFormatIndex );

//	connect( mWorker, &QThread::finished, mWorker, &QObject::deleteLater );

	connect( mWorker, &VideoCaptureWorker::frameReady, this, &VideoCaptureDevice::handleFrame );

	mWorker->start();
}

VideoCaptureDevice::~VideoCaptureDevice()
{
	mWorker->requestInterruption();
	mWorker->wait();

	delete mWorker;
}

void VideoCaptureWorker::run()
{
#if defined( VIDEOCAPTURE_SUPPORTED )
	std::vector<ca::Capability>	CapLst = mCapture.getCapabilities( mDeviceIndex );

	if( mFormatIndex < 0 || mFormatIndex >= CapLst.size() )
	{
		return;
	}

	mDevCap = CapLst.at( mFormatIndex );

	ca::Settings		DevCfg;

	DevCfg.device     = mDeviceIndex;
	DevCfg.capability = mFormatIndex;
	DevCfg.format     = mDevCap.pixel_format;

	switch( mDevCap.pixel_format )
	{
		case CA_JPEG_OPENDML:                                                          /* JPEG with Open-DML extensions */
		case CA_H264:                                                                  /* H264 */
		case CA_MJPEG:                                                                /* MJPEG 2*/

		case CA_YUV422P:                                                             /* YUV422 Planar */
		case CA_YUV420BP:                                                            /* YUV420 Bi Planar */
		case CA_YUVJ420P:                                                          /* YUV420 Planar Full Range (JPEG), J comes from the JPEG. (values 0-255 used) */
		case CA_YUVJ420BP:                                                          /* YUV420 Bi-Planer Full Range (JPEG), J comes fro the JPEG. (values: luma = [16,235], chroma=[16,240]) */
			{
				DevCfg.format = CA_YUYV422;
			}
			break;

		default:
			break;

	}

	int DevErr = mCapture.open( DevCfg );

	if( DevErr < 0 )
	{
		return;
	}

	DevErr = mCapture.start();

	if( DevErr < 0 )
	{
		return;
	}

	while( !isInterruptionRequested() )
	{
		mCapture.update();
	}

	mCapture.stop();
	mCapture.close();
#endif
}

void VideoCaptureDevice::handleFrame( VideoFrame pVideoFrame )
{
	mVideoFrame = pVideoFrame;

	VideoCapturePlugin::instance()->app()->scheduleFrame();

	emit frameUpdated();
}
