#include "distancetransformnode.h"

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

DistanceTransformNode::DistanceTransformNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_IMAGE, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_OUTPUT_IMAGE, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );
	FUGID( PIN_OUTPUT_LABELS, "261cc653-d7fa-4c34-a08b-3603e8ae71d5" );
	//FUGID( PIN_XXX_XXX, "249f2932-f483-422f-b811-ab679f006381" );
	//FUGID( PIN_XXX_XXX, "ce8d578e-c5a4-422f-b3c4-a1bdf40facdb" );
	//FUGID( PIN_XXX_XXX, "e6bf944e-5f46-4994-bd51-13c2aa6415b7" );
	//FUGID( PIN_XXX_XXX, "a2bbf374-0dc8-42cb-b85a-6a43b58a348f" );

	mPinInputImage = pinInput( "Image", PIN_INPUT_IMAGE );

	mPinInputImage->registerPinInputType( PID_IMAGE );

	mValOutputImage = pinOutput<fugio::VariantInterface *>( "Image", mPinOutputImage, PID_IMAGE, PIN_OUTPUT_IMAGE );

	mValOutputLabels = pinOutput<fugio::VariantInterface *>( "Labels", mPinOutputLabels, PID_IMAGE, PIN_OUTPUT_LABELS );
}

void DistanceTransformNode::inputsUpdated( qint64 pTimeStamp )
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
		mNode->context()->futureSync( QtConcurrent::run( &DistanceTransformNode::conversion, this ) );
	}
	else
	{
		conversion( this );
	}
#endif
}

void DistanceTransformNode::conversion( DistanceTransformNode *pNode )
{
#if defined( OPENCV_SUPPORTED )
	fugio::Image	SrcImg = variantStatic<fugio::Image>( pNode->mPinInputImage );

	cv::Mat			MatSrc = OpenCVPlugin::image2mat( SrcImg );

	try
	{
#if ( ( defined( CV_VERSION_EPOCH ) && ( CV_VERSION_EPOCH > 2 ) ) || ( !defined( CV_VERSION_EPOCH ) && CV_VERSION_MAJOR >= 3 ) )
		cv::distanceTransform( MatSrc, pNode->mMatImg, pNode->mMatLab, cv::DIST_L2, 5, cv::DIST_LABEL_CCOMP );
#else
		cv::distanceTransform( MatSrc, pNode->mMatImg, pNode->mMatLab, CV_DIST_L2, 5, cv::DIST_LABEL_CCOMP );
#endif

		fugio::Image	DstImg = pNode->mValOutputImage->variant().value<fugio::Image>();
		fugio::Image	DstLab = pNode->mValOutputLabels->variant().value<fugio::Image>();

		OpenCVPlugin::mat2image( pNode->mMatImg, DstImg );
		OpenCVPlugin::mat2image( pNode->mMatLab, DstLab );

		pNode->pinUpdated( pNode->mPinOutputImage );

		pNode->pinUpdated( pNode->mPinOutputLabels );

		pNode->mNode->setStatus( fugio::NodeInterface::Initialised );
	}
	catch( ... )
	{
		pNode->mNode->setStatus( fugio::NodeInterface::Error );
	}

#endif
}
