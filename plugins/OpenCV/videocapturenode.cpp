#include "videocapturenode.h"

#include "opencvplugin.h"

#include <fugio/global_interface.h>
#include "fugio/context_interface.h"
#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>
#include <fugio/context_signals.h>

#include <fugio/image/uuid.h>

#include <fugio/performance.h>

#include <QSpinBox>

#if defined( OPENCV_SUPPORTED )
#include <opencv2/imgproc/imgproc.hpp>
#endif

VideoCaptureNode::VideoCaptureNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mVidCapDev( nullptr ), mFrmNum( 0 ), mCameraIndex( -1 )
{
	if( ( mOutputImage = pinOutput<fugio::VariantInterface *>( "Output", mPinOutputImage, PID_IMAGE ) ) == 0 )
	{
		return;
	}

	mOutputSize = pinOutput<VariantInterface *>( "Size", mPinOutputSize, PID_SIZE );
}

VideoCaptureNode::~VideoCaptureNode()
{

}

bool VideoCaptureNode::initialise()
{
	setCameraIndex( 0 );

	return( true );
}

bool VideoCaptureNode::deinitialise()
{
	disconnect( mNode->context()->qobject(), SIGNAL(frameStart(qint64)), this, SLOT(onFrameStart(qint64)) );

	setCameraIndex( -1 );

	return( true );
}

void VideoCaptureNode::onFrameStart( qint64 pTimeStamp )
{
	Q_UNUSED( pTimeStamp )

#if defined( OPENCV_SUPPORTED )
	if( !mVidCapDev )
	{
		return;
	}

	if( mVidCapDev->frameNumber() == mFrmNum )
	{
		return;
	}

	fugio::Performance	P( mNode, "onFrameStart", pTimeStamp );

	mMatImg = mVidCapDev->frame();

	mFrmNum = mVidCapDev->frameNumber();

//	int		T = mMatImg.type();
//	int		C = mMatImg.channels();
//	int		G = CV_8UC3;

	fugio::Image		I = mOutputImage->variant().value<fugio::Image>();

	if( mOutputSize->variant().toSize() != QSize( mMatImg.cols, mMatImg.rows ) )
	{
		mOutputSize->setVariant( QSize( mMatImg.cols, mMatImg.rows ) );
	}

	I.setSize( mMatImg.cols, mMatImg.rows );
	I.setLineSize( 0, mMatImg.step );

//	switch( mMatImg.format() )
//	{
//		case fugio::ImageFormat::RGB8:
//		case fugio::ImageFormat::BGR8:
//			MatImg = cv::Mat( pSrcImg->size().height(), pSrcImg->size().width(), CV_8UC3, (void *)pSrcImg->buffer( 0 ), pSrcImg->lineSize( 0 ) );
//			break;

//		case fugio::ImageFormat::RGBA8:
//		case fugio::ImageFormat::BGRA8:
//			MatImg = cv::Mat( pSrcImg->size().height(), pSrcImg->size().width(), CV_8UC4, (void *)pSrcImg->buffer( 0 ), pSrcImg->lineSize( 0 ) );
//			break;

//		case fugio::ImageFormat::GRAY8:
//			MatImg = cv::Mat( pSrcImg->size().height(), pSrcImg->size().width(), CV_8U, (void *)pSrcImg->buffer( 0 ), pSrcImg->lineSize( 0 ) );
//			break;

//		case fugio::ImageFormat::GRAY16:
//			MatImg = cv::Mat( pSrcImg->size().height(), pSrcImg->size().width(), CV_16U, (void *)pSrcImg->buffer( 0 ), pSrcImg->lineSize( 0 ) );
//			break;

//		case fugio::ImageFormat::YUYV422:
//			MatImg = cv::Mat( pSrcImg->size().height(), pSrcImg->size().width(), CV_8UC2, (void *)pSrcImg->buffer( 0 ), pSrcImg->lineSize( 0 ) );
//			break;
//	}

	switch( mMatImg.type() )
	{
		case CV_8U:
			if( mMatImg.channels() == 1 )
			{
				I.setFormat( fugio::ImageFormat::GRAY8 );
			}
			else if( mMatImg.channels() == 3 )
			{
				I.setFormat( fugio::ImageFormat::BGR8 );
			}
			else if( mMatImg.channels() == 4 )
			{
				I.setFormat( fugio::ImageFormat::BGRA8 );
			}
			break;

		case CV_16U:
			I.setFormat( fugio::ImageFormat::GRAY16 );
			break;

		case CV_8UC3:
			I.setFormat( fugio::ImageFormat::BGR8 );
			break;
	}

	I.setBuffer( 0, mMatImg.data );

	pinUpdated( mPinOutputImage );
#endif
}

void VideoCaptureNode::setCameraIndex( int pIndex )
{
	if( pIndex == mCameraIndex )
	{
		return;
	}

#if defined( OPENCV_SUPPORTED )
	if( mVidCapDev )
	{
		DeviceVideoCapture	*TmpVidCapDev = mVidCapDev;

		mVidCapDev = 0;

		DeviceVideoCapture::delDevice( TmpVidCapDev );

		mCameraIndex = -1;

		disconnect( mNode->context()->qobject(), SIGNAL(frameStart(qint64)), this, SLOT(onFrameStart(qint64)) );
	}
#endif

	if( pIndex < 0 )
	{
		return;
	}

#if defined( OPENCV_SUPPORTED )
	if( ( mVidCapDev = DeviceVideoCapture::newDevice( pIndex ) ) )
	{
		mVidCapDev->start();

		mCameraIndex = pIndex;

		connect( mNode->context()->qobject(), SIGNAL(frameStart(qint64)), this, SLOT(onFrameStart(qint64)) );
	}
#endif
}

QWidget *VideoCaptureNode::gui()
{
	QSpinBox		*SpinBox = new QSpinBox();

	SpinBox->setMinimum( 0 );
	SpinBox->setMaximum( 10 );

	connect( SpinBox, SIGNAL(valueChanged(int)), this, SLOT(setCameraIndex(int)) );

	return( SpinBox );
}
