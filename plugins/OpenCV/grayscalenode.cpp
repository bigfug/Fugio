#include "grayscalenode.h"

#include <QtConcurrent/QtConcurrentRun>

#include "opencvplugin.h"

#include <fugio/global_interface.h>
#include "fugio/context_interface.h"
#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>

#include <fugio/image/uuid.h>

#if defined( OPENCV_SUPPORTED )
#include <opencv2/imgproc/imgproc.hpp>
#endif

GrayscaleNode::GrayscaleNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	if( ( mPinInputImage = pinInput( "Input" ) ) == 0 )
	{
		return;
	}

	if( ( mOutputImage = pinOutput<fugio::ImageInterface *>( "Output", mPinOutputImage, PID_IMAGE ) ) == 0 )
	{
		return;
	}
}

GrayscaleNode::~GrayscaleNode()
{

}

void GrayscaleNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	if( !mPinInputImage->isConnected() )
	{
		return;
	}

	fugio::ImageInterface			*SrcImg = input<fugio::ImageInterface *>( mPinInputImage );

	if( !SrcImg || SrcImg->size().isEmpty() )
	{
		return;
	}

#if defined( OPENCV_SUPPORTED )
	mNode->context()->futureSync( QtConcurrent::run( &GrayscaleNode::conversion, this ) );
#endif
}

void GrayscaleNode::conversion( GrayscaleNode *pNode )
{
#if defined( OPENCV_SUPPORTED )
	fugio::ImageInterface				*SrcImg = pNode->input<fugio::ImageInterface *>( pNode->mPinInputImage );

	cv::Mat						 MatSrc = OpenCVPlugin::image2mat( SrcImg );

	fugio::ImageInterface::Format		 DstFmt = fugio::ImageInterface::FORMAT_GRAY8;

	switch( SrcImg->format() )
	{
		case fugio::ImageInterface::FORMAT_RGB8:
			cv::cvtColor( MatSrc, pNode->mMatImg, CV_RGB2GRAY );
			break;

		case fugio::ImageInterface::FORMAT_BGR8:
			cv::cvtColor( MatSrc, pNode->mMatImg, CV_BGR2GRAY );
			break;

		case fugio::ImageInterface::FORMAT_RGBA8:
			cv::cvtColor( MatSrc, pNode->mMatImg, CV_RGBA2GRAY );
			break;

		case fugio::ImageInterface::FORMAT_BGRA8:
			cv::cvtColor( MatSrc, pNode->mMatImg, CV_BGRA2GRAY );
			break;

//		case fugio::ImageInterface::FORMAT_GRAY16:
//			break;

//		case fugio::ImageInterface::FORMAT_YUYV422:
//			cv::cvtColor( MatSrc, mMatImg, CV_YUV2GRAY_420 );
//			break;

		default:
			return;
	}

	OpenCVPlugin::mat2image( pNode->mMatImg, pNode->mOutputImage, DstFmt );

	pNode->pinUpdated( pNode->mPinOutputImage );
#endif
}
