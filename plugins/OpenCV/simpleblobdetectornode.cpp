#include "simpleblobdetectornode.h"

#include <QtConcurrent/QtConcurrentRun>

#include "opencvplugin.h"

#include <fugio/core/uuid.h>

#include <fugio/global_interface.h>
#include "fugio/context_interface.h"
#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>

#include <fugio/image/uuid.h>

#if defined( OPENCV_SUPPORTED )
#include <opencv2/imgproc/imgproc.hpp>
#endif

SimpleBlobDetectorNode::SimpleBlobDetectorNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_IMAGE, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_OUTPUT_POINTS, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );
	FUGID( PIN_OUTPUT_SIZES, "e27ce75e-fb9f-4a57-97b8-1c2d966f053b" );

	mPinInputImage = pinInput( "Image", PIN_INPUT_IMAGE );

	mPinInputImage->registerPinInputType( PID_IMAGE );

	mValOutputPoints = pinOutput<fugio::VariantInterface *>( "Points", mPinOutputPoints, PID_POINT, PIN_OUTPUT_POINTS );

	mValOutputSizes = pinOutput<fugio::VariantInterface *>( "Sizes", mPinOutputSizes, PID_FLOAT, PIN_OUTPUT_SIZES );

	mValOutputPoints->variantClear();
	mValOutputSizes->variantClear();

#if defined( OPENCV_SUPPORTED )

	cv::SimpleBlobDetector::Params	BlbPrm;

	BlbPrm.filterByArea        = true;
	BlbPrm.filterByCircularity = false;
	BlbPrm.filterByColor       = false;
	BlbPrm.filterByConvexity   = false;
	BlbPrm.filterByInertia     = false;

	BlbPrm.minArea = 0.01f;
	BlbPrm.maxArea = 10000.0f;

	BlbPrm.minDistBetweenBlobs = 0.1f;

#if ( CV_MAJOR_VERSION <= 2 )
	mDetector = new cv::SimpleBlobDetector( BlbPrm );
#else
	mDetector = cv::SimpleBlobDetector::create( BlbPrm );
#endif

#endif
}

void SimpleBlobDetectorNode::inputsUpdated( qint64 pTimeStamp )
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

	mValOutputPoints->variantClear();
	mValOutputSizes->variantClear();

#if defined( OPENCV_SUPPORTED )
	try
	{
		cv::Mat					 MatSrc = OpenCVPlugin::image2mat( SrcImg );

		std::vector<cv::KeyPoint> KeyPoints;

		mDetector->detect( MatSrc, KeyPoints );

		qDebug() << "keyPoints" << KeyPoints.size();

		mValOutputPoints->setVariantCount( KeyPoints.size() );
		mValOutputSizes->setVariantCount( KeyPoints.size() );

		for( unsigned int i = 0 ; i < KeyPoints.size() ; i++ )
		{
			const cv::KeyPoint &kp = KeyPoints[ i ];

			mValOutputPoints->setVariant( i, QPointF( kp.pt.x, kp.pt.y ) );

			mValOutputSizes->setVariant( i, kp.size );
		}
	}
	catch( cv::Exception e )
	{
		mNode->setStatus( fugio::NodeInterface::Error );
		mNode->setStatusMessage( QString::fromStdString( e.msg ) );

		return;
	}
	catch( ... )
	{
		mNode->setStatus( fugio::NodeInterface::Error );

		return;
	}

	mNode->setStatus( fugio::NodeInterface::Initialised );

	pinUpdated( mPinOutputPoints );
	pinUpdated( mPinOutputSizes );

#endif
}

