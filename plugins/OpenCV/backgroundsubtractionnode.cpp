#include "backgroundsubtractionnode.h"

#include <QtConcurrent/QtConcurrentRun>

#include <fugio/image/uuid.h>

#include <fugio/image/image_interface.h>

#include "opencvplugin.h"


BackgroundSubtractionNode::BackgroundSubtractionNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_IMAGE, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_INPUT_RESET, "261cc653-d7fa-4c34-a08b-3603e8ae71d5" );
	FUGID( PIN_OUTPUT_IMAGE, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );

	if( ( mPinInputImage = pinInput( "Input", PIN_INPUT_IMAGE ) ) == 0 )
	{
		return;
	}

	mPinInputReset = pinInput( "Reset", PIN_INPUT_RESET );

	if( ( mOutputImage = pinOutput<fugio::ImageInterface *>( "Output", mPinOutputImage, PID_IMAGE, PIN_OUTPUT_IMAGE ) ) == 0 )
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

	if( mPinInputReset->isUpdated( pTimeStamp ) )
	{
		mLearningRate = 1;
	}
	else
	{
		mLearningRate = -1;
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
	fugio::ImageInterface		*SrcImg = input<fugio::ImageInterface *>( mPinInputImage );

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
	mBckSub->apply( MatSrc, mMatDst, mLearningRate );
#else
	mBckSub->operator()( MatSrc, mMatDst, mLearningRate );
#endif

	OpenCVPlugin::mat2image( mMatDst, mOutputImage );

	pinUpdated( mPinOutputImage );
#endif
}
