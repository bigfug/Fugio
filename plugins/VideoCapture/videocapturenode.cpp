#include "videocapturenode.h"

#include <QPushButton>

#include <fugio/core/uuid.h>
#include <fugio/image/uuid.h>

#include <fugio/context_signals.h>

#include <fugio/performance.h>

#include "devicedialog.h"

#include "videocaptureplugin.h"

VideoCaptureNode::VideoCaptureNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mDeviceIndex( -1 ), mFormatIndex( -1 ), mLastFrameTimeStamp( -1 )
{
	FUGID( PIN_OUTPUT_IMAGE, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );

	mValOutputImage = pinOutput<fugio::VariantInterface *>( "Image", mPinOutputImage, PID_IMAGE, PIN_OUTPUT_IMAGE );

#if defined( VIDEOCAPTURE_SUPPORTED )
	memset( &mPrvDat, 0, sizeof( ca::PixelBuffer ) );
#endif
}

bool VideoCaptureNode::initialise( void )
{
	if( !NodeControlBase::initialise() )
	{
		return( false );
	}

	if( mDeviceIndex < 0 || mFormatIndex < 0 )
	{
		setCurrentDevice( qMax( mDeviceIndex, 0 ), qMax( mFormatIndex, 0 ) );
	}

	return( true );
}

bool VideoCaptureNode::deinitialise( void )
{
	mDevice.clear();

	return( NodeControlBase::deinitialise() );
}

void VideoCaptureNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

//	if( !mNode->context()->active() )
//	{
//		return;
//	}

	if( mDevice && mDevice->timestamp() > mLastFrameTimeStamp )
	{
		fugio::Performance	P( mNode, __FUNCTION__, pTimeStamp );

#if defined( VIDEOCAPTURE_SUPPORTED )
		fugio::Image				Output = mValOutputImage->variant().value<fugio::Image>();

		for( int i = 0 ; i < 3 ; i++ )
		{
			Output.setBuffer( i, nullptr );
		}

		mVideoFrame = mDevice->frame();

		ca::PixelBuffer				TF = mVideoFrame.pixelBuffer();

		for( int i = 0 ; i < 3 ; i++ )
		{
			TF.plane[ i ] = 0;
		}

		TF.pixels = 0;
		TF.user   = 0;

		if( memcmp( &TF, &mPrvDat, sizeof( ca::PixelBuffer ) ) != 0 )
		{
			mPrvDat = TF;

			Output.setSize( mVideoFrame.width(), mVideoFrame.height() );

			switch( TF.pixel_format )
			{
				case CA_YUV422P:                                                             /* YUV422 Planar */
				case CA_YUVJ420P:                                                          /* YUV420 Planar Full Range (JPEG), J comes from the JPEG. (values 0-255 used) */
				case CA_YUVJ420BP:                                                          /* YUV420 Bi-Planer Full Range (JPEG), J comes fro the JPEG. (values: luma = [16,235], chroma=[16,240]) */
				case CA_JPEG_OPENDML:                                                          /* JPEG with Open-DML extensions */
				case CA_H264:                                                                  /* H264 */
					return;

				case CA_YUV420BP:                                                            /* YUV420 Bi Planar */
					Output.setFormat( fugio::ImageFormat::NV12 );
					break;

				case CA_MJPEG:                                                                /* MJPEG 2*/
					Output.setFormat( fugio::ImageFormat::YUVJ422P );
					break;

				case CA_YUV420P:                                                           /* YUV420 Planar */
					Output.setFormat( fugio::ImageFormat::YUV420P );
					break;

				case CA_UYVY422:                                                              /* Cb Y0 Cr Y1 */
					Output.setFormat( fugio::ImageFormat::UYVY422 );
					break;

				case CA_YUYV422:                                                             /* Y0 Cb Y1 Cr */
					Output.setFormat( fugio::ImageFormat::YUYV422 );
					break;

				case CA_ARGB32:                                                              /* ARGB 8:8:8:8 32bpp, ARGBARGBARGB... */
				case CA_RGBA32:                                                              /* RGBA 8:8:8:8 32bpp. */
					Output.setFormat( fugio::ImageFormat::RGBA8 );
					break;

				case CA_BGRA32:                                                             /* BGRA 8:8:8:8 32bpp, BGRABGRABGRA... */
					Output.setFormat( fugio::ImageFormat::BGRA8 );
					break;

				case CA_RGB24:                                                              /* RGB 8:8:8 24bit */
	#if defined( Q_OS_WIN )
					Output.setFormat( fugio::ImageFormat::BGR8 );
	#else
					Output.setFormat( fugio::ImageFormat::RGB8 );
	#endif
					break;
			}

			for( int i = 0 ; i < 3 ; i++ )
			{
				Output.setLineSize( i, TF.stride[ i ] );
			}
		}

		if( Output.isValid() )
		{
			for( int i = 0 ; i < 3 ; i++ )
			{
				Output.setBuffer( i, TF.plane[ i ] );
			}

			pinUpdated( mPinOutputImage );
		}
#endif

		mLastFrameTimeStamp = mDevice->timestamp();
	}
}

void VideoCaptureNode::setCurrentDevice( int pDevIdx, int pCfgIdx )
{
	if( mDeviceIndex == pDevIdx && mFormatIndex == pCfgIdx )
	{
		return;
	}

	if( mDevice )
	{
		disconnect( mDevice.data(), &VideoCaptureDevice::frameUpdated, this, &VideoCaptureNode::frameUpdated );
	}

	mDevice.clear();

	mDeviceIndex = pDevIdx;
	mFormatIndex = pCfgIdx;

	mDevice = VideoCapturePlugin::instance()->device( mDeviceIndex, mFormatIndex );

	if( mDevice )
	{
		connect( mDevice.data(), &VideoCaptureDevice::frameUpdated, this, &VideoCaptureNode::frameUpdated );
	}
}

void VideoCaptureNode::chooseDevice()
{
#if defined( VIDEOCAPTURE_SUPPORTED )
	DeviceDialog		DD( mDeviceIndex, mFormatIndex );

	if( DD.exec() == QDialog::Accepted )
	{
		setCurrentDevice( DD.deviceIdx(), DD.formatIdx() );
	}
#endif
}

void VideoCaptureNode::frameUpdated()
{
	mNode->context()->updateNode( mNode );
}

QWidget *VideoCaptureNode::gui()
{
	QPushButton					*GUI = new QPushButton( "Select..." );

	connect( GUI, SIGNAL(released()), this, SLOT(chooseDevice()) );

	return( GUI );
}

void VideoCaptureNode::loadSettings( QSettings &pSettings )
{
	int	DevIdx = pSettings.value( "index", mDeviceIndex ).toInt();
	int	CfgIdx = pSettings.value( "format", mFormatIndex ).toInt();

	setCurrentDevice( DevIdx, CfgIdx );
}

void VideoCaptureNode::saveSettings( QSettings &pSettings ) const
{
	pSettings.setValue( "index", mDeviceIndex );
	pSettings.setValue( "format", mFormatIndex );
}
