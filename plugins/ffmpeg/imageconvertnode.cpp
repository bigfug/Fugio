#include "imageconvertnode.h"

#include <QComboBox>
#include <QSettings>

#include <fugio/ffmpeg/uuid.h>
#include <fugio/image/uuid.h>

#include <fugio/context_interface.h>

#include <fugio/performance.h>
#include <fugio/utils.h>

#ifndef INT64_C
#define INT64_C(c) (c ## LL)
#define UINT64_C(c) (c ## ULL)
#endif

#if defined( FFMPEG_SUPPORTED )
extern "C"
{
#include <libavutil/pixfmt.h>
#include <libavutil/imgutils.h>
}
#endif

using namespace fugio;

QMap<ImageInterface::Format,QString>	 ImageConvertNode::mImageFormatMap;

ImageConvertNode::ImageConvertNode( QSharedPointer<NodeInterface> pNode )
	: NodeControlBase( pNode ), mCurrImageFormat( ImageInterface::FORMAT_BGRA8 ), mLastImageFormat( ImageInterface::FORMAT_UNKNOWN )
{
	FUGID( PIN_INPUT_IMAGE, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_INPUT_WIDTH, "261cc653-d7fa-4c34-a08b-3603e8ae71d5" );
	FUGID( PIN_INPUT_HEIGHT, "249f2932-f483-422f-b811-ab679f006381" );
	FUGID( PIN_OUTPUT_IMAGE, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );

#if defined( FFMPEG_SUPPORTED )
	mSrcPixFmt = AV_PIX_FMT_NONE;
	mScaleContext = 0;
#endif

	mPinInputImage = pinInput( "Image", PIN_INPUT_IMAGE );

	mPinInputWidth = pinInput( "Width", PIN_INPUT_WIDTH );

	mPinInputHeight = pinInput( "Height", PIN_INPUT_HEIGHT );

	mValOutputImage = pinOutput<ImageInterface *>( "Image", mPinOutput, PID_IMAGE, PIN_OUTPUT_IMAGE );

	if( mImageFormatMap.isEmpty() )
	{
		mImageFormatMap.insert( ImageInterface::FORMAT_RGB8, "RGB8" );
		mImageFormatMap.insert( ImageInterface::FORMAT_RGBA8, "RGBA8" );
		mImageFormatMap.insert( ImageInterface::FORMAT_BGR8, "BGR8" );
		mImageFormatMap.insert( ImageInterface::FORMAT_BGRA8, "BGRA8" );
		mImageFormatMap.insert( ImageInterface::FORMAT_YUYV422, "YUYV422" );
		mImageFormatMap.insert( ImageInterface::FORMAT_YUV420P, "YUV420P" );
		mImageFormatMap.insert( ImageInterface::FORMAT_GRAY16, "GRAY16" );
		mImageFormatMap.insert( ImageInterface::FORMAT_GRAY8, "GRAY8" );
	}

#if defined( FFMPEG_SUPPORTED )
	memset( &mDstDat, 0, sizeof( mDstDat ) );
	memset( &mDstLen, 0, sizeof( mDstLen ) );
#endif
}

void ImageConvertNode::clearImage()
{
#if defined( FFMPEG_SUPPORTED )
	av_freep( &mDstDat[ 0 ] );

	for( int i = 0 ; i < AV_NUM_DATA_POINTERS ; i++ )
	{
		mDstDat[ i ] = 0;
		mDstLen[ i ] = 0;
	}

	mValOutputImage->setBuffers( mDstDat );
#endif
}

void ImageConvertNode::inputsUpdated( qint64 pTimeStamp )
{
	Performance( mNode, "inputsUpdated", pTimeStamp );

	ImageInterface		*SrcImg = input<ImageInterface *>( mPinInputImage );

	if( !SrcImg || !SrcImg->isValid() )
	{
		return;
	}

#if defined( FFMPEG_SUPPORTED )
	if( mCurrImageFormat != mLastImageFormat )
	{
		if( mScaleContext )
		{
			sws_freeContext( mScaleContext );

			mScaleContext = 0;
		}

		mLastImageFormat = mCurrImageFormat;
	}

	AVPixelFormat		SrcFmt = AV_PIX_FMT_NONE;
	AVPixelFormat		DstFmt = AV_PIX_FMT_NONE;

	if( SrcImg->format() == ImageInterface::FORMAT_INTERNAL )
	{
		SrcFmt = AVPixelFormat( SrcImg->internalFormat() );

		//		AV_PIX_FMT_YUVJ420P,  ///< planar YUV 4:2:0, 12bpp, full scale (JPEG), deprecated in favor of AV_PIX_FMT_YUV420P and setting color_range
		//		AV_PIX_FMT_YUVJ422P,  ///< planar YUV 4:2:2, 16bpp, full scale (JPEG), deprecated in favor of AV_PIX_FMT_YUV422P and setting color_range
		//		AV_PIX_FMT_YUVJ444P,  ///< planar YUV 4:4:4, 24bpp, full scale (JPEG), deprecated in favor of AV_PIX_FMT_YUV444P and setting color_range

		if( SrcFmt == AV_PIX_FMT_YUVJ420P )
		{
			SrcFmt = AV_PIX_FMT_YUV420P;
		}

		if( SrcFmt == AV_PIX_FMT_YUVJ422P )
		{
			SrcFmt = AV_PIX_FMT_YUV422P;
		}

		if( SrcFmt == AV_PIX_FMT_YUVJ444P )
		{
			SrcFmt = AV_PIX_FMT_YUV444P;
		}
	}
	else
	{
		switch( SrcImg->format() )
		{
			case ImageInterface::FORMAT_BGR8:
				SrcFmt = AV_PIX_FMT_BGR24;
				break;

			case ImageInterface::FORMAT_BGRA8:
				SrcFmt = AV_PIX_FMT_BGRA;
				break;

			case ImageInterface::FORMAT_GRAY8:
				SrcFmt = AV_PIX_FMT_GRAY8;
				break;

			case ImageInterface::FORMAT_GRAY16:
				SrcFmt = AV_PIX_FMT_GRAY16;
				break;

			case ImageInterface::FORMAT_RGB8:
				SrcFmt = AV_PIX_FMT_RGB24;
				break;

			case ImageInterface::FORMAT_RGBA8:
				SrcFmt = AV_PIX_FMT_RGBA;
				break;

			case ImageInterface::FORMAT_YUYV422:
				SrcFmt = AV_PIX_FMT_UYVY422;
				break;

			case ImageInterface::FORMAT_YUV420P:
				SrcFmt = AV_PIX_FMT_YUV420P;
				break;

			default:
				break;
		}
	}

	switch( mCurrImageFormat )
	{
		case ImageInterface::FORMAT_BGR8:
			DstFmt = AV_PIX_FMT_BGR24;
			break;

		case ImageInterface::FORMAT_BGRA8:
			DstFmt = AV_PIX_FMT_BGRA;
			break;

		case ImageInterface::FORMAT_GRAY8:
			DstFmt = AV_PIX_FMT_GRAY8;
			break;

		case ImageInterface::FORMAT_GRAY16:
			DstFmt = AV_PIX_FMT_GRAY16;
			break;

		case ImageInterface::FORMAT_RGB8:
			DstFmt = AV_PIX_FMT_RGB24;
			break;

		case ImageInterface::FORMAT_RGBA8:
			DstFmt = AV_PIX_FMT_RGBA;
			break;

		case ImageInterface::FORMAT_YUYV422:
			DstFmt = AV_PIX_FMT_UYVY422;
			break;

		case ImageInterface::FORMAT_YUV420P:
			DstFmt = AV_PIX_FMT_YUV420P;
			break;

		default:
			break;
	}

	QSize			NewSze = mValOutputImage->size();

	if( !pTimeStamp || !mScaleContext || mPinInputWidth->isUpdated( pTimeStamp ) || mPinInputHeight->isUpdated( pTimeStamp ) )
	{
		qreal			 Xscl = variant( mPinInputWidth  ).toReal();
		qreal			 Yscl = variant( mPinInputHeight ).toReal();

		if( Xscl <= 1.0 )
		{
			Xscl *= qreal( SrcImg->width() );
		}

		if( Yscl <= 1.0 )
		{
			Yscl *= qreal( SrcImg->height() );
		}

		NewSze = QSize( Xscl, Yscl );

		if( !NewSze.width() && !NewSze.height() )
		{
			NewSze = SrcImg->size();
		}
		else if( !NewSze.width() || !NewSze.height() )
		{
			// Adapted from QImage::scaledToWidth()/scaledToHeight()

			int	ws = SrcImg->width();
			int hs = SrcImg->height();
			qreal factor;

			if( NewSze.width() > 0 && NewSze.height() == 0 )
			{
				factor = (qreal) NewSze.width() / SrcImg->width();
			}
			else
			{
				factor = (qreal) NewSze.height() / SrcImg->height();
			}

			QTransform wm = QTransform::fromScale(factor, factor);
			QTransform mat = trueMatrix(wm, ws, hs);

			int hd = int(qAbs(mat.m22()) * hs + 0.9999);
			int wd = int(qAbs(mat.m11()) * ws + 0.9999);

			NewSze = QSize( wd, hd );
		}
	}

	if( !NewSze.isValid() )
	{
		return;
	}

	if( mScaleContext && (
				SrcFmt != mSrcPixFmt ||
				NewSze != mValOutputImage->size() ||
				mCurrImageFormat != mValOutputImage->format() ) )
	{
		sws_freeContext( mScaleContext );

		mScaleContext = 0;

		clearImage();
	}

	if( !mScaleContext )
	{
		if( sws_isSupportedInput( SrcFmt ) <= 0 )
		{
			return;
		}

		if( sws_isSupportedOutput( DstFmt ) <= 0 )
		{
			return;
		}
	}

	if( !mScaleContext )
	{
		if( av_image_alloc( mDstDat, mDstLen, NewSze.width(), NewSze.height(), DstFmt, 4 ) < 0 )
		{
			return;
		}

		mValOutputImage->setFormat( mCurrImageFormat );
		mValOutputImage->setSize( NewSze.width(), NewSze.height() );
		mValOutputImage->setLineSizes( mDstLen );
		mValOutputImage->setBuffers( mDstDat );

		if( ( mScaleContext = sws_getContext( SrcImg->width(), SrcImg->height(), SrcFmt, NewSze.width(), NewSze.height(), DstFmt, 0, NULL, NULL, NULL ) ) == 0 )
		{
			clearImage();

			return;
		}

		mSrcPixFmt = SrcFmt;
	}

	if( !mScaleContext )
	{
		return;
	}

	sws_scale( mScaleContext, SrcImg->buffers(), SrcImg->lineSizes(), 0, SrcImg->height(), mDstDat, mDstLen );

	pinUpdated( mPinOutput );
#endif
}

QWidget *ImageConvertNode::gui()
{
	QComboBox		*GUI = new QComboBox();

	if( GUI )
	{
		GUI->addItems( mImageFormatMap.values() );

		GUI->setCurrentText( mImageFormatMap.value( mCurrImageFormat ) );

		connect( GUI, SIGNAL(currentIndexChanged(QString)), this, SLOT(formatChanged(QString)) );
	}

	return( GUI );
}

void ImageConvertNode::loadSettings(QSettings &pSettings)
{
	QString	FmtNam = pSettings.value( "format", mImageFormatMap.value( mCurrImageFormat ) ).toString();

	mCurrImageFormat = mImageFormatMap.key( FmtNam, mCurrImageFormat );
}

void ImageConvertNode::saveSettings( QSettings &pSettings ) const
{
	pSettings.setValue( "format", mImageFormatMap.value( mCurrImageFormat ) );
}

void ImageConvertNode::formatChanged(const QString &pFormat)
{
	mCurrImageFormat = mImageFormatMap.key( pFormat );

	mNode->context()->updateNode( mNode );
}
