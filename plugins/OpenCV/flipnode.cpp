#include "flipnode.h"

#include "opencvplugin.h"

#include <fugio/global_interface.h>
#include "fugio/context_interface.h"
#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>

#include <fugio/image/uuid.h>

#if defined( OPENCV_SUPPORTED )
#include <opencv2/imgproc/imgproc.hpp>
#endif

FlipNode::FlipNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	mPinInputImage = pinInput( "Image" );

	mPinInputImage->registerPinInputType( PID_IMAGE );

	mValOutputImage = pinOutput<fugio::VariantInterface *>( "Image", mPinOutputImage, PID_IMAGE );
}

void FlipNode::inputsUpdated( qint64 pTimeStamp )
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
	cv::Mat						 MatSrc = OpenCVPlugin::image2mat( SrcImg );

	cv::flip( MatSrc, mMatImg, 0 );

	fugio::Image	DstImg = mValOutputImage->variant().value<fugio::Image>();

	OpenCVPlugin::mat2image( mMatImg, DstImg, SrcImg.format() );

	pinUpdated( mPinOutputImage );
#endif
}
