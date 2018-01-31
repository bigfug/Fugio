#include "imagethresholdnode.h"

#include <fugio/global_interface.h>
#include "fugio/context_interface.h"
#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>

#include "opencvplugin.h"

#include <fugio/image/uuid.h>

#if defined( OPENCV_SUPPORTED )
#include <opencv2/imgproc/imgproc.hpp>
#endif

QMap<QString,int>	ImageThresholdNode::mTypeList =
{
#if defined( OPENCV_SUPPORTED )
	{ "THRESH_BINARY",		cv::THRESH_BINARY },
	{ "THRESH_BINARY_INV",	cv::THRESH_BINARY_INV },
	{ "THRESH_TRUNC",		cv::THRESH_TRUNC },
	{ "THRESH_TOZERO",		cv::THRESH_TOZERO },
	{ "THRESH_TOZERO_INV",	cv::THRESH_TOZERO_INV }
#endif
};

ImageThresholdNode::ImageThresholdNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_IMAGE, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_INPUT_THRESHOLD, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );
	FUGID( PIN_INPUT_MAXVAL, "261cc653-d7fa-4c34-a08b-3603e8ae71d5" );
	FUGID( PIN_INPUT_TYPE, "249f2932-f483-422f-b811-ab679f006381" );
	FUGID( PIN_OUTPUT_IMAGE, "ce8d578e-c5a4-422f-b3c4-a1bdf40facdb" );

	mPinInputImage = pinInput( "Input", PIN_INPUT_IMAGE );
	mPinInputThreshold = pinInput( "Threshold", PIN_INPUT_THRESHOLD );
	mPinInputMaxVal = pinInput( "MaxVal", PIN_INPUT_MAXVAL );

	mValInputType = pinInput<fugio::ChoiceInterface *>( "Type", mPinInputType, PID_CHOICE, PIN_INPUT_TYPE );

	mValInputType->setChoices( mTypeList.keys() );

	mValOutputImage = pinOutput<fugio::VariantInterface *>( "Output", mPinOutputImage, PID_IMAGE, PIN_OUTPUT_IMAGE );

	mPinInputThreshold->setValue( "THRESH_BINARY" );
	mPinInputThreshold->setValue( 128 );
	mPinInputMaxVal->setValue( 255 );
}

void ImageThresholdNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	fugio::Image		SrcImg = variant<fugio::Image>( mPinInputImage );

	if( SrcImg.isEmpty() )
	{
		return;
	}

#if defined( OPENCV_SUPPORTED )
	cv::Mat		MatSrc = OpenCVPlugin::image2mat( SrcImg );

	double		Threshold = variant( mPinInputThreshold ).toDouble();
	double		MaxVal    = variant( mPinInputMaxVal ).toDouble();
	int			Type      = mTypeList.value( variant( mPinInputType ).toString() );

	try
	{
		cv::threshold( MatSrc, mMatImg, Threshold, MaxVal, Type );

		fugio::Image	DstImg = mValOutputImage->variant().value<fugio::Image>();

		OpenCVPlugin::mat2image( mMatImg, DstImg );

		pinUpdated( mPinOutputImage );
	}
	catch( ... )
	{
		mNode->setStatus( fugio::NodeInterface::Error );
	}

#endif
}
