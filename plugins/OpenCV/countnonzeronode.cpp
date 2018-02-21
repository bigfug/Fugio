#include "countnonzeronode.h"

#if defined( OPENCV_SUPPORTED )
#include <opencv2/core/core.hpp>
#endif

#include <QRectF>

#include <fugio/core/uuid.h>
#include <fugio/image/uuid.h>

#include <fugio/image/image.h>

#include <fugio/performance.h>

#include "opencvplugin.h"

CountNonZeroNode::CountNonZeroNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_IMAGE,		"9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_INPUT_ROI,		"5064e449-8b0b-4447-9009-c81997f754ef" );
	FUGID( PIN_OUTPUT_COUNT,	"1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );

	mPinInputImage = pinInput( tr( "Image" ), PIN_INPUT_IMAGE );

	mPinInputImage->registerPinInputType( PID_IMAGE );

	mPinInputROI = pinInput( "ROI", PIN_INPUT_ROI );

	mValOutputCount = pinOutput<fugio::VariantInterface *>( tr( "Count" ), mPinOutputCount, PID_INTEGER, PIN_OUTPUT_COUNT );
}

void CountNonZeroNode::inputsUpdated( qint64 pTimeStamp )
{
	fugio::Performance	P( mNode, __FUNCTION__, pTimeStamp );

	NodeControlBase::inputsUpdated( pTimeStamp );

	bool				UpdateOutput = mPinOutputCount->alwaysUpdate();
	int					MatCnt = 0;

	QRectF				SrcROI = variant<QRectF>( mPinInputROI );

	fugio::Image		SrcImg = variant<fugio::Image>( mPinInputImage );

	if( SrcImg.isValid() )
	{
#if defined( OPENCV_SUPPORTED )
		cv::Mat	MatSrc = OpenCVPlugin::image2mat( SrcImg );

		if( MatSrc.channels() == 1 )
		{
			if( SrcROI.isValid() )
			{
				cv::Rect	MatROI;

				SrcROI = SrcROI.intersected( QRect( QPoint(), SrcImg.size() ) );

				if( SrcROI.isValid() )
				{
					MatROI = cv::Rect( SrcROI.x(), SrcROI.y(), SrcROI.width(), SrcROI.height() );

					cv::Mat		MatTmp( MatSrc, MatROI );

					MatCnt = cv::countNonZero( MatTmp );
				}
			}
			else
			{
				MatCnt = cv::countNonZero( MatSrc );
			}
		}
		else
		{

		}
#endif
	}

	variantSetValue<int>( mValOutputCount, 0, MatCnt, UpdateOutput );

	if( UpdateOutput )
	{
		pinUpdated( mPinOutputCount );
	}
}
