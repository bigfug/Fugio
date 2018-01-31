#include "addnode.h"

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

AddNode::AddNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_IMAGE1, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_INPUT_IMAGE2, "ce8d578e-c5a4-422f-b3c4-a1bdf40facdb" );
	FUGID( PIN_OUTPUT_IMAGE, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );

	mPinInputImage1 = pinInput( "Image1", PIN_INPUT_IMAGE1 );

	mPinInputImage1->registerPinInputType( PID_IMAGE );

	mPinInputImage2 = pinInput( "Image2", PIN_INPUT_IMAGE2 );

	mPinInputImage2->registerPinInputType( PID_IMAGE );

	mValOutputImage = pinOutput<fugio::VariantInterface *>( "Image", mPinOutputImage, PID_IMAGE, PIN_OUTPUT_IMAGE );
}

void AddNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	fugio::Image	SrcImg1 = variant<fugio::Image>( mPinInputImage1 );

	if( !SrcImg1.isValid() )
	{
		return;
	}

	fugio::Image	SrcImg2 = variant<fugio::Image>( mPinInputImage2 );

	if( !SrcImg2.isValid() )
	{
		return;
	}

	if( SrcImg1.size() != SrcImg2.size() )
	{
		return;
	}

#if defined( OPENCV_SUPPORTED )
	cv::Mat							 MatSrc1 = OpenCVPlugin::image2mat( SrcImg1 );
	cv::Mat							 MatSrc2 = OpenCVPlugin::image2mat( SrcImg2 );

	try
	{
		cv::add( MatSrc1, MatSrc2, mMatImg );
	}
	catch( ... )
	{
		mNode->setStatus( fugio::NodeInterface::Error );

		return;
	}

	mNode->setStatus( fugio::NodeInterface::Initialised );

	fugio::Image	DstImg = mValOutputImage->variant().value<fugio::Image>();

	OpenCVPlugin::mat2image( mMatImg, DstImg );

	pinUpdated( mPinOutputImage );
#endif
}
