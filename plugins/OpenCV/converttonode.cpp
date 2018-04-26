#include "converttonode.h"

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

QMap<QString,int>					 ConvertToNode::mTypeMap =
{
#if defined( OPENCV_SUPPORTED )
	{ "CV_8S", CV_8S },
	{ "CV_8U", CV_8U },
	{ "CV_16S", CV_16S },
	{ "CV_16U", CV_16U },
	{ "CV_32F", CV_32F },
	{ "CV_32S", CV_32S }
#endif
};

ConvertToNode::ConvertToNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_IMAGE, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_INPUT_TYPE, "249f2932-f483-422f-b811-ab679f006381" );
	FUGID( PIN_OUTPUT_IMAGE, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );
	//FUGID( PIN_XXX_XXX, "ce8d578e-c5a4-422f-b3c4-a1bdf40facdb" );
	//FUGID( PIN_XXX_XXX, "e6bf944e-5f46-4994-bd51-13c2aa6415b7" );
	//FUGID( PIN_XXX_XXX, "a2bbf374-0dc8-42cb-b85a-6a43b58a348f" );

	mPinInputImage = pinInput( "Image", PIN_INPUT_IMAGE );

	mPinInputImage->registerPinInputType( PID_IMAGE );

	mValInputType = pinInput<fugio::ChoiceInterface *>( "Type", mPinInputType, PID_CHOICE, PIN_INPUT_TYPE );

	mValInputType->setChoices( mTypeMap.keys() );

	mValOutputImage = pinOutput<fugio::VariantInterface *>( "Image", mPinOutputImage, PID_IMAGE, PIN_OUTPUT_IMAGE );
}

void ConvertToNode::inputsUpdated( qint64 pTimeStamp )
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
	int								 Type = mTypeMap.value( variant( mPinInputType ).toString() );

	cv::Mat							 MatSrc = OpenCVPlugin::image2mat( SrcImg );

	try
	{
		MatSrc.convertTo( mMatImg, Type );
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
