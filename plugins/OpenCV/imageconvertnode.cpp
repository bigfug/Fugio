#include "imageconvertnode.h"

#include <QComboBox>
#include <QSettings>

#include <fugio/node_interface.h>
#include <fugio/context_interface.h>
#include <fugio/image/uuid.h>

#include <fugio/performance.h>

#include "opencvplugin.h"

#if defined( OPENCV_SUPPORTED )
#include <opencv2/imgproc/imgproc.hpp>
#endif

QMap<QString,fugio::ImageInterface::Format>	 ImageConvertNode::mFormats;


ImageConvertNode::ImageConvertNode( QSharedPointer<fugio::NodeInterface> pNode ) :
	NodeControlBase( pNode ), mDestinationFormat( fugio::ImageInterface::FORMAT_RGB8 )
{
	mPinInputImage = pinInput( "Input" );

	mOutputImage = pinOutput<fugio::ImageInterface *>( "Output", mPinOutputImage, PID_IMAGE );

	if( mFormats.isEmpty() )
	{
		mFormats[ "RGB8" ] = fugio::ImageInterface::FORMAT_RGB8;
		mFormats[ "RGBA8" ] = fugio::ImageInterface::FORMAT_RGBA8;
		mFormats[ "BGR8" ] = fugio::ImageInterface::FORMAT_BGR8;
		mFormats[ "BGRA8" ] = fugio::ImageInterface::FORMAT_BGRA8;
		mFormats[ "YUYV422" ] = fugio::ImageInterface::FORMAT_YUYV422;
		mFormats[ "UYVY422" ] = fugio::ImageInterface::FORMAT_UYVY422;
		mFormats[ "YUV420P" ] = fugio::ImageInterface::FORMAT_YUV420P;
		mFormats[ "GRAY16" ] = fugio::ImageInterface::FORMAT_GRAY16;
		mFormats[ "GRAY8" ] = fugio::ImageInterface::FORMAT_GRAY8;
		mFormats[ "HSV8" ] = fugio::ImageInterface::FORMAT_HSV8;
	}
}

QWidget *ImageConvertNode::gui( void )
{
	QComboBox		*GUI = new QComboBox();

	for( QMap<QString,fugio::ImageInterface::Format>::const_iterator it = mFormats.begin() ; it != mFormats.end() ; it++ )
	{
		GUI->addItem( it.key(), int( it.value() ) );
	}

	GUI->setCurrentText( mFormats.key( mDestinationFormat ) );

	connect( GUI, SIGNAL(currentIndexChanged(QString)), this, SLOT(targetChanged(QString)) );

	connect( this, SIGNAL(targetUpdated(QString)), GUI, SLOT(setCurrentText(QString)) );

	return( GUI );
}

void ImageConvertNode::loadSettings( QSettings &pSettings )
{
	QString								 FmtNam = pSettings.value( "format", mFormats.firstKey() ).toString();
	fugio::ImageInterface::Format		 ImgFmt = fugio::ImageInterface::Format( mFormats.value( FmtNam, mDestinationFormat ) );

	if( ImgFmt != mDestinationFormat )
	{
		mDestinationFormat = ImgFmt;

		mNode->context()->updateNode( mNode );

		emit targetUpdated( FmtNam );
	}
}

void ImageConvertNode::saveSettings( QSettings &pSettings ) const
{
	pSettings.setValue( "format", mFormats.key( mDestinationFormat ) );
}

void ImageConvertNode::targetChanged( const QString &pName )
{
	fugio::ImageInterface::Format		 ImgFmt = mFormats.value( pName );

	if( ImgFmt == mDestinationFormat )
	{
		return;
	}

	mDestinationFormat = ImgFmt;

	mNode->context()->updateNode( mNode );

	emit targetUpdated( pName );
}

void ImageConvertNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	fugio::ImageInterface							*SrcImg = input<fugio::ImageInterface *>( mPinInputImage );

	if( !SrcImg || SrcImg->size().isEmpty() )
	{
		mNode->setStatus( NodeInterface::Warning );
		mNode->setStatusMessage( "No valid input image" );

		return;
	}

#if defined( OPENCV_SUPPORTED )
	cv::Mat						MatSrc;

	try
	{
		MatSrc = OpenCVPlugin::image2mat( SrcImg );
	}
	catch( cv::Exception e )
	{
		return;
	}

	int							CvtCod = -1;

	switch( SrcImg->format() )
	{
		case fugio::ImageInterface::FORMAT_HSV8:
			switch( mDestinationFormat )
			{
				case fugio::ImageInterface::FORMAT_BGR8:
					CvtCod = CV_HSV2BGR;
					break;

				case fugio::ImageInterface::FORMAT_RGB8:
					CvtCod = CV_HSV2RGB;
					break;

				default:
					break;
			}
			break;

		case fugio::ImageInterface::FORMAT_RGB8:
			switch( mDestinationFormat )
			{
				case fugio::ImageInterface::FORMAT_GRAY8:
					CvtCod = CV_RGB2GRAY;
					break;

				case fugio::ImageInterface::FORMAT_HSV8:
					CvtCod = CV_RGB2HSV;
					break;

				default:
					break;
			}
			break;

		case fugio::ImageInterface::FORMAT_RGBA8:
			switch( mDestinationFormat )
			{
				case fugio::ImageInterface::FORMAT_RGB8:
					CvtCod = CV_RGBA2RGB;
					break;

				default:
					break;
			}
			break;

		case fugio::ImageInterface::FORMAT_BGR8:
			switch( mDestinationFormat )
			{
				case fugio::ImageInterface::FORMAT_GRAY8:
					CvtCod = CV_BGR2GRAY;
					break;

				case fugio::ImageInterface::FORMAT_RGB8:
					CvtCod = CV_BGR2RGB;
					break;

				case fugio::ImageInterface::FORMAT_HSV8:
					CvtCod = CV_BGR2HSV;
					break;

				default:
					break;
			}
			break;

		case fugio::ImageInterface::FORMAT_BGRA8:
			switch( mDestinationFormat )
			{
				case fugio::ImageInterface::FORMAT_GRAY8:
					CvtCod = CV_BGRA2GRAY;
					break;

				case fugio::ImageInterface::FORMAT_RGB8:
					CvtCod = CV_BGRA2RGB;
					break;

				default:
					break;
			}
			break;

		case fugio::ImageInterface::FORMAT_YUYV422:
			switch( mDestinationFormat )
			{
				case fugio::ImageInterface::FORMAT_RGB8:
					CvtCod = CV_YUV2RGB_Y422;
					break;

				case fugio::ImageInterface::FORMAT_GRAY8:
					CvtCod = CV_YUV2GRAY_Y422;
					break;

				default:
					break;
			}
			break;

		case fugio::ImageInterface::FORMAT_GRAY8:
		case fugio::ImageInterface::FORMAT_GRAY16:
			switch( mDestinationFormat )
			{
				case fugio::ImageInterface::FORMAT_RGB8:
					CvtCod = CV_GRAY2RGB;
					break;

				case fugio::ImageInterface::FORMAT_RGBA8:
					CvtCod = CV_GRAY2RGBA;
					break;

				case fugio::ImageInterface::FORMAT_BGR8:
					CvtCod = CV_GRAY2BGR;
					break;

				case fugio::ImageInterface::FORMAT_BGRA8:
					CvtCod = CV_GRAY2BGRA;
					break;

				default:
					break;
			}
			break;

		default:
			mNode->setStatus( NodeInterface::Error );
			mNode->setStatusMessage( tr( "Unknown source format: %1" ).arg( SrcImg->format() ) );
			return;
	}

	if( CvtCod == -1 )
	{
		mNode->setStatus( NodeInterface::Error );
		mNode->setStatusMessage( tr( "Can't convert input to %1" ).arg( mDestinationFormat ) );

		return;
	}
	else if( mNode->status() != NodeInterface::Initialised )
	{
		mNode->setStatus( NodeInterface::Initialised );
		mNode->setStatusMessage( QString() );
	}

	fugio::Performance	P( mNode, "inputsUpdated", pTimeStamp );

	const int SrcDep = MatSrc.depth();

	if( SrcDep != CV_8U && SrcDep != CV_8S )
	{
		cv::Mat		MatTmp;

		cv::cvtColor( MatSrc, MatTmp, CvtCod );

		MatTmp.convertTo( mMatImg, CV_8U );
	}
	else
	{
		cv::cvtColor( MatSrc, mMatImg, CvtCod );
	}

	OpenCVPlugin::mat2image( mMatImg, mOutputImage, mDestinationFormat );
#endif

	pinUpdated( mPinOutputImage );
}
