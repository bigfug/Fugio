#include "countnonzeronode.h"

#if defined( OPENCV_SUPPORTED )
#include <opencv2/core/core.hpp>
#endif

#include <fugio/core/uuid.h>
#include <fugio/image/uuid.h>

#include <fugio/image/image_interface.h>

#include "opencvplugin.h"

CountNonZeroNode::CountNonZeroNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_IMAGE, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_OUTPUT_COUNT, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );

	mPinInputImage = pinInput( tr( "Image" ), PIN_INPUT_IMAGE );

	mPinInputImage->registerPinInputType( PID_IMAGE );

	mValOutputCount = pinOutput<fugio::VariantInterface *>( tr( "Count" ), mPinOutputCount, PID_INTEGER, PIN_OUTPUT_COUNT );
}

void CountNonZeroNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	int				 MatCnt = 0;

	if( mPinInputImage->isConnected() )
	{
		fugio::ImageInterface			*SrcImg = input<fugio::ImageInterface *>( mPinInputImage );

		if( SrcImg && SrcImg->isValid() )
		{
#if defined( OPENCV_SUPPORTED )
			cv::Mat	MatSrc = OpenCVPlugin::image2mat( SrcImg );

			if( MatSrc.channels() == 1 )
			{
				MatCnt = cv::countNonZero( MatSrc );
			}
			else
			{

			}
#endif
		}
	}

	if( MatCnt != mValOutputCount->variant().toInt() )
	{
		mValOutputCount->setVariant( MatCnt );

		pinUpdated( mPinOutputCount );
	}
}
