#include "erodenode.h"

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

ErodeNode::ErodeNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_IMAGE, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_INPUT_ITERATIONS, "261cc653-d7fa-4c34-a08b-3603e8ae71d5" );
	FUGID( PIN_OUTPUT_IMAGE, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );

	mPinInputImage = pinInput( "Image", PIN_INPUT_IMAGE );

	mPinInputImage->registerPinInputType( PID_IMAGE );

	mPinInputIterations = pinInput( "Iterations", PIN_INPUT_ITERATIONS );

	mPinInputIterations->setValue( 1 );

	mValOutputImage = pinOutput<fugio::VariantInterface *>( "Image", mPinOutputImage, PID_IMAGE, PIN_OUTPUT_IMAGE );
}

void ErodeNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	if( !mPinInputImage->isConnected() )
	{
		return;
	}

	fugio::Image	SrcImg = variant<fugio::Image>( mPinInputImage );

	if( !SrcImg.isValid() )
	{
		return;
	}

#if defined( OPENCV_SUPPORTED )
	if( false )
	{
		mNode->context()->futureSync( QtConcurrent::run( &ErodeNode::conversion, this ) );
	}
	else
	{
		conversion( this );
	}
#endif
}

void ErodeNode::conversion( ErodeNode *pNode )
{
#if defined( OPENCV_SUPPORTED )
	fugio::Image	SrcImg = variantStatic<fugio::Image>( pNode->mPinInputImage );

	cv::Mat			MatSrc = OpenCVPlugin::image2mat( SrcImg );

	cv::erode( MatSrc, pNode->mMatImg, cv::Mat(), cv::Point( -1, -1 ), pNode->variant( pNode->mPinInputIterations ).toReal() );

	fugio::Image	DstImg = pNode->mValOutputImage->variant().value<fugio::Image>();

	OpenCVPlugin::mat2image( pNode->mMatImg, DstImg );

	pNode->pinUpdated( pNode->mPinOutputImage );
#endif
}

