#include "cascadeclassifiernode.h"

#include <QtConcurrent/QtConcurrentRun>

#include "opencvplugin.h"

#include <fugio/global_interface.h>
#include <fugio/context_interface.h>
#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>
#include <fugio/file/filename_interface.h>

#include <fugio/image/uuid.h>

#include <fugio/performance.h>

#if defined( OPENCV_SUPPORTED )
#include <opencv2/imgproc/imgproc.hpp>
#endif

CascadeClassifierNode::CascadeClassifierNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_FILENAME, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_INPUT_IMAGE, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );
	FUGID( PIN_INPUT_ROI, "261cc653-d7fa-4c34-a08b-3603e8ae71d5" );
	FUGID( PIN_OUTPUT_RECTS, "249f2932-f483-422f-b811-ab679f006381" );

	mPinInputFilename = pinInput( "Filename", PIN_INPUT_FILENAME );

	mPinInputImage = pinInput( "Image", PIN_INPUT_IMAGE );

	mPinInputImage->registerPinInputType( PID_IMAGE );

	mPinInputROI = pinInput( "ROI", PIN_INPUT_ROI );

	mPinInputImage->registerPinInputType( PID_RECT );

	mValOutputRects = pinOutput<fugio::VariantInterface *>( "Rects", mPinOutputRects, PID_RECT, PIN_OUTPUT_RECTS );

	mValOutputRects->variantClear();
}

void CascadeClassifierNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

#if defined( OPENCV_SUPPORTED )
	if( mPinInputFilename->isUpdated( pTimeStamp ) )
	{
		QString				 Filename = variant( mPinInputFilename ).toString();

		if( Filename != mFilename )
		{
			mFilename = Filename;

			if( !mClassifier.load( mFilename.toStdString() ) )
			{
				mNode->setStatus( fugio::NodeInterface::Error );

				return;
			}

			mNode->setStatus( fugio::NodeInterface::Initialised );
		}
	}

	if( mClassifier.empty() )
	{
		mNode->setStatus( fugio::NodeInterface::Error );

		return;
	}

	fugio::Image	SrcImg = variant<fugio::Image>( mPinInputImage );

	if( !SrcImg.isValid() )
	{
		return;
	}

	if( false )
	{
		mNode->context()->futureSync( QtConcurrent::run( &CascadeClassifierNode::conversion, this ) );
	}
	else
	{
		fugio::Performance	P( mNode, "inputsUpdated", pTimeStamp );

		conversion( this );
	}
#endif
}

void CascadeClassifierNode::conversion( CascadeClassifierNode *pNode )
{
#if defined( OPENCV_SUPPORTED )
	fugio::Image	SrcImg = variantStatic<fugio::Image>( pNode->mPinInputImage );

	cv::Mat			MatSrc = OpenCVPlugin::image2mat( SrcImg );

	QRect			SrcROI = pNode->variant( pNode->mPinInputROI ).toRect();

	try
	{
		if( SrcROI.isEmpty() )
		{
			pNode->mClassifier.detectMultiScale( MatSrc, pNode->mRctTmp, 1.1, 3, cv::CASCADE_SCALE_IMAGE );
		}
		else
		{
			pNode->mClassifier.detectMultiScale( MatSrc( cv::Rect( SrcROI.x(), SrcROI.y(), SrcROI.width(), SrcROI.height() ) ), pNode->mRctTmp, 1.1, 3, cv::CASCADE_SCALE_IMAGE );
		}
	}
	catch( cv::Exception e )
	{
		pNode->mNode->setStatus( fugio::NodeInterface::Error );
		pNode->mNode->setStatusMessage( e.err.c_str() );

		return;
	}
	catch( ... )
	{
		return;
	}

	if( !pNode->mRctTmp.size() )
	{
		pNode->mValOutputRects->variantClear();
	}
	else
	{
		pNode->mRctVec.resize( pNode->mRctTmp.size() );

		pNode->mValOutputRects->setVariantCount( pNode->mRctVec.size() );

		const cv::Rect	*SrcRct = &pNode->mRctTmp[ 0 ];

		for( size_t i = 0 ; i < pNode->mRctTmp.size() ; i++, SrcRct++ )
		{
			pNode->mValOutputRects->setVariant( i, QRect( SrcRct->x, SrcRct->y, SrcRct->width, SrcRct->height ) );
		}
	}

	pNode->pinUpdated( pNode->mPinOutputRects );
#endif
}

