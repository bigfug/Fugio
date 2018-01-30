#include "imagehomographynode.h"

#include <QImage>
#include <QPainter>
#include <QMatrix3x3>
#include <QMatrix4x4>

#if defined( OPENCV_SUPPORTED )
#include <opencv2/imgproc/imgproc.hpp>
#endif

#include <fugio/node_interface.h>
#include <fugio/context_interface.h>
#include <fugio/core/uuid.h>
#include <fugio/math/uuid.h>
#include <fugio/image/uuid.h>
#include <fugio/context_signals.h>

#include "opencvplugin.h"

ImageHomographyNode::ImageHomographyNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mCurIdx( 0 ), mLstBut( false )
{
	if( ( mPinInputWidth = pinInput( "Width" ) ) == 0 )
	{
		return;
	}

	if( ( mPinInputHeight = pinInput( "Height" ) ) == 0 )
	{
		return;
	}

	if( ( mPinInputImage = pinInput( "Input Image" ) ) == 0 )
	{
		return;
	}

	if( ( mPinInputMouseButtonLeft = pinInput( "Mouse Left" ) ) == 0 )
	{
		return;
	}

	if( ( mPinInputMousePosition = pinInput( "Mouse Position" ) ) == 0 )
	{
		return;
	}

	if( ( mPinInputWarp = pinInput( "Warp" ) ) == 0 )
	{
		return;
	}

	if( ( mOutputImage = pinOutput<fugio::VariantInterface *>( "Image", mPinOutputImage, PID_IMAGE ) ) == 0 )
	{
		return;
	}

	if( ( mOutputMatrix = pinOutput<VariantInterface *>( "Matrix", mPinOutputMatrix, PID_MATRIX4 ) ) == 0 )
	{
		return;
	}

	if( ( mOutputInverse = pinOutput<VariantInterface *>( "Inverse", mPinOutputInverse, PID_MATRIX4 ) ) == 0 )
	{
		return;
	}

	mPinInputMargin = pinInput( "Margin" );

	mPinInputMargin->setValue( 100 );
}

ImageHomographyNode::~ImageHomographyNode()
{

}

bool ImageHomographyNode::initialise()
{
	if( !fugio::NodeControlBase::initialise() )
	{
		return( false );
	}

#if defined( OPENCV_SUPPORTED )
    mPntSrc.resize( 4 );
	mPntDst.resize( 4 );
#endif

	//connect( mNode->context()->context(), SIGNAL(contextStart()), this, SLOT(onContextFrame()) );

	return( true );
}

void ImageHomographyNode::onContextFrame()
{
}

void ImageHomographyNode::updateHomography()
{
#if defined( OPENCV_SUPPORTED )
    mMatHom = cv::getPerspectiveTransform( mPntDst.data(), mPntSrc.data() );

	cv::Mat			MatTmp = mMatHom.inv();

	QMatrix4x4		MatOut, MatInv;

	for( int i = 0 ; i < 3 ; i++ )
	{
		for( int j = 0 ; j < 3 ; j++ )
		{
			MatOut( i, j ) = mMatHom.at<double>( i, j );
			MatInv( i, j ) = MatTmp.at<double>( i, j );
		}
	}

	mOutputMatrix->setVariant( MatOut );
	mOutputInverse->setVariant( MatInv );

	mNode->context()->pinUpdated( mPinOutputMatrix );
	mNode->context()->pinUpdated( mPinOutputInverse );
#endif
}

void ImageHomographyNode::inputsUpdated( qint64 )
{
#if defined( OPENCV_SUPPORTED )
    QSize				 ImgSze;

	ImgSze.setWidth( variant( mPinInputWidth ).toInt() );
	ImgSze.setHeight( variant( mPinInputHeight ).toInt() );

	if( ImgSze.width() <= 0 || ImgSze.height() <= 0 )
	{
		return;
	}

	int		Margin = variant( mPinInputMargin ).toInt();

	if( mLstSzeDst != ImgSze || mLstMargin != Margin )
	{
		mPntDst[ 0 ] = cv::Point2f( Margin, Margin );
		mPntDst[ 1 ] = cv::Point2f( ImgSze.width() - 1 - Margin, Margin );
		mPntDst[ 2 ] = cv::Point2f( ImgSze.width() - 1 - Margin, ImgSze.height() - 1 - Margin );
		mPntDst[ 3 ] = cv::Point2f( Margin, ImgSze.height() - 1 - Margin );

		updateHomography();

		mLstSzeDst = ImgSze;
		mLstMargin = Margin;
	}

	//-------------------------------------------------------------------------

	mImgScl.setX( qreal( mImgSrc.width()  ) / qreal( ImgSze.width()  ) );
	mImgScl.setY( qreal( mImgSrc.height() ) / qreal( ImgSze.height() ) );

	//-------------------------------------------------------------------------

	bool	Warp = variant( mPinInputWarp ).toBool();

	//-------------------------------------------------------------------------

	if( !Warp )
	{
		bool	CurBut = variant( mPinInputMouseButtonLeft ).toBool();
		QPointF	CurPos = variant( mPinInputMousePosition ).toPointF();

		if( mLstBut && !CurBut )
		{
			qDebug() << ImgSze << mImgSrc.size();

			mPntSrc[ mCurIdx ] = cv::Point2f( CurPos.x() * mImgScl.x(), CurPos.y() * mImgScl.y() );

			updateHomography();

			mCurIdx = ( mCurIdx + 1 ) % 4;
		}

		mLstBut = CurBut;
		mLstPos = CurPos;
	}

	//-------------------------------------------------------------------------

	if( mPinInputImage->isConnected() )
	{
		fugio::Image		I1 = variant<fugio::Image>( mPinInputImage );

//		if( I1->format() != fugio::ImageFormat::RGB8 )
//		{
//			return;
//		}

		mImgSrc = I1.image().convertToFormat( QImage::Format_RGB888 );

		if( mImgSrc.isNull() )
		{
			return;
		}

		if( mLstSzeSrc != mImgSrc.size() )
		{
			mPntSrc[ 0 ] = cv::Point2f( Margin, Margin );
			mPntSrc[ 1 ] = cv::Point2f( mImgSrc.width() - 1 - Margin, Margin );
			mPntSrc[ 2 ] = cv::Point2f( mImgSrc.width() - 1 - Margin, mImgSrc.height() - 1 - Margin );
			mPntSrc[ 3 ] = cv::Point2f( Margin, mImgSrc.height() - 1 - Margin );

			updateHomography();

			mLstSzeSrc = mImgSrc.size();
		}

		cv::Mat		MatSrc = cv::Mat( mImgSrc.height(), mImgSrc.width(), CV_8UC3, (void *)mImgSrc.constBits(), size_t( mImgSrc.bytesPerLine() ) );

		if( !Warp )
		{
			for( int i = 0 ; i < 4 ; i++ )
			{
				int		i1 = ( i + 0 ) % 4;
				int		i2 = ( i + 1 ) % 4;

				cv::line( MatSrc, mPntSrc[ i1 ], mPntSrc[ i2 ], cv::Scalar( 0, 0, 255 ) );
			}

			cv::resize( MatSrc, mMatDst, cv::Size( ImgSze.width(), ImgSze.height() ) );

			for( int i = 0 ; i < 4 ; i++ )
			{
				cv::circle( mMatDst, mPntDst[ i ], 30, mCurIdx == i ? cv::Scalar( 0, 0, 255 ) : cv::Scalar( 255, 0, 0 ), -1 );
			}

			for( int i = 0 ; i < 4 ; i++ )
			{
				int		i1 = ( i + 0 ) % 4;
				int		i2 = ( i + 1 ) % 4;

				cv::Point2f		p1 = mPntDst[ i1 ];
				cv::Point2f		p2 = mPntDst[ i2 ];

				cv::line( mMatDst, p1, p2, cv::Scalar( 255, 255, 255 ) );
			}
		}
		else
		{
			cv::warpPerspective( MatSrc, mMatDst, mMatHom.inv(), cv::Size( ImgSze.width(), ImgSze.height() ) );
		}

		fugio::Image	Output = mOutputImage->variant().value<fugio::Image>();

		Output.setSize( mMatDst.cols, mMatDst.rows );
		Output.setLineSize( 0, mMatDst.step );

		Output.setFormat( fugio::ImageFormat::RGB8 );

		Output.setBuffer( 0, mMatDst.data );

		pinUpdated( mPinOutputImage );
	}
#endif
}
