#include "backgroundsubtractionnode.h"

#include <QtConcurrent/QtConcurrentRun>

#include <fugio/image/uuid.h>

#include <fugio/image/image_interface.h>

#include "opencvplugin.h"


BackgroundSubtractionNode::BackgroundSubtractionNode( QSharedPointer<fugio::NodeInterface> pNode )
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

void BackgroundSubtractionNode::inputsUpdated( qint64 pTimeStamp )
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
	mNode->context()->futureSync( QtConcurrent::run( &BackgroundSubtractionNode::process, this ) );
#endif
}

void BackgroundSubtractionNode::process( BackgroundSubtractionNode *pNode )
{
	pNode->process();
}

void BackgroundSubtractionNode::process( void )
{
#if defined( OPENCV_SUPPORTED )
	fugio::ImageInterface				*SrcImg = input<fugio::ImageInterface *>( mPinInputImage );

	cv::Mat						 MatSrc = OpenCVPlugin::image2mat( SrcImg );

	if( !mBckSub )
	{
#if ( ( defined( CV_VERSION_EPOCH ) && ( CV_VERSION_EPOCH > 2 ) ) || ( !defined( CV_VERSION_EPOCH ) && CV_VERSION_MAJOR >= 3 ) )
		mBckSub = cv::createBackgroundSubtractorMOG2();
#else
		mBckSub = new cv::BackgroundSubtractorMOG2();
#endif
	}

#if ( ( defined( CV_VERSION_EPOCH ) && ( CV_VERSION_EPOCH > 2 ) ) || ( !defined( CV_VERSION_EPOCH ) && CV_VERSION_MAJOR >= 3 ) )
	mBckSub->apply( MatSrc, mMatDst );
#else
	mBckSub->operator()( MatSrc, mMatDst );
#endif

	OpenCVPlugin::mat2image( mMatDst, mOutputImage );

	pinUpdated( mPinOutputImage );
#endif
}
