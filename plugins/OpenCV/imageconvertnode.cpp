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

QMap<QString,fugio::ImageFormat>	 ImageConvertNode::mFormats;


ImageConvertNode::ImageConvertNode( QSharedPointer<fugio::NodeInterface> pNode ) :
	NodeControlBase( pNode ), mDestinationFormat( fugio::ImageFormat::RGB8 )
{
	mPinInputImage = pinInput( "Input" );

	mValOutputImage = pinOutput<fugio::VariantInterface *>( "Output", mPinOutputImage, PID_IMAGE );

	if( mFormats.isEmpty() )
	{
		mFormats[ "RGB8" ] = fugio::ImageFormat::RGB8;
		mFormats[ "RGBA8" ] = fugio::ImageFormat::RGBA8;
		mFormats[ "BGR8" ] = fugio::ImageFormat::BGR8;
		mFormats[ "BGRA8" ] = fugio::ImageFormat::BGRA8;
		mFormats[ "YUYV422" ] = fugio::ImageFormat::YUYV422;
		mFormats[ "UYVY422" ] = fugio::ImageFormat::UYVY422;
		mFormats[ "YUV420P" ] = fugio::ImageFormat::YUV420P;
		mFormats[ "GRAY16" ] = fugio::ImageFormat::GRAY16;
		mFormats[ "GRAY8" ] = fugio::ImageFormat::GRAY8;
		mFormats[ "HSV8" ] = fugio::ImageFormat::HSV8;
	}
}

QWidget *ImageConvertNode::gui( void )
{
	QComboBox		*GUI = new QComboBox();

	for( QMap<QString,fugio::ImageFormat>::const_iterator it = mFormats.begin() ; it != mFormats.end() ; it++ )
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
	fugio::ImageFormat		 ImgFmt = fugio::ImageFormat( mFormats.value( FmtNam, mDestinationFormat ) );

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
	fugio::ImageFormat		 ImgFmt = mFormats.value( pName );

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

	fugio::Image		SrcImg = variant<fugio::Image>( mPinInputImage );

	if( !SrcImg.isValid() )
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

	switch( SrcImg.format() )
	{
		case fugio::ImageFormat::HSV8:
			switch( mDestinationFormat )
			{
				case fugio::ImageFormat::BGR8:
					CvtCod = CV_HSV2BGR;
					break;

				case fugio::ImageFormat::RGB8:
					CvtCod = CV_HSV2RGB;
					break;

				default:
					break;
			}
			break;

		case fugio::ImageFormat::RGB8:
			switch( mDestinationFormat )
			{
				case fugio::ImageFormat::GRAY8:
					CvtCod = CV_RGB2GRAY;
					break;

				case fugio::ImageFormat::HSV8:
					CvtCod = CV_RGB2HSV;
					break;

				default:
					break;
			}
			break;

		case fugio::ImageFormat::RGBA8:
			switch( mDestinationFormat )
			{
				case fugio::ImageFormat::RGB8:
					CvtCod = CV_RGBA2RGB;
					break;

				default:
					break;
			}
			break;

		case fugio::ImageFormat::BGR8:
			switch( mDestinationFormat )
			{
				case fugio::ImageFormat::GRAY8:
					CvtCod = CV_BGR2GRAY;
					break;

				case fugio::ImageFormat::RGB8:
					CvtCod = CV_BGR2RGB;
					break;

				case fugio::ImageFormat::HSV8:
					CvtCod = CV_BGR2HSV;
					break;

				default:
					break;
			}
			break;

		case fugio::ImageFormat::BGRA8:
			switch( mDestinationFormat )
			{
				case fugio::ImageFormat::GRAY8:
					CvtCod = CV_BGRA2GRAY;
					break;

				case fugio::ImageFormat::RGB8:
					CvtCod = CV_BGRA2RGB;
					break;

				default:
					break;
			}
			break;

		case fugio::ImageFormat::YUYV422:
			switch( mDestinationFormat )
			{
				case fugio::ImageFormat::RGB8:
					CvtCod = CV_YUV2RGB_Y422;
					break;

				case fugio::ImageFormat::GRAY8:
					CvtCod = CV_YUV2GRAY_Y422;
					break;

				default:
					break;
			}
			break;

		case fugio::ImageFormat::GRAY8:
		case fugio::ImageFormat::GRAY16:
		case fugio::ImageFormat::R32F:
		case fugio::ImageFormat::R32S:
			switch( mDestinationFormat )
			{
				case fugio::ImageFormat::RGB8:
					CvtCod = CV_GRAY2RGB;
					break;

				case fugio::ImageFormat::RGBA8:
					CvtCod = CV_GRAY2RGBA;
					break;

				case fugio::ImageFormat::BGR8:
					CvtCod = CV_GRAY2BGR;
					break;

				case fugio::ImageFormat::BGRA8:
					CvtCod = CV_GRAY2BGRA;
					break;

				default:
					break;
			}
			break;

		default:
			mNode->setStatus( NodeInterface::Error );
			mNode->setStatusMessage( tr( "Unknown source format: %1" ).arg( SrcImg.format() ) );
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

	fugio::Image	DstImg = mValOutputImage->variant().value<fugio::Image>();

	OpenCVPlugin::mat2image( mMatImg, DstImg );
#endif

	pinUpdated( mPinOutputImage );
}
