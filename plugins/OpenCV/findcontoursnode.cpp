#include "findcontoursnode.h"

#include <QtConcurrent/QtConcurrentRun>

#include "opencvplugin.h"

#include <fugio/global_interface.h>
#include "fugio/context_interface.h"
#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>
#include <fugio/performance.h>

#include <fugio/image/uuid.h>

#if defined( OPENCV_SUPPORTED )
#include <opencv2/imgproc/imgproc.hpp>

Q_STATIC_ASSERT( sizeof( QPoint ) == sizeof( cv::Point ) );

#endif

FindContoursNode::FindContoursNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_IMAGE, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_OUTPUT_CONTOURS, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );
	FUGID( PIN_OUTPUT_HIERARCHY, "261cc653-d7fa-4c34-a08b-3603e8ae71d5" );

	mPinInputImage = pinInput( "Image", PIN_INPUT_IMAGE );

	mPinInputImage->registerPinInputType( PID_IMAGE );

	mValOutputContours = pinOutput<fugio::VariantInterface *>( "Contours", mPinOutputContours, PID_POLYGON, PIN_OUTPUT_CONTOURS );

	mValOutputHierarchy = pinOutput<fugio::VariantInterface *>( "Hierarchy", mPinOutputHierarchy, PID_INTEGER, PIN_OUTPUT_HIERARCHY );

	mValOutputHierarchy->setVariantElementCount( 4 );

	mValOutputHierarchy->variantClear();
}

void FindContoursNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	fugio::Image	SrcImg = variant<fugio::Image>( mPinInputImage );

	if( !SrcImg.isValid() )
	{
		return;
	}

#if defined( OPENCV_SUPPORTED )
	if( false )
	{
		mNode->context()->futureSync( QtConcurrent::run( &FindContoursNode::conversion, this, pTimeStamp ) );
	}
	else
	{
		conversion( this, pTimeStamp );
	}
#endif
}

void FindContoursNode::conversion( FindContoursNode *pNode, qint64 pTimeStamp )
{
	fugio::Performance	Perf( pNode->node(), "conversion", pTimeStamp );

#if defined( OPENCV_SUPPORTED )
	std::vector< std::vector<cv::Point> >		 Contours;
	std::vector<cv::Vec4i>						 Hierarchy;

	fugio::Image	SrcImg = variantStatic<fugio::Image>( pNode->mPinInputImage );

	cv::Mat			MatSrc = OpenCVPlugin::image2mat( SrcImg ).clone();		// findContrours modifies the source mat

	cv::findContours( MatSrc, Contours, Hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE );

	pNode->mValOutputContours->setVariantCount( Contours.size() );

	for( unsigned int i = 0 ; i < Contours.size() ; i++ )
	{
		QPolygonF					 Ply;

		std::vector<cv::Point>		&Src = Contours.at( i );

		for( unsigned int j = 0 ; j < Src.size() ; j++ )
		{
			cv::Point				 Pnt = Src.at( j );

			Ply << QPointF( Pnt.x, Pnt.y );
		}

		pNode->mValOutputContours->setVariant( i, Ply );
	}

	pNode->mValOutputHierarchy->setVariantCount( Hierarchy.size() );

	if( Hierarchy.size() )
	{
		memcpy( pNode->mValOutputHierarchy->variantArray(), Hierarchy.data(), sizeof( int ) * 4 * Hierarchy.size() );
	}

	pNode->pinUpdated( pNode->mPinOutputContours );
	pNode->pinUpdated( pNode->mPinOutputHierarchy );
#endif
}
