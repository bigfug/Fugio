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

extern "C"
{
	#include <libavutil/pixfmt.h>
	#include <libavutil/imgutils.h>
}

using namespace fugio;

QMap<ImageInterface::Format,QString>	 ImageConvertNode::mImageFormatMap;

ImageConvertNode::ImageConvertNode( QSharedPointer<NodeInterface> pNode )
	: NodeControlBase( pNode ), mCurrImageFormat( ImageInterface::FORMAT_BGRA8 ), mLastImageFormat( ImageInterface::FORMAT_UNKNOWN ), mScaleContext( 0 ),
	  mSrcPixFmt( AV_PIX_FMT_NONE )
{
	mPinInput = pinInput( "Image" );

	mOutput = pinOutput<ImageInterface *>( "Image", mPinOutput, PID_IMAGE );

	if( mImageFormatMap.isEmpty() )
	{
		mImageFormatMap.insert( ImageInterface::FORMAT_BGR8, "BGR8" );
		mImageFormatMap.insert( ImageInterface::FORMAT_RGB8, "RGB8" );
		mImageFormatMap.insert( ImageInterface::FORMAT_BGRA8, "BGRA8" );
	}

	memset( &mDstDat, 0, sizeof( mDstDat ) );
	memset( &mDstLen, 0, sizeof( mDstLen ) );
}

void ImageConvertNode::clearImage()
{
	av_freep( &mDstDat[ 0 ] );

	for( int i = 0 ; i < AV_NUM_DATA_POINTERS ; i++ )
	{
		mDstDat[ i ] = 0;
		mDstLen[ i ] = 0;
	}

	mOutput->setBuffers( mDstDat );
}

void ImageConvertNode::inputsUpdated( qint64 pTimeStamp )
{
	Performance( mNode, "inputsUpdated", pTimeStamp );

	ImageInterface		*SrcImg = input<ImageInterface *>( mPinInput );

	if( !SrcImg || !SrcImg->isValid() )
	{
		return;
	}

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

			default:
				break;
		}
	}

	switch( mCurrImageFormat )
	{
		case ImageInterface::FORMAT_BGR8:
			DstFmt = AV_PIX_FMT_BGR8;
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
			DstFmt = AV_PIX_FMT_RGB8;
			break;

		case ImageInterface::FORMAT_RGBA8:
			DstFmt = AV_PIX_FMT_RGBA;
			break;

		default:
			break;
	}

	if( mScaleContext && (
		SrcFmt != mSrcPixFmt ||
		SrcImg->width() != mOutput->width() ||
		SrcImg->height() != mOutput->height() ||
		mCurrImageFormat != mOutput->format() ) )
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
		if( av_image_alloc( mDstDat, mDstLen, SrcImg->width(), SrcImg->height(), DstFmt, 32 ) < 0 )
		{
			return;
		}

		mOutput->setFormat( mCurrImageFormat );
		mOutput->setSize( SrcImg->width(), SrcImg->height() );
		mOutput->setLineSizes( mDstLen );
		mOutput->setBuffers( mDstDat );

		if( ( mScaleContext = sws_getContext( SrcImg->width(), SrcImg->height(), SrcFmt, SrcImg->width(), SrcImg->height(), DstFmt, 0, NULL, NULL, NULL ) ) == 0 )
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
}

QWidget *ImageConvertNode::gui()
{
	QComboBox		*GUI = new QComboBox();

	if( GUI )
	{
		GUI->addItems( mImageFormatMap.values() );

		GUI->setCurrentText( mImageFormatMap.value( mCurrImageFormat ) );
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
