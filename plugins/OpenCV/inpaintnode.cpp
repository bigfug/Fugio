#include "inpaintnode.h"

#include "opencvplugin.h"

#include <fugio/global_interface.h>
#include "fugio/context_interface.h"
#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>

#include <fugio/image/uuid.h>

#if defined( OPENCV_SUPPORTED )
#include <opencv2/photo/photo.hpp>
#endif

InPaintNode::InPaintNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	if( ( mPinInputImage = pinInput( "Input" ) ) == 0 )
	{
		return;
	}

	if( ( mPinInputMask = pinInput( "Mask" ) ) == 0 )
	{
		return;
	}

	if( ( mPinInputRadius = pinInput( "Radius" ) ) == 0 )
	{
		return;
	}

	if( ( mValOutputImage = pinOutput<fugio::VariantInterface *>( "Output", mPinOutputImage, PID_IMAGE ) ) == 0 )
	{
		return;
	}
}

InPaintNode::~InPaintNode()
{

}

bool InPaintNode::initialise()
{
	mPinInputRadius->setValue( "5.0" );

	return( true );
}

void InPaintNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	fugio::Image		SrcImg = variant<fugio::Image>( mPinInputImage );

	if( SrcImg.isEmpty() )
	{
		return;
	}

	fugio::Image		MskImg = variant<fugio::Image>( mPinInputMask );

	if( MskImg.isEmpty() )
	{
		return;
	}

#if defined( OPENCV_SUPPORTED )
	cv::Mat		MatSrc = OpenCVPlugin::image2mat( SrcImg );
	cv::Mat		MatMsk = OpenCVPlugin::image2mat( MskImg );
	cv::Mat		MatTmp;

	cv::cvtColor( MatSrc, MatTmp, CV_BGRA2RGB );

	cv::inpaint( MatTmp, MatMsk, mMatImg, mPinInputRadius->value().toDouble(), cv::INPAINT_NS );

	fugio::Image	DstImg = mValOutputImage->variant().value<fugio::Image>();

	OpenCVPlugin::mat2image( mMatImg, DstImg );
#endif

	pinUpdated( mPinOutputImage );
}
