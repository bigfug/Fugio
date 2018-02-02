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

	if( ( mValOutputImage = pinOutput<fugio::VariantInterface *>( "Output", mPinOutputImage, PID_IMAGE ) ) == 0 )
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

	fugio::Image	SrcImg = variant<fugio::Image>( mPinInputImage );

	if( !SrcImg.isValid() )
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
	fugio::Image			SrcImg = variantStatic<fugio::Image>( pNode->mPinInputImage );

	cv::Mat					MatSrc = OpenCVPlugin::image2mat( SrcImg );

	fugio::ImageFormat		DstFmt = fugio::ImageFormat::GRAY8;

	switch( SrcImg.format() )
	{
		case fugio::ImageFormat::RGB8:
			cv::cvtColor( MatSrc, pNode->mMatImg, CV_RGB2GRAY );
			break;

		case fugio::ImageFormat::BGR8:
			cv::cvtColor( MatSrc, pNode->mMatImg, CV_BGR2GRAY );
			break;

		case fugio::ImageFormat::RGBA8:
			cv::cvtColor( MatSrc, pNode->mMatImg, CV_RGBA2GRAY );
			break;

		case fugio::ImageFormat::BGRA8:
			cv::cvtColor( MatSrc, pNode->mMatImg, CV_BGRA2GRAY );
			break;

//		case fugio::ImageFormat::GRAY16:
//			break;

//		case fugio::ImageFormat::YUYV422:
//			cv::cvtColor( MatSrc, mMatImg, CV_YUV2GRAY_420 );
//			break;

		default:
			return;
	}

	fugio::Image	DstImg = pNode->mValOutputImage->variant().value<fugio::Image>();

	OpenCVPlugin::mat2image( pNode->mMatImg, DstImg );

	pNode->pinUpdated( pNode->mPinOutputImage );
#endif
}
