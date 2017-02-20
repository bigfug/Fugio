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
#endif

Q_STATIC_ASSERT( sizeof( QPoint ) == sizeof( cv::Point ) );

FindContoursNode::FindContoursNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	mPinInputImage = pinInput( "Image" );

	mPinInputImage->registerPinInputType( PID_IMAGE );

	mValOutputContours = pinOutput<fugio::ArrayListInterface *>( "Contours", mPinOutputContours, PID_ARRAY_LIST );

	mValOutputHierarchy = pinOutput<fugio::ArrayInterface *>( "Hierarchy", mPinOutputHierarchy, PID_ARRAY );

	mValOutputHierarchy->setSize( 4 );
	mValOutputHierarchy->setStride( sizeof( int ) * 4 );
	mValOutputHierarchy->setType( QMetaType::Int );
}

void FindContoursNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	fugio::ImageInterface			*SrcImg = input<fugio::ImageInterface *>( mPinInputImage );

	if( !SrcImg || !SrcImg->isValid() )
	{
		return;
	}

#if defined( OPENCV_SUPPORTED )
	if( true ) //false )
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
	fugio::ImageInterface		*SrcImg = pNode->input<fugio::ImageInterface *>( pNode->mPinInputImage );

	cv::Mat						 MatSrc;

	OpenCVPlugin::image2mat( SrcImg ).copyTo( MatSrc );		// findContrours modifies the source mat

	pNode->mValOutputContours->clear();

	pNode->mValOutputHierarchy->setCount( 0 );
	pNode->mValOutputHierarchy->setArray( nullptr );

	pNode->mContours.clear();
	pNode->mHierarchy.clear();

	cv::findContours( MatSrc, pNode->mContours, pNode->mHierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE );

	pNode->mValOutputContours->setCount( pNode->mContours.size() );

	for( unsigned int i = 0 ; i < pNode->mContours.size() ; i++ )
	{
		std::vector<cv::Point>		&Src = pNode->mContours.at( i );
		fugio::ArrayInterface		*Dst = pNode->mValOutputContours->arrayIndex( i );

		Dst->setArray( Src.data() );
		Dst->setCount( Src.size() );
		Dst->setStride( sizeof( cv::Point ) );
		Dst->setType( QMetaType::QPoint );
		Dst->setSize( 1 );
	}

	pNode->mValOutputHierarchy->setCount( pNode->mHierarchy.size() );
	pNode->mValOutputHierarchy->setArray( pNode->mHierarchy.data() );

	pNode->pinUpdated( pNode->mPinOutputContours );
	pNode->pinUpdated( pNode->mPinOutputHierarchy );
#endif
}
