#include "inrangenode.h"

#include <QtConcurrent/QtConcurrentRun>
#include <QVector3D>
#include <QColor>
#include <QImage>

#include "opencvplugin.h"

#include <fugio/global_interface.h>
#include "fugio/context_interface.h"
#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>

#include <fugio/image/uuid.h>

#if defined( OPENCV_SUPPORTED )
#include <opencv2/imgproc/imgproc.hpp>
#endif

InRangeNode::InRangeNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	if( ( mPinInputImage = pinInput( "Input" ) ) == 0 )
	{
		return;
	}

	mPinInputLow = pinInput( "Low" );

	mPinInputHigh = pinInput( "High" );

	mPinInputLow->setValue( QVector3D( 0, 0, 0 ) );
	mPinInputHigh->setValue( QVector3D( 1, 1, 1 ) );

	if( ( mValOutputImage = pinOutput<fugio::VariantInterface *>( "Output", mPinOutputImage, PID_IMAGE ) ) == 0 )
	{
		return;
	}
}

InRangeNode::~InRangeNode()
{

}

void InRangeNode::inputsUpdated( qint64 pTimeStamp )
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

#if defined( OPENCV_SUPPORTED )
	if( false )
	{
		mNode->context()->futureSync( QtConcurrent::run( &InRangeNode::conversion, this ) );
	}
	else
	{
		conversion( this );
	}
#endif
}

void InRangeNode::conversion( InRangeNode *pNode )
{
#if defined( OPENCV_SUPPORTED )
	fugio::Image		SrcImg = variantStatic<fugio::Image>( pNode->mPinInputImage );

	if( SrcImg.isEmpty() )
	{
		return;
	}

	cv::Mat						 MatSrc = OpenCVPlugin::image2mat( SrcImg );

	int			l1, l2, l3;
	int			h1, h2, h3;

	l1 = l2 = l3 = 0;
	h1 = h2 = h3 = 255;

	pin2ints( pNode, pNode->mPinInputLow,  l1, l2, l3 );
	pin2ints( pNode, pNode->mPinInputHigh, h1, h2, h3 );

	cv::inRange( MatSrc, cv::Scalar( l1, l2, l3 ), cv::Scalar( h1, h2, h3 ), pNode->mMatImg );

	fugio::Image	DstImg = pNode->mValOutputImage->variant().value<fugio::Image>();

	OpenCVPlugin::mat2image( pNode->mMatImg, DstImg );

	pNode->pinUpdated( pNode->mPinOutputImage );
#endif
}

void InRangeNode::pin2ints( InRangeNode *pNode, QSharedPointer<PinInterface> &P, int &i1, int &i2, int &i3 )
{
	QVariant		V = pNode->variant( P );

	if( V.type() == QVariant::Vector3D )
	{
		QVector3D		V3 = V.value<QVector3D>();

		i1 = V3.x() * 255.0f;
		i2 = V3.y() * 255.0f;
		i3 = V3.z() * 255.0f;
	}
	else if( V.type() == QVariant::Color )
	{
		QColor			C = V.value<QColor>();

		i1 = C.red();
		i2 = C.green();
		i3 = C.blue();
	}
}
