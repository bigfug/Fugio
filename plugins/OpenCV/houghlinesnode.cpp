#include "houghlinesnode.h"

#include <QList>
#include <QLine>

#include <fugio/core/uuid.h>
#include <fugio/image/uuid.h>

#include "opencvplugin.h"

#if defined( OPENCV_SUPPORTED )
#include <opencv2/imgproc/imgproc.hpp>
#endif

HoughLinesNode::HoughLinesNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_IMAGE,  "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_INPUT_RHO, "1ceaeb2d-0463-44bc-9b80-10129cd8f4eb" );
	FUGID( PIN_INPUT_THETA, "5064e449-8b0b-4447-9009-c81997f754ef" );
	FUGID( PIN_INPUT_THRESHOLD, "5c8f8f4e-58ce-4e47-9e1e-4168d17e1863" );
	FUGID( PIN_INPUT_MIN_LINE_LENGTH, "a2bbf374-0dc8-42cb-b85a-6a43b58a348f" );
	FUGID( PIN_INPUT_MAX_LINE_GAP, "51297977-7b4b-4e08-9dea-89a8add4abe0" );
	FUGID( PIN_OUTPUT_LINES, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );

	mPinInputImage = pinInput( "Image", PIN_INPUT_IMAGE );

	mPinInputImage->registerPinInputType( PID_IMAGE );

	mValOutputLines = pinOutput<fugio::VariantInterface *>( "Lines", mPinOutputLines, PID_LINE, PIN_OUTPUT_LINES );

	mPinInputRho           = pinInput( "Rho", PIN_INPUT_RHO );
	mPinInputTheta         = pinInput( "Theta", PIN_INPUT_THETA );
	mPinInputThreshold     = pinInput( "Threshold", PIN_INPUT_THRESHOLD );
	mPinInputMinLineLength = pinInput( "Min Line Length", PIN_INPUT_MIN_LINE_LENGTH );
	mPinInputMaxLineGap    = pinInput( "Max Line Gap", PIN_INPUT_MAX_LINE_GAP );

	mPinInputRho->setValue( 1 );
	mPinInputTheta->setValue( M_PI/180 );
	mPinInputThreshold->setValue( 100 );
	mPinInputMinLineLength->setValue( 0 );
	mPinInputMaxLineGap->setValue( 0 );

	mValOutputLines->variantClear();
}

void HoughLinesNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	fugio::Image	SrcImg = variant<fugio::Image>( mPinInputImage );

	if( !SrcImg.isValid() )
	{
		return;
	}

	qreal	Rho           = variant( mPinInputRho ).toReal();
	qreal	Theta         = variant( mPinInputTheta ).toReal();
	qreal	Threshold     = variant( mPinInputThreshold ).toReal();
	qreal	MinLineLength = variant( mPinInputMinLineLength ).toReal();
	qreal	MaxLineGap    = variant( mPinInputMaxLineGap ).toReal();

#if defined( OPENCV_SUPPORTED )
	cv::Mat							 MatSrc = OpenCVPlugin::image2mat( SrcImg );

	mValOutputLines->variantClear();

	try
	{
#if 0
		std::vector<cv::Vec2f> lines;

		cv::HoughLines( MatSrc, lines, 1, CV_PI/180, 100 );

		mLineData.resize( lines.size() );

		for( size_t i = 0; i < lines.size(); i++ )
		{
			float rho = lines[i][0];
			float theta = lines[i][1];
			double a = cos(theta), b = sin(theta);
			double x0 = a*rho, y0 = b*rho;
			QPointF pt1(cvRound(x0 + MatSrc.cols*(-b)),
					   cvRound(y0 + MatSrc.rows*(a)));
			QPointF pt2(cvRound(x0 - MatSrc.cols*(-b)),
					   cvRound(y0 - MatSrc.rows*(a)));

			mLineData[ i ] = QLineF( pt1, pt2 );
		}
#else
		std::vector<cv::Vec4i> lines;

		cv::HoughLinesP( MatSrc, lines, Rho, Theta, Threshold, MinLineLength, MaxLineGap );

		mValOutputLines->setVariantCount( lines.size() );

		for( size_t i = 0; i < lines.size() ; i++ )
		{
			const cv::Vec4i &v = lines[ i ];

			mValOutputLines->setVariant( i, QLineF( QPointF( v[0], v[1] ), QPointF( v[ 2 ], v[ 3 ] ) ) );
		}
#endif
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

	pinUpdated( mPinOutputLines );
#endif
}
