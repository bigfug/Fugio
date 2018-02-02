#include "momentsnode.h"

#include <QtConcurrent/QtConcurrentRun>

#include "opencvplugin.h"

#include <fugio/global_interface.h>
#include "fugio/context_interface.h"
#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>

#include <fugio/core/uuid.h>
#include <fugio/image/uuid.h>

#if defined( OPENCV_SUPPORTED )
#include <opencv2/imgproc/imgproc.hpp>
#endif

MomentsNode::MomentsNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_IMAGE, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_OUTPUT_AREA, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );
	FUGID( PIN_OUTPUT_POSITION, "261cc653-d7fa-4c34-a08b-3603e8ae71d5" );

	mPinInputImage = pinInput( "Image", PIN_INPUT_IMAGE );

	mPinInputImage->registerPinInputType( PID_IMAGE );

	mValOutputArea = pinOutput<fugio::VariantInterface *>( "Area", mPinOutputArea, PID_FLOAT, PIN_OUTPUT_AREA );

	mValOutputPosition = pinOutput<fugio::VariantInterface *>( "Position", mPinOutputPosition, PID_POINT, PIN_OUTPUT_POSITION );
}

void MomentsNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	if( !mPinInputImage->isConnected() )
	{
		return;
	}

	fugio::Image		SrcImg = variant<fugio::Image>( mPinInputImage );

	if( SrcImg.isEmpty() )
	{
		return;
	}

#if defined( OPENCV_SUPPORTED )
	cv::Mat				MatSrc = OpenCVPlugin::image2mat( SrcImg );

	cv::Moments			SrcMom = cv::moments( MatSrc, true );

	if( mValOutputArea->variant().toFloat() != SrcMom.m00 )
	{
		mValOutputArea->setVariant( SrcMom.m00 );

		pinUpdated( mPinOutputArea );
	}

	QPointF				SrcPnt = mValOutputPosition->variant().toPointF();

	if( SrcMom.m00 > 0 )
	{
		SrcPnt.setX( SrcMom.m10 / SrcMom.m00 );
		SrcPnt.setY( SrcMom.m01 / SrcMom.m00 );

		if( SrcPnt != mValOutputPosition->variant().toPointF() )
		{
			mValOutputPosition->setVariant( SrcPnt );

			pinUpdated( mPinOutputPosition );
		}
	}
#endif
}
