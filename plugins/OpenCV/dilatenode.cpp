#include "dilatenode.h"

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

DilateNode::DilateNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	mPinInputImage = pinInput( "Image" );

	mPinInputImage->registerPinInputType( PID_IMAGE );

	mOutputImage = pinOutput<fugio::ImageInterface *>( "Image", mPinOutputImage, PID_IMAGE );
}

void DilateNode::inputsUpdated( qint64 pTimeStamp )
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
	if( false )
	{
		mNode->context()->futureSync( QtConcurrent::run( &DilateNode::conversion, this ) );
	}
	else
	{
		conversion( this );
	}
#endif
}

void DilateNode::conversion( DilateNode *pNode )
{
#if defined( OPENCV_SUPPORTED )
	fugio::ImageInterface				*SrcImg = pNode->input<fugio::ImageInterface *>( pNode->mPinInputImage );

	cv::Mat						 MatSrc = OpenCVPlugin::image2mat( SrcImg );

	cv::dilate( MatSrc, pNode->mMatImg, cv::Mat() );

	OpenCVPlugin::mat2image( pNode->mMatImg, pNode->mOutputImage );

	pNode->pinUpdated( pNode->mPinOutputImage );
#endif
}

