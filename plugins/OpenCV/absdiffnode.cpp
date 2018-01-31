#include "absdiffnode.h"

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

AbsDiffNode::AbsDiffNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_IMAGE1, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_INPUT_IMAGE2, "261cc653-d7fa-4c34-a08b-3603e8ae71d5" );
	FUGID( PIN_OUTPUT_IMAGE, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );

	mPinInputImage1 = pinInput( "Image1", PIN_INPUT_IMAGE1 );

	mPinInputImage1->registerPinInputType( PID_IMAGE );

	mPinInputImage2 = pinInput( "Image2", PIN_INPUT_IMAGE2 );

	mPinInputImage2->registerPinInputType( PID_IMAGE );

	mValOutputImage = pinOutput<fugio::VariantInterface *>( "Image", mPinOutputImage, PID_IMAGE, PIN_OUTPUT_IMAGE );
}

void AbsDiffNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	if( !mPinInputImage1->isConnected() || !mPinInputImage2->isConnected() )
	{
		return;
	}

	fugio::Image				SrcImg1 = variant( mPinInputImage1 ).value<fugio::Image>();

	if( !SrcImg1.isEmpty() )
	{
		return;
	}

	fugio::Image				SrcImg2 = variant( mPinInputImage2 ).value<fugio::Image>();

	if( !SrcImg2.isEmpty() )
	{
		return;
	}

#if defined( OPENCV_SUPPORTED )
	cv::Mat						 MatSrc1 = OpenCVPlugin::image2mat( SrcImg1 );
	cv::Mat						 MatSrc2 = OpenCVPlugin::image2mat( SrcImg2 );

	try
	{
		cv::absdiff( MatSrc1, MatSrc2, mMatImg );

		fugio::Image	DstImg = mValOutputImage->variant().value<fugio::Image>();

		OpenCVPlugin::mat2image( mMatImg, DstImg );

		pinUpdated( mPinOutputImage );
	}
	catch( ... )
	{

	}
#endif
}
