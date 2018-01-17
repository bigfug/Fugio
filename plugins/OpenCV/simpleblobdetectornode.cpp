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

	mValOutputPoints = pinOutput<fugio::ArrayInterface *>( "Points", mPinOutputPoints, PID_ARRAY, PIN_OUTPUT_POINTS );

	mValOutputPoints->setType( QMetaType::QPointF );
	mValOutputPoints->setSize( 1 );
	mValOutputPoints->setStride( sizeof( QPointF ) );

	mValOutputSizes = pinOutput<fugio::ArrayInterface *>( "Sizes", mPinOutputSizes, PID_ARRAY, PIN_OUTPUT_SIZES );

	mValOutputSizes->setType( QMetaType::Float );
	mValOutputSizes->setSize( 1 );
	mValOutputSizes->setStride( sizeof( float ) );

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

	fugio::ImageInterface		*SrcImg = input<fugio::ImageInterface *>( mPinInputImage );

	if( !SrcImg || SrcImg->size().isEmpty() )
	{
		return;
	}

	mValOutputPoints->setArray( Q_NULLPTR );
	mValOutputPoints->setCount( 0 );

	mValOutputSizes->setArray( Q_NULLPTR );
	mValOutputSizes->setCount( 0 );

#if defined( OPENCV_SUPPORTED )
	try
	{
		cv::Mat					 MatSrc = OpenCVPlugin::image2mat( SrcImg );

		std::vector<cv::KeyPoint> KeyPoints;

		mDetector->detect( MatSrc, KeyPoints );

		qDebug() << "keyPoints" << KeyPoints.size();

		mPoints.resize( KeyPoints.size() );
		mSizes.resize( KeyPoints.size() );

		for( unsigned int i = 0 ; i < KeyPoints.size() ; i++ )
		{
			const cv::KeyPoint &kp = KeyPoints[ i ];

			mPoints[ i ] = QPointF( kp.pt.x, kp.pt.y );

			mSizes[ i ] = kp.size;
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

	if( !mPoints.isEmpty() )
	{
		mValOutputPoints->setArray( mPoints.data() );
		mValOutputPoints->setCount( mPoints.size() );

		mValOutputSizes->setArray( mSizes.data() );
		mValOutputSizes->setCount( mSizes.size() );
	}

	pinUpdated( mPinOutputPoints );
	pinUpdated( mPinOutputSizes );

#endif
}

