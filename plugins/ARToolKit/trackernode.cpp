#include "trackernode.h"

#include <QMatrix4x4>

#include <fugio/artoolkit/uuid.h>
#include <fugio/image/uuid.h>
#include <fugio/file/uuid.h>
#include <fugio/math/uuid.h>
#include <fugio/core/uuid.h>

#include <fugio/performance.h>

#include <fugio/image/image.h>
#include <fugio/file/filename_interface.h>

TrackerNode::TrackerNode( QSharedPointer<fugio::NodeInterface> pNode )
	:NodeControlBase( pNode )
#if defined( ARTOOLKIT_SUPPORTED )
	, mParamLT( 0 ), mHandle( 0 ), mHandle3D( 0 ), mHandlePatt( 0 ),
	  mPixFmt( AR_PIXEL_FORMAT_INVALID ), mPatternIndex( -1 ), mPatFnd( false )
#endif
{
	FUGID( PIN_INPUT_PARAM, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_INPUT_IMAGE, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );
	FUGID( PIN_INPUT_PATTERN, "261cc653-d7fa-4c34-a08b-3603e8ae71d5" );
	FUGID( PIN_INPUT_WIDTH, "0fb3ba87-ff71-41bc-84ee-4f488a18068f" );
	FUGID( PIN_OUTPUT_MATRIX, "249f2932-f483-422f-b811-ab679f006381" );
	FUGID( PIN_OUTPUT_CENTER, "ce8d578e-c5a4-422f-b3c4-a1bdf40facdb" );
	FUGID( PIN_OUTPUT_POINTS, "e6bf944e-5f46-4994-bd51-13c2aa6415b7" );
	FUGID( PIN_OUTPUT_CONFIDENCE, "fd8ac195-827f-43f0-ac4d-1ca8aad83f01" );

	mPinInputParam = pinInput( "Param", PIN_INPUT_PARAM );

	mPinInputParam->registerPinInputType( PID_AR_PARAM );

	mPinInputImage = pinInput( "Image", PIN_INPUT_IMAGE );

	mPinInputImage->registerPinInputType( PID_IMAGE );

	mPinInputPattern = pinInput( "Pattern", PIN_INPUT_PATTERN );

	mPinInputPattern->registerPinInputType( PID_FILENAME );

	mPinInputWidth = pinInput( "Width", PIN_INPUT_WIDTH );

	mPinInputWidth->setValue( 80.0f );

	mPinInputWidth->setDescription( tr( "Width of the marker pattern in mm" ) );

	mValOutputMatrix = pinOutput<fugio::VariantInterface *>( "Matrix", mPinOutputMatrix, PID_MATRIX4, PIN_OUTPUT_MATRIX );

	mValOutputCenter = pinOutput<fugio::VariantInterface *>( "Center", mPinOutputCenter, PID_POINT, PIN_OUTPUT_CENTER );

	mValOutputPoints = pinOutput<fugio::VariantInterface *>( "Points", mPinOutputPoints, PID_POINT, PIN_OUTPUT_POINTS );

	mValOutputConfidence = pinOutput<fugio::VariantInterface *>( "Confidence", mPinOutputConfidence, PID_FLOAT, PIN_OUTPUT_CONFIDENCE );

	mValOutputPoints->setVariantCount( 4 );
}

bool TrackerNode::deinitialise()
{
	freeTracker();

	return( NodeControlBase::deinitialise() );
}

void TrackerNode::updateConfidence( float cf )
{
	if( cf != mValOutputConfidence->variant().toFloat() )
	{
		mValOutputConfidence->setVariant( cf );

		pinUpdated( mPinOutputConfidence );
	}
}

void TrackerNode::inputsUpdated( qint64 pTimeStamp )
{
	fugio::Performance				 Perf( mNode, "inputsUpdated", pTimeStamp );

	fugio::Image					 ImgInt = variant( mPinInputImage ).value<fugio::Image>();

	if( mPinInputParam->isUpdated( pTimeStamp ) )
	{
		freeTracker();
	}

	if( !ImgInt.isEmpty() )
	{
		return;
	}

#if defined( ARTOOLKIT_SUPPORTED )
	if( mPinInputImage->isUpdated( pTimeStamp ) )
	{
		AR_PIXEL_FORMAT			PixFmt = mPixFmt;

		switch( ImgInt.format() )
		{
			case fugio::ImageFormat::RGB8:
				PixFmt = AR_PIXEL_FORMAT_RGB;
				break;

			case fugio::ImageFormat::RGBA8:
				PixFmt = AR_PIXEL_FORMAT_RGBA;
				break;

			case fugio::ImageFormat::BGR8:
				PixFmt = AR_PIXEL_FORMAT_BGR;
				break;

			case fugio::ImageFormat::BGRA8:
				PixFmt = AR_PIXEL_FORMAT_BGRA;
				break;

			case fugio::ImageFormat::UYVY422:
				PixFmt = AR_PIXEL_FORMAT_UYVY;
				break;

			case fugio::ImageFormat::YUYV422:
				PixFmt = AR_PIXEL_FORMAT_YUY2;
				break;

			case fugio::ImageFormat::GRAY8:
				PixFmt = AR_PIXEL_FORMAT_MONO;
				break;

			default:
				PixFmt = AR_PIXEL_FORMAT_INVALID;
				break;
		}

		if( PixFmt != mPixFmt )
		{
			freeTracker();

			mPixFmt = PixFmt;
		}

		if( mPixFmt == AR_PIXEL_FORMAT_INVALID )
		{
			return;
		}

		if( mParamLT )
		{
			const QSize			ImgSze = ImgInt.size();

			if( mParamLT->param.xsize != ImgSze.width() || mParamLT->param.ysize != ImgSze.height() )
			{
				freeTracker();
			}
		}
	}

	if( !mHandle3D )
	{
		fugio::ar::ParamInterface		*PrmInt = input<fugio::ar::ParamInterface *>( mPinInputParam );

		ARParam							 Param = PrmInt->param();

		arParamChangeSize( &Param, ImgInt.size().width(), ImgInt.size().height(), &Param );

		if( !( mParamLT = arParamLTCreate( &Param, AR_PARAM_LT_DEFAULT_OFFSET ) ) )
		{
			return;
		}

		if( !( mHandle = arCreateHandle( mParamLT ) ) )
		{
			freeTracker();

			return;
		}

		if( arSetPixelFormat( mHandle, mPixFmt ) < 0 )
		{
			freeTracker();

			return;
		}

		if( !( mHandle3D = ar3DCreateHandle( &Param ) ) )
		{
			freeTracker();

			return;
		}

		if( !( mHandlePatt = arPattCreateHandle() ) )
		{
			freeTracker();

			return;
		}

		arPattAttach( mHandle, mHandlePatt );
	}

	if( mPatternIndex == -1 || mPinInputPattern->isUpdated( pTimeStamp ) )
	{
		if( mPatternIndex != -1 )
		{
			arPattFree( mHandlePatt, mPatternIndex );

			mPatternIndex = -1;
		}

		QString		PatternFilename = variant( mPinInputPattern ).toString();

		if( !PatternFilename.isEmpty() )
		{
			try
			{
				mPatternIndex = arPattLoad( mHandlePatt, PatternFilename.toLatin1().constData() );
			}
			catch( ... )
			{
				return;
			}
		}
	}

	if( mPatternIndex == -1 )
	{
		return;
	}

	if( mPinInputImage->isUpdated( pTimeStamp ) )
	{
		if( arDetectMarker( mHandle, (ARUint8 *)ImgInt.buffer( 0 ) ) < 0 )
		{
			updateConfidence( -1 );

			return;
		}

		int		MrkCnt = arGetMarkerNum( mHandle );

		if( MrkCnt <= 0 )
		{
			updateConfidence( -1 );

			return;
		}

		int				 MrkIdx = -1;
		int				 MrkSiz = 0;

		ARMarkerInfo	*MrkInf = arGetMarker( mHandle );

		for( int i = 0 ; i < MrkCnt ; i++ )
		{
			if( MrkIdx < 0 || MrkInf[ i ].area > MrkSiz )
			{
				MrkIdx = i;
				MrkSiz = MrkInf[ i ].area;
			}
		}

		if( MrkIdx < 0 )
		{
			updateConfidence( -1 );

			return;
		}

		MrkInf = &MrkInf[ MrkIdx ];

		updateConfidence( MrkInf->cf );

		mValOutputCenter->setVariant( QPointF( MrkInf->pos[ 0 ], MrkInf->pos[ 1 ] ) );

		pinUpdated( mPinOutputCenter );

		float		*PntPtr = (float *)mValOutputPoints->variantArray();

		for( int i = 0 ; i < 4 ; i++ )
		{
			*PntPtr++ = MrkInf->vertex[ i ][ 0 ];
			*PntPtr++ = MrkInf->vertex[ i ][ 1 ];
		}

		pinUpdated( mPinOutputPoints );

		float		PatternWidth = variant( mPinInputWidth ).toFloat();

		if( true )
		{
			if( arGetTransMatSquare( mHandle3D, MrkInf, PatternWidth, mPatTrn ) < 0 )
			{
				return;
			}
		}
		else
		{
			if( arGetTransMatSquareCont( mHandle3D, MrkInf, mPatTrn, PatternWidth, mPatTrn ) < 0 )
			{
				return;
			}
		}

		mPatFnd = true;

		QMatrix4x4		MatOut;

		arglCameraViewRH( mPatTrn, MatOut.data() );

		mValOutputMatrix->setVariant( MatOut );

		pinUpdated( mPinOutputMatrix );
	}
#endif
}

void TrackerNode::freeTracker()
{
#if defined( ARTOOLKIT_SUPPORTED )
	if( mPatternIndex != -1 )
	{
		arPattFree( mHandlePatt, mPatternIndex );

		mPatternIndex = -1;
	}

	if( mHandlePatt )
	{
		arPattDeleteHandle( mHandlePatt );

		mHandlePatt = 0;
	}

	if( mHandle3D )
	{
		ar3DDeleteHandle( &mHandle3D );

		mHandle3D = 0;
	}

	if( mHandle )
	{
		arDeleteHandle( mHandle );

		mHandle = 0;
	}

	arParamLTFree( &mParamLT );
#endif
}


