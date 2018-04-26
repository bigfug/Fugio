#include "equalizehistnode.h"

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

EqualizeHistNode::EqualizeHistNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	mPinInputImage = pinInput( "Image" );

	mPinInputImage->registerPinInputType( PID_IMAGE );

	mValOutputImage = pinOutput<fugio::VariantInterface *>( "Image", mPinOutputImage, PID_IMAGE );
}

void EqualizeHistNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	if( !mPinInputImage->isConnected() )
	{
		return;
	}

	fugio::Image	SrcImg = variant<fugio::Image>( mPinInputImage );

	if( !SrcImg.isValid() )
	{
		return;
	}

#if defined( OPENCV_SUPPORTED )
	if( false )
	{
		mNode->context()->futureSync( QtConcurrent::run( &EqualizeHistNode::conversion, this ) );
	}
	else
	{
		fugio::Performance	P( mNode, "inputsUpdated", pTimeStamp );

		conversion( this );
	}
#endif
}

void EqualizeHistNode::conversion( EqualizeHistNode *pNode )
{
#if defined( OPENCV_SUPPORTED )
	fugio::Image	SrcImg = variantStatic<fugio::Image>( pNode->mPinInputImage );

	cv::Mat			MatSrc = OpenCVPlugin::image2mat( SrcImg );

	try
	{
		cv::equalizeHist( MatSrc, pNode->mMatImg );

		pNode->mNode->setStatus( fugio::NodeInterface::Initialised );
	}
	catch( cv::Exception e )
	{
		pNode->mNode->setStatus( fugio::NodeInterface::Error );
		pNode->mNode->setStatusMessage( e.err.c_str() );
	}
	catch( ... )
	{

	}

	fugio::Image	DstImg = pNode->mValOutputImage->variant().value<fugio::Image>();

	OpenCVPlugin::mat2image( pNode->mMatImg, DstImg );

	pNode->pinUpdated( pNode->mPinOutputImage );
#endif
}

