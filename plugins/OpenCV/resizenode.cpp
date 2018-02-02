#include "resizenode.h"

#if defined( OPENCV_SUPPORTED )
#include <opencv2/imgproc/imgproc.hpp>
#endif

#include <fugio/image/uuid.h>

#include "opencvplugin.h"

#if defined( OPENCV_SUPPORTED )
QMap<int,QString>					ResizeNode::mInterpolationMap =
{
	{ cv::INTER_NEAREST, "INTER_NEAREST" },
	{ cv::INTER_LINEAR, "INTER_LINEAR" },
	{ cv::INTER_AREA, "INTER_AREA" },
	{ cv::INTER_CUBIC, "INTER_CUBIC" },
	{ cv::INTER_LANCZOS4, "INTER_LANCZOS4" }
};

#endif

ResizeNode::ResizeNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_IMAGE, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_INPUT_SIZE, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );
	FUGID( PIN_INPUT_SCALE_X, "261cc653-d7fa-4c34-a08b-3603e8ae71d5" );
	FUGID( PIN_INPUT_SCALE_Y, "249f2932-f483-422f-b811-ab679f006381" );
	FUGID( PIN_INPUT_INTERPOLATION, "ce8d578e-c5a4-422f-b3c4-a1bdf40facdb" );
	FUGID( PIN_OUTPUT_IMAGE, "e6bf944e-5f46-4994-bd51-13c2aa6415b7" );

	mPinInputImage = pinInput( "Image", PIN_INPUT_IMAGE );
	mPinInputSize = pinInput( "Size", PIN_INPUT_SIZE );
	mPinInputScaleX = pinInput( "Scale X", PIN_INPUT_SCALE_X );
	mPinInputScaleY = pinInput( "Scale Y", PIN_INPUT_SCALE_Y );

	mPinInputImage->registerPinInputType( PID_IMAGE );
	mPinInputSize->registerPinInputType( PID_SIZE );

	mValInputInterpolation = pinInput<fugio::ChoiceInterface *>( "Interpolation", mPinInputInterpolation, PID_CHOICE, PIN_INPUT_INTERPOLATION );

#if defined( OPENCV_SUPPORTED )
	mValInputInterpolation->setChoices( mInterpolationMap.values() );
	mPinInputInterpolation->setValue( "INTER_LINEAR" );
#endif

	mValOutputImage = pinOutput<fugio::VariantInterface *>( "Image", mPinOutputImage, PID_IMAGE, PIN_OUTPUT_IMAGE );
}

void ResizeNode::inputsUpdated( qint64 pTimeStamp )
{
	if( !mPinInputImage->isUpdated( pTimeStamp ) )
	{
		return;
	}

#if defined( OPENCV_SUPPORTED )

	fugio::Image	SrcImg = variant( mPinInputImage ).value<fugio::Image>();

	if( SrcImg.isEmpty() )
	{
		return;
	}

	QSize						 DstVal = variant( mPinInputSize ).toSize();
	double						 SclX   = variant( mPinInputScaleX ).toDouble();
	double						 SclY   = variant( mPinInputScaleY ).toDouble();
	QString						 IntStr = variant( mPinInputInterpolation ).toString();
	int							 IntVal = mInterpolationMap.key( IntStr, cv::INTER_LINEAR );

	cv::Mat						 SrcMat = OpenCVPlugin::image2mat( SrcImg );

	try
	{
		cv::Size				 SrcSze( qMax( 0, DstVal.width() ), qMax( 0, DstVal.height() ) );

		cv::resize( SrcMat, mMatImg, SrcSze, SclX, SclY, IntVal );

		OpenCVPlugin::mat2image( mMatImg, SrcImg, SrcImg.format() );

		pinUpdated( mPinOutputImage );
	}
	catch( ... )
	{
		mNode->setStatus( fugio::NodeInterface::Error );
	}
#endif
}
