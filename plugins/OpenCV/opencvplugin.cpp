#include "opencvplugin.h"

#include <QTranslator>
#include <QApplication>

#include <fugio/image/image_interface.h>

#include "grayscalenode.h"
#include "imagethresholdnode.h"
#include "inpaintnode.h"
#include "videocapturenode.h"
#include "imageconvertnode.h"
#include "imagehomographynode.h"
#include "backgroundsubtractionnode.h"
#include "inrangenode.h"
#include "erodenode.h"
#include "dilatenode.h"
#include "findcontoursnode.h"
#include "resizenode.h"
#include "equalizehistnode.h"
#include "cascadeclassifiernode.h"
#include "momentsnode.h"
#include "distancetransformnode.h"
#include "converttonode.h"
#include "addnode.h"
#include "flipnode.h"

QList<QUuid>				NodeControlBase::PID_UUID;

ClassEntry	OpenCVPlugin::mNodeEntries[] =
{
	ClassEntry( "Add", "OpenCV", NID_OPENCV_ADD, &AddNode::staticMetaObject ),
	ClassEntry( "Cascade Classifier", "OpenCV", NID_OPENCV_CASCADE_CLASSIFER, &CascadeClassifierNode::staticMetaObject ),
	ClassEntry( "ConvertTo", "OpenCV", NID_OPENCV_CONVERT_TO, &ConvertToNode::staticMetaObject ),
	ClassEntry( "Distance Transform", "OpenCV", NID_OPENCV_DISTANCE_TRANSFORM, &DistanceTransformNode::staticMetaObject ),
	ClassEntry( "Dilate", "OpenCV", NID_OPENCV_DILATE, &DilateNode::staticMetaObject ),
	ClassEntry( "Erode", "OpenCV", NID_OPENCV_ERODE, &ErodeNode::staticMetaObject ),
	ClassEntry( "EqualizeHist", "OpenCV", NID_OPENCV_EQUALIZE_HIST, &EqualizeHistNode::staticMetaObject ),
	ClassEntry( "Find Contours", "OpenCV", NID_OPENCV_FIND_CONTOURS, &FindContoursNode::staticMetaObject ),
	ClassEntry( "Flip", "OpenCV", NID_OPENCV_FLIP, &FlipNode::staticMetaObject ),
	ClassEntry( "Grayscale", "OpenCV", NID_OPENCV_GRAYSCALE, &GrayscaleNode::staticMetaObject ),
	ClassEntry( "Threshold (Image)", "OpenCV", NID_OPENCV_IMAGE_THRESHOLD, &ImageThresholdNode::staticMetaObject ),
	ClassEntry( "InPaint", "OpenCV", NID_OPENCV_INPAINT, &InPaintNode::staticMetaObject ),
	ClassEntry( "InRange", "OpenCV", NID_OPENCV_INRANGE, &InRangeNode::staticMetaObject ),
	ClassEntry( "Video Capture", "OpenCV", ClassEntry::Deprecated, NID_OPENCV_VIDEO_CAPTURE, &VideoCaptureNode::staticMetaObject ),		// Unstable!
	ClassEntry( "Image Convert", "OpenCV", NID_OPENCV_IMAGE_CONVERT, &ImageConvertNode::staticMetaObject ),
	ClassEntry( "Image Homography", "OpenCV", NID_OPENCV_IMAGE_HOMOGRAPHY, &ImageHomographyNode::staticMetaObject ),
	ClassEntry( "Background Subtraction", "OpenCV", NID_OPENCV_BACKGROUND_SUBTRACTION, &BackgroundSubtractionNode::staticMetaObject ),
	ClassEntry( "Resize", "OpenCV", ClassEntry::Deprecated, NID_OPENCV_RESIZE, &ResizeNode::staticMetaObject ),
	ClassEntry( "Moments", "OpenCV", NID_OPENCV_MOMENTS, &MomentsNode::staticMetaObject ),
	ClassEntry()
};

ClassEntry OpenCVPlugin::mPinEntries[] =
{
	ClassEntry()
};

OpenCVPlugin::OpenCVPlugin()
	: mApp( 0 )
{
	//-------------------------------------------------------------------------
	// Install translator

	static QTranslator		Translator;

	if( Translator.load( QLocale(), QLatin1String( "fugio_opencv" ), QLatin1String( "_" ), ":/translations" ) )
	{
		qApp->installTranslator( &Translator );
	}
}

OpenCVPlugin::~OpenCVPlugin()
{

}

PluginInterface::InitResult OpenCVPlugin::initialise( fugio::GlobalInterface *pApp, bool pLastChance )
{
	Q_UNUSED( pLastChance )

	mApp = pApp;

	DeviceVideoCapture::deviceInitialise();

	mApp->registerNodeClasses( mNodeEntries );

	mApp->registerPinClasses( mPinEntries );

	return( INIT_OK );
}

void OpenCVPlugin::deinitialise()
{
	mApp->unregisterNodeClasses( mNodeEntries );

	mApp->unregisterPinClasses( mPinEntries );

	DeviceVideoCapture::deviceDeinitialise();
}

#if defined( OPENCV_SUPPORTED )

cv::Mat OpenCVPlugin::image2mat( fugio::ImageInterface *pSrcImg )
{
	cv::Mat		MatImg;

//	FORMAT_YUYV422,

	switch( pSrcImg->format() )
	{
		case fugio::ImageInterface::FORMAT_RGB8:
		case fugio::ImageInterface::FORMAT_BGR8:
		case fugio::ImageInterface::FORMAT_HSV8:
			MatImg = cv::Mat( pSrcImg->size().height(), pSrcImg->size().width(), CV_8UC3, (void *)pSrcImg->buffer( 0 ), pSrcImg->lineSize( 0 ) );
			break;

		case fugio::ImageInterface::FORMAT_RGBA8:
		case fugio::ImageInterface::FORMAT_BGRA8:
			MatImg = cv::Mat( pSrcImg->size().height(), pSrcImg->size().width(), CV_8UC4, (void *)pSrcImg->buffer( 0 ), pSrcImg->lineSize( 0 ) );
			break;

		case fugio::ImageInterface::FORMAT_GRAY8:
			MatImg = cv::Mat( pSrcImg->size().height(), pSrcImg->size().width(), CV_8UC1, (void *)pSrcImg->buffer( 0 ), pSrcImg->lineSize( 0 ) );
			break;

		case fugio::ImageInterface::FORMAT_GRAY16:
			MatImg = cv::Mat( pSrcImg->size().height(), pSrcImg->size().width(), CV_16UC1, (void *)pSrcImg->buffer( 0 ), pSrcImg->lineSize( 0 ) );
			break;

		case fugio::ImageInterface::FORMAT_YUYV422:
		case fugio::ImageInterface::FORMAT_UYVY422:
			MatImg = cv::Mat( pSrcImg->size().height(), pSrcImg->size().width(), CV_8UC2, (void *)pSrcImg->buffer( 0 ), pSrcImg->lineSize( 0 ) );
			break;

		case fugio::ImageInterface::FORMAT_R32F:
			MatImg = cv::Mat( pSrcImg->size().height(), pSrcImg->size().width(), CV_32FC1, (void *)pSrcImg->buffer( 0 ), pSrcImg->lineSize( 0 ) );
			break;

		case fugio::ImageInterface::FORMAT_R32S:
			MatImg = cv::Mat( pSrcImg->size().height(), pSrcImg->size().width(), CV_32SC1, (void *)pSrcImg->buffer( 0 ), pSrcImg->lineSize( 0 ) );
			break;

		default:
			break;
	}

	return( MatImg );
}

void OpenCVPlugin::mat2image( cv::Mat &pSrcMat, fugio::ImageInterface *pDstImg, fugio::ImageInterface::Format pDstFmt )
{
	pDstImg->setSize( pSrcMat.cols, pSrcMat.rows );
	pDstImg->setLineSize( 0, pSrcMat.step );

	if( pDstFmt != fugio::ImageInterface::FORMAT_UNKNOWN )
	{
		pDstImg->setFormat( pDstFmt );
	}
	else
	{
		switch( pSrcMat.type() )
		{
			case CV_8UC1:
				pDstImg->setFormat( fugio::ImageInterface::FORMAT_GRAY8 );
				break;

			case CV_8UC3:
				pDstImg->setFormat( fugio::ImageInterface::FORMAT_RGB8 );
				break;

			case CV_8UC4:
				pDstImg->setFormat( fugio::ImageInterface::FORMAT_RGBA8 );
				break;

			case CV_16UC1:
				pDstImg->setFormat( fugio::ImageInterface::FORMAT_GRAY16 );
				break;

			case CV_32SC1:
				pDstImg->setFormat( fugio::ImageInterface::FORMAT_R32S );
				break;

			case CV_32FC1:
				pDstImg->setFormat( fugio::ImageInterface::FORMAT_R32F );
				break;

#if defined( QT_DEBUG )
			default:
				qDebug() << "Unknown format:" << pSrcMat.type();
				break;
#endif
		}
	}

	pDstImg->setBuffer( 0, pSrcMat.ptr() );
}

#endif // defined( OPENCV_SUPPORTED )
