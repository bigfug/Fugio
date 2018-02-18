#include "videocapturenode.h"

#include <QPushButton>

#include <fugio/core/uuid.h>
#include <fugio/image/uuid.h>

#include <fugio/context_signals.h>

#include <fugio/performance.h>

#include "devicedialog.h"

VideoCaptureNode::VideoCaptureNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mDeviceIndex( -1 ), mFormatIndex( -1 )
#if defined( VIDEOCAPTURE_SUPPORTED )
	 , mCapture( &VideoCaptureNode::frameCallbackStatic, this )
#endif
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
#if defined( VIDEOCAPTURE_SUPPORTED )
	mCapture.stop();

	disconnect( mNode->context()->qobject(), SIGNAL(frameStart(qint64)), this, SLOT(frameStart(qint64)) );

	mCapture.close();
#endif

	return( NodeControlBase::deinitialise() );
}

void VideoCaptureNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );
}

#if defined( VIDEOCAPTURE_SUPPORTED )
void VideoCaptureNode::frameCallbackStatic( ca::PixelBuffer &pBuffer )
{
	static_cast<VideoCaptureNode *>( pBuffer.user )->frameCallback( pBuffer );
}

void VideoCaptureNode::frameCallback( ca::PixelBuffer &pBuffer )
{
	if( !mNode->context()->active() )
	{
		return;
	}

	fugio::Performance	P( mNode, __FUNCTION__, mNode->context()->global()->timestamp() );

	ca::PixelBuffer TmpBuf = pBuffer;

	for( int i = 0 ; i < 3 ; i++ )
	{
		TmpBuf.plane[ i ] = 0;
	}

	TmpBuf.pixels = 0;
	TmpBuf.user = 0;

	fugio::Image	Output = mValOutputImage->variant().value<fugio::Image>();

	if( memcmp( &TmpBuf, &mPrvDat, sizeof( ca::PixelBuffer ) ) != 0 )
	{
		mPrvDat = TmpBuf;

		Output.setSize( pBuffer.width[ 0 ], pBuffer.height[ 0 ] );

		switch( pBuffer.pixel_format )
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
				Output.setFormat( fugio::ImageFormat::RGB8 );
				break;
		}

		for( int i = 0 ; i < 3 ; i++ )
		{
			Output.setLineSize( i, pBuffer.stride[ i ] );
		}
	}

	if( !Output.isValid() )
	{
		return;
	}

	for( int i = 0 ; i < 3 ; i++ )
	{
		const uint8_t		*SrcPtr = pBuffer.plane[ i ];
		uint8_t				*DstPtr = Output.internalBuffer( i );

		if( SrcPtr && DstPtr )
		{
			memcpy( DstPtr, SrcPtr, pBuffer.stride[ i ] * pBuffer.height[ i ] );
		}
	}

	pinUpdated( mPinOutputImage );
}

void VideoCaptureNode::frameStart( qint64 pTimeStamp )
{
	fugio::Performance	P( mNode, __FUNCTION__, pTimeStamp );

	mCapture.update();
}
#endif

void VideoCaptureNode::setCurrentDevice( int pDevIdx, int pCfgIdx )
{
	if( mDeviceIndex == pDevIdx && mFormatIndex == pCfgIdx )
	{
		return;
	}

#if defined( VIDEOCAPTURE_SUPPORTED )
	disconnect( mNode->context()->qobject(), SIGNAL(frameStart(qint64)), this, SLOT(frameStart(qint64)) );

	mCapture.stop();
	mCapture.close();

	mDeviceIndex = pDevIdx;
	mFormatIndex = pCfgIdx;

	if( mDeviceIndex < 0 || mFormatIndex < 0 )
	{
		return;
	}

	std::vector<ca::Capability>	CapLst = mCapture.getCapabilities( mDeviceIndex );

	if( mFormatIndex < 0 || mFormatIndex >= CapLst.size() )
	{
		return;
	}

	ca::Capability		DevCap = CapLst.at( mFormatIndex );

	ca::Settings		DevCfg;

	DevCfg.device     = mDeviceIndex;
	DevCfg.capability = mFormatIndex;
	DevCfg.format     = DevCap.pixel_format;

	switch( DevCap.pixel_format )
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
		mNode->setStatus( fugio::NodeInterface::Error );

		return;
	}

	DevErr = mCapture.start();

	if( DevErr >= 0 )
	{
		mNode->setStatus( fugio::NodeInterface::Initialised );

		connect( mNode->context()->qobject(), SIGNAL(frameStart(qint64)), this, SLOT(frameStart(qint64)) );
	}
#endif
}

void VideoCaptureNode::chooseDevice()
{
#if defined( VIDEOCAPTURE_SUPPORTED )
	DeviceDialog		DD( mCapture, mDeviceIndex, mFormatIndex );

	if( DD.exec() == QDialog::Accepted )
	{
		setCurrentDevice( DD.deviceIdx(), DD.formatIdx() );
	}
#endif
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
