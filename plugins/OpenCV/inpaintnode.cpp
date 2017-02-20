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

	if( ( mOutputImage = pinOutput<fugio::ImageInterface *>( "Output", mPinOutputImage, PID_IMAGE ) ) == 0 )
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

	if( !mPinInputImage->isConnected() )
	{
		return;
	}

	QSharedPointer<PinControlInterface>		 C1 = mPinInputImage->connectedPin()->control();

	if( C1.isNull() )
	{
		return;
	}

	fugio::ImageInterface							*SrcImg = qobject_cast<fugio::ImageInterface *>( C1->qobject() );

	if( SrcImg == 0 || SrcImg->size().isEmpty() )
	{
		return;
	}

	QSharedPointer<PinControlInterface>		 C2 = mPinInputMask->connectedPin()->control();

	if( C2.isNull() )
	{
		return;
	}

	fugio::ImageInterface							*MskImg = qobject_cast<fugio::ImageInterface *>( C2->qobject() );

	if( MskImg == 0 || MskImg->size().isEmpty() )
	{
		return;
	}

#if defined( OPENCV_SUPPORTED )
	cv::Mat		MatSrc = OpenCVPlugin::image2mat( SrcImg );
	cv::Mat		MatMsk = OpenCVPlugin::image2mat( MskImg );
	cv::Mat		MatTmp;

	cv::cvtColor( MatSrc, MatTmp, CV_BGRA2RGB );

	cv::inpaint( MatTmp, MatMsk, mMatImg, mPinInputRadius->value().toDouble(), cv::INPAINT_NS );

	OpenCVPlugin::mat2image( mMatImg, mOutputImage, fugio::ImageInterface::FORMAT_RGB8 );
#endif

	mNode->context()->pinUpdated( mPinOutputImage );
}
