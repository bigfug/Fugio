#include "kinectnode.h"

#include <QSettings>
#include <QDebug>

#include <QVector2D>
#include <QVector3D>
#include <QVector4D>

#include <QPushButton>

#include <fugio/core/uuid.h>

#include <fugio/global_interface.h>
#include "fugio/context_interface.h"
#include <fugio/context_signals.h>
#include <fugio/image/uuid.h>
#include <fugio/math/uuid.h>
#include <fugio/performance.h>

template <class T>
void SafeRelease(T& IUnk)
{
	if (IUnk)
	{
		IUnk->Release();
		IUnk = 0;
	}
}

KinectNode::KinectNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mLastElevationCommand( 0 ), mLastElevationUpdate( 0 ), mSensorIndex( -1 )
{
	FUGID( PIN_INPUT_INDEX,			"c15d663c-b17e-4ed6-b51d-dfe7b82216f2" );
	FUGID( PIN_INPUT_ELEVATION,		"60683ff6-a8c6-43d4-b0ba-164cf18a60ec" );
	FUGID( PID_OUTPUT_CAMERA,		"ac6f2365-9915-4e39-a16f-28a72a9975b8" );
	FUGID( PIN_OUTPUT_DEPTH,		"77bc0265-9f40-48b6-bdd2-f331f48b73fe" );
	FUGID( PIN_OUTPUT_USER,			"BEA42409-77CF-4246-B931-F12F233C6FD0" );
	FUGID( PIN_OUTPUT_FLOOR_PLANE,	"55d04d46-d940-49f6-b890-fba91939f7a9" );
	FUGID( PIN_OUTPUT_SKELETON,		"49914f5b-059c-4b93-a114-294e703ea2db" );
	FUGID( PIN_OUTPUT_ELEVATION,	"3c3f841c-8000-428f-a24d-af757295d2e2" );

	mPinInputIndex = pinInput( "Index", PIN_INPUT_INDEX );

	mPinInputElevation = pinInput( "Elevation", PIN_INPUT_ELEVATION );

	mPinInputIndex->registerPinInputType( PID_INTEGER );
	mPinInputElevation->registerPinInputType( PID_INTEGER );

	mValOutputCamera   = pinOutput<fugio::VariantInterface *>( "Camera", mPinOutputCamera, PID_IMAGE, PID_OUTPUT_CAMERA );

	mValOutputDepth    = pinOutput<fugio::VariantInterface *>( "Depth", mPinOutputDepth, PID_IMAGE, PIN_OUTPUT_DEPTH );

	mValOutputUser     = pinOutput<fugio::VariantInterface *>( "User", mPinOutputUser, PID_IMAGE, PIN_OUTPUT_USER );

	mValOutputElevation = pinOutput<fugio::VariantInterface *>( "Elevation", mPinOutputElevation, PID_INTEGER, PIN_OUTPUT_ELEVATION );

	mValOutputFloorClipPlane = pinOutput<fugio::VariantInterface *>( "Floor Plane", mPinOutputFloorClipPlane, PID_VECTOR4, PIN_OUTPUT_FLOOR_PLANE );

	mValOutputSkeleton = pinOutput<fugio::VariantInterface *>( "Skeleton", mPinOutputSkeleton, PID_VARIANT, PIN_OUTPUT_SKELETON );

	mCameraType = COLOUR;
	mCameraResolution = RES_640x480;

	mDepthResolution = RES_640x480;

	mUserTracking = false;

	mSkeletonTracking = false;
	mSkeletonNearMode = false;
	mSkeletonSeatedMode = false;

#ifdef KINECT_SUPPORTED
	m_pNuiSensor = 0;
	m_pDepthStreamHandle = 0;
	m_pColorStreamHandle = 0;

#if defined( InteractionClient )
	mInteractionStream = 0;
#endif

	mNextColorFrameEvent = INVALID_HANDLE_VALUE;
	mNextDepthFrameEvent = INVALID_HANDLE_VALUE;
	mInteractionEvent    = INVALID_HANDLE_VALUE;
	mSkeletonEvent       = INVALID_HANDLE_VALUE;

	if( ( mNextColorFrameEvent = CreateEvent( NULL, TRUE, FALSE, NULL ) ) != INVALID_HANDLE_VALUE )
	{
		mColourFrameNotifier.setHandle( mNextColorFrameEvent );

		mColourFrameNotifier.setEnabled( true );

		connect( &mColourFrameNotifier, &QWinEventNotifier::activated, this, &KinectNode::colourFrame );
	}

	if( ( mNextDepthFrameEvent = CreateEvent( NULL, TRUE, FALSE, NULL ) ) != INVALID_HANDLE_VALUE )
	{
		mDepthFrameNotifier.setHandle( mNextDepthFrameEvent );

		mDepthFrameNotifier.setEnabled( true );

		connect( &mDepthFrameNotifier, &QWinEventNotifier::activated, this, &KinectNode::depthFrame );
	}

	if( ( mSkeletonEvent = CreateEvent( NULL, TRUE, FALSE, NULL ) ) != INVALID_HANDLE_VALUE )
	{
		mSkeletonFrameNotifier.setHandle( mSkeletonEvent );

		mSkeletonFrameNotifier.setEnabled( true );

		connect( &mSkeletonFrameNotifier, &QWinEventNotifier::activated, this, &KinectNode::skeletonFrame );
	}

	if( ( mInteractionEvent = CreateEvent( NULL, TRUE, FALSE, NULL ) ) != INVALID_HANDLE_VALUE )
	{
		//mEventNotifier.setHandle( mNextFrameEvent );
	}

#endif
}

KinectNode::~KinectNode( void )
{
#ifdef KINECT_SUPPORTED
	deviceClose();

	if( mInteractionEvent != INVALID_HANDLE_VALUE )
	{
		CloseHandle( mInteractionEvent );

		mInteractionEvent = INVALID_HANDLE_VALUE;
	}

	if( mSkeletonEvent != INVALID_HANDLE_VALUE )
	{
		CloseHandle( mSkeletonEvent );

		mSkeletonEvent = INVALID_HANDLE_VALUE;
	}

	if( mNextColorFrameEvent != INVALID_HANDLE_VALUE )
	{
		CloseHandle( mNextColorFrameEvent );

		mNextColorFrameEvent = INVALID_HANDLE_VALUE;
	}

	if( mNextDepthFrameEvent != INVALID_HANDLE_VALUE )
	{
		CloseHandle( mNextDepthFrameEvent );

		mNextDepthFrameEvent = INVALID_HANDLE_VALUE;
	}
#endif
}

bool KinectNode::initialise()
{
	if( !NodeControlBase::initialise() )
	{
		return( false );
	}

#if !defined( KINECT_SUPPORTED )
	mNode->setStatus( fugio::NodeInterface::Error );
	mNode->setStatusMessage( tr( "Kinect not supported" ) );

	return( false );
#else
	connect( mNode->context()->qobject(), SIGNAL(frameStart(qint64)), this, SLOT(frameStart(qint64)) );

	return( true );
#endif
}

bool KinectNode::deinitialise()
{
	mNode->context()->qobject()->disconnect( this );

#ifdef KINECT_SUPPORTED
	deviceClose();
#endif

	return( NodeControlBase::deinitialise() );
}

void KinectNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

#ifdef KINECT_SUPPORTED
	if( mPinInputIndex->isUpdated( pTimeStamp ) )
	{
		deviceClose();

		deviceOpen( variant( mPinInputIndex ).toInt() );
	}

	LONG		CamAng = qBound( -27, variant( mPinInputElevation ).toInt(), 27 );

	if( CamAng != mSensorElevation )
	{
		mSensorElevation = CamAng;

		mLastElevationUpdate = pTimeStamp;
	}
#endif
}

void KinectNode::frameStart( qint64 pTimeStamp )
{
	/*
#if !defined( KINECT_SUPPORTED )
	Q_UNUSED( pTimeStamp )
#else
	if( !m_pNuiSensor )
	{
		return;
	}

	fugio::Performance	Perf( mNode, "frameStart", pTimeStamp );

	HRESULT			hr;

	if( mLastElevationUpdate > mLastElevationCommand && pTimeStamp - mLastElevationCommand >= 1000 )
	{
		//		m_pNuiSensor->NuiCameraElevationSetAngle( mSensorElevation );

		mLastElevationCommand = pTimeStamp;
	}

//	LONG			Elevation;

//	if( ( hr = m_pNuiSensor->NuiCameraElevationGetAngle( &Elevation ) ) >= 0 )
//	{
//		if( mValOutputElevation->variant().toInt() != Elevation )
//		{
//			mValOutputElevation->setVariant( int( Elevation ) );

//			pinUpdated( mPinOutputElevation );
//		}
//	}


#if defined( InteractionClient )
	if( mInteractionEvent != INVALID_HANDLE_VALUE && WaitForSingleObject( mInteractionEvent, 0 ) == WAIT_OBJECT_0 )
	{
		NUI_INTERACTION_FRAME	InteractionFrame;

		hr = mInteractionStream->GetNextFrame( 0, &InteractionFrame );

		if( SUCCEEDED( hr ) )
		{
			for( int s = 0 ; s < NUI_SKELETON_COUNT ; s++ )
			{
				const NUI_USER_INFO	&UserInfo = InteractionFrame.UserInfos[ s ];

				for( int h = 0 ; h < NUI_USER_HANDPOINTER_COUNT ; h++ )
				{
					const NUI_HANDPOINTER_INFO &HandInfo = UserInfo.HandPointerInfos[ h ];

					if( HandInfo.HandEventType != NUI_HANDPOINTER_STATE_NOT_TRACKED )
					{
						qDebug() << s << ( HandInfo.HandType == NUI_HAND_TYPE_LEFT ? "LEFT" : "RIGHT" ) << HandInfo.HandEventType;
					}
				}
			}
		}

		ResetEvent( mInteractionEvent );
	}
#endif

#endif
*/
}

#ifdef KINECT_SUPPORTED

QSize KinectNode::nuiSize( NUI_IMAGE_RESOLUTION pNuiRes )
{
	QSize			S;

	switch( pNuiRes )
	{
		case NUI_IMAGE_RESOLUTION_80x60:	S = QSize( 80, 60 );	break;
		case NUI_IMAGE_RESOLUTION_320x240:	S = QSize( 320, 240 );	break;
		case NUI_IMAGE_RESOLUTION_640x480:	S = QSize( 640, 480 );	break;
		case NUI_IMAGE_RESOLUTION_1280x960:	S = QSize( 1280, 960 );	break;
		case NUI_IMAGE_RESOLUTION_INVALID:	break;
	}

	return( S );
}

fugio::ImageFormat KinectNode::nuiFormat( NUI_IMAGE_TYPE pNuiType )
{
	fugio::ImageFormat		ImgFmt = fugio::ImageFormat::UNKNOWN;

	switch( pNuiType )
	{
		case NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX:		break;
		case NUI_IMAGE_TYPE_COLOR:						ImgFmt = fugio::ImageFormat::BGRA8;	break;
		case NUI_IMAGE_TYPE_COLOR_YUV:					break;
		case NUI_IMAGE_TYPE_COLOR_RAW_YUV:				break;
		case NUI_IMAGE_TYPE_DEPTH:						ImgFmt = fugio::ImageFormat::GRAY16;	break;
		case NUI_IMAGE_TYPE_COLOR_INFRARED:				ImgFmt = fugio::ImageFormat::GRAY16;	break;
		case NUI_IMAGE_TYPE_COLOR_RAW_BAYER:			break;

	}

	return( ImgFmt );
}

void KinectNode::deviceOpen( int pDeviceIndex )
{
	HRESULT			 hr;

	NUI_IMAGE_RESOLUTION	ColourResolution = NUI_IMAGE_RESOLUTION_INVALID;
	NUI_IMAGE_RESOLUTION	DepthResolution  = NUI_IMAGE_RESOLUTION_INVALID;

	const bool				InfraredCamera = ( mCameraType == INFRARED );

	if( mCameraType != NONE )
	{
		switch( mCameraResolution )
		{
			case RES_640x480:
				ColourResolution = NUI_IMAGE_RESOLUTION_640x480;
				break;
		}
	}

	switch( mDepthResolution )
	{
		case RES_640x480:
			DepthResolution = NUI_IMAGE_RESOLUTION_640x480;
			break;
	}

	int iSensorCount = 0;
	hr = NuiGetSensorCount( &iSensorCount );
	if( FAILED( hr ) ) { return; }

	if( !iSensorCount )
	{
		mNode->setStatus( fugio::NodeInterface::Error );
		mNode->setStatusMessage( tr( "No Kinect sensors detected" ) );

		return;
	}

	INuiSensor		*pNuiSensor = NULL;

	hr = NuiCreateSensorByIndex( pDeviceIndex, &pNuiSensor );
	if( FAILED( hr ) ) { return; }

	hr = pNuiSensor->NuiStatus();

	if( FAILED( hr ) )
	{
		switch( hr )
		{
			case S_NUI_INITIALIZING:
				mNode->setStatusMessage( "The device is connected, but still initializing." );
				break;

			case E_NUI_NOTCONNECTED:
				mNode->setStatusMessage( "The device is not connected." );
				break;

			case E_NUI_NOTGENUINE:
				mNode->setStatusMessage( "The device is not a valid Kinect." );
				break;

			case E_NUI_NOTSUPPORTED:
				mNode->setStatusMessage( "The device is an unsupported model." );
				break;

			case E_NUI_INSUFFICIENTBANDWIDTH:
				mNode->setStatusMessage( "The device is connected to a hub without the necessary bandwidth requirements." );
				break;

			case E_NUI_NOTPOWERED:
				mNode->setStatusMessage( "The device is connected, but unpowered." );
				break;

			case E_NUI_NOTREADY:
				mNode->setStatusMessage( "There was some other unspecified error." );
				break;
		}

		mNode->setStatus( fugio::NodeInterface::Error );

		pNuiSensor->Release();

		return;
	}

	mNode->setStatus( fugio::NodeInterface::Initialised );
	mNode->setStatusMessage( QStringLiteral( "" ) );

	m_pNuiSensor = pNuiSensor;

	DWORD		InitFlags = 0;

	if( ColourResolution != NUI_IMAGE_RESOLUTION_INVALID )
	{
		InitFlags |= NUI_INITIALIZE_FLAG_USES_COLOR;
	}

	if( DepthResolution != NUI_IMAGE_RESOLUTION_INVALID )
	{
		if( mUserTracking )
		{
			InitFlags |= NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX;
		}
		else
		{
			InitFlags |= NUI_INITIALIZE_FLAG_USES_DEPTH;
		}
	}

	if( mSkeletonTracking )
	{
		InitFlags |= NUI_INITIALIZE_FLAG_USES_SKELETON;
	}

	hr = m_pNuiSensor->NuiInitialize( InitFlags );
	if (FAILED(hr) ) { deviceClose(); return; }

	if( mSkeletonTracking )
	{
		DWORD dwSkeletonFlags = 0;

		if( mSkeletonNearMode )
		{
			dwSkeletonFlags |= NUI_SKELETON_TRACKING_FLAG_ENABLE_IN_NEAR_RANGE;
		}

		if( mSkeletonSeatedMode )
		{
			dwSkeletonFlags |= NUI_SKELETON_TRACKING_FLAG_ENABLE_SEATED_SUPPORT;
		}

		hr = NuiSkeletonTrackingEnable( mSkeletonEvent, dwSkeletonFlags );
		if (FAILED(hr) ) { deviceClose(); return; }
	}

#if defined( InteractionClient )
	if( true )
	{
		hr = NuiCreateInteractionStream( m_pNuiSensor, (INuiInteractionClient *)&mInteractionClient, &mInteractionStream );

		if( SUCCEEDED( hr ) )
		{
			hr = mInteractionStream->Enable( mInteractionEvent );
		}
	}
#endif

	if( mDepthResolution != NUI_IMAGE_RESOLUTION_INVALID )
	{
		if( mUserTracking )
		{
			// Open a depth image stream to receive depth frames
			hr = m_pNuiSensor->NuiImageStreamOpen(
					 NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX,
					 DepthResolution,
					 0,
					 2,
					 mNextDepthFrameEvent,
					 &m_pDepthStreamHandle );
			if (FAILED(hr) ) { qWarning() << hr; deviceClose(); return; }
		}
		else
		{
			// Open a depth image stream to receive depth frames
			hr = m_pNuiSensor->NuiImageStreamOpen(
					 NUI_IMAGE_TYPE_DEPTH,
					 DepthResolution,
					 0,
					 2,
					 mNextDepthFrameEvent,
					 &m_pDepthStreamHandle );
			if (FAILED(hr) ) { qWarning() << hr; deviceClose(); return; }
		}
	}

	if( ColourResolution != NUI_IMAGE_RESOLUTION_INVALID )
	{
		if( InfraredCamera )
		{
			// Open a color image stream to receive color frames
			hr = m_pNuiSensor->NuiImageStreamOpen(
					 NUI_IMAGE_TYPE_COLOR_INFRARED,
					 ColourResolution,
					 0,
					 2,
					 mNextColorFrameEvent,
					 &m_pColorStreamHandle );
			if (FAILED(hr) ) { qWarning() << hr; deviceClose(); return; }
		}
		else
		{
			// Open a color image stream to receive color frames
			hr = m_pNuiSensor->NuiImageStreamOpen(
					 NUI_IMAGE_TYPE_COLOR,
					 ColourResolution,
					 0,
					 2,
					 mNextColorFrameEvent,
					 &m_pColorStreamHandle );
			if (FAILED(hr) ) { qWarning() << hr; deviceClose(); return; }
		}
	}

	m_pNuiSensor->NuiCameraElevationGetAngle( &mSensorElevation );
}

void KinectNode::deviceClose()
{
	if( m_pNuiSensor )
	{
		m_pNuiSensor->NuiShutdown();
		m_pNuiSensor->Release();

		m_pNuiSensor = nullptr;

		m_pDepthStreamHandle = 0;
		m_pColorStreamHandle = 0;

#if defined( InteractionClient )
		mInteractionStream = 0;
#endif
	}
}

void KinectNode::colourFrame( HANDLE pHandle )
{
	fugio::Performance	Perf( mNode, "colourFrame", mNode->context()->global()->timestamp() );

	HRESULT			hr;
	NUI_IMAGE_FRAME imageFrame;
	NUI_LOCKED_RECT LockedRect;

	if( m_pColorStreamHandle )
	{
		hr = m_pNuiSensor->NuiImageStreamGetNextFrame( m_pColorStreamHandle, 0, &imageFrame);

		if( SUCCEEDED( hr ) )
		{
			hr = imageFrame.pFrameTexture->LockRect(0, &LockedRect, NULL, 0);

			if( SUCCEEDED( hr ) )
			{
				//-------------------------------------------------------------------------

				//if( mPinOutputCamera->isConnected() )
				{
					fugio::Image	ImgDat = mValOutputCamera->variant().value<fugio::Image>();

					QSize			S = nuiSize( imageFrame.eResolution );

					ImgDat.setSize( S.width(), S.height() );
					ImgDat.setLineSize( 0, LockedRect.Pitch );
					ImgDat.setFormat( nuiFormat( imageFrame.eImageType ) );

					if( imageFrame.eImageType == NUI_IMAGE_TYPE_COLOR )
					{
						const quint8	*SrcPtr = LockedRect.pBits;
						quint8			*DstPtr = ImgDat.internalBuffer( 0 );
						const int		 SrcSze = ImgDat.bufferSize( 0 );

						for( int i = 0 ; i < SrcSze ; i += 4 )
						{
							DstPtr[ i + 0 ] = SrcPtr[ i + 0 ];
							DstPtr[ i + 1 ] = SrcPtr[ i + 1 ];
							DstPtr[ i + 2 ] = SrcPtr[ i + 2 ];
							DstPtr[ i + 3 ] = 0xff;
						}
					}
					else
					{
						memcpy( ImgDat.internalBuffer( 0 ), LockedRect.pBits, ImgDat.bufferSize( 0 ) );
					}

					pinUpdated( mPinOutputCamera );
				}

				//-------------------------------------------------------------------------

				hr = imageFrame.pFrameTexture->UnlockRect(0);
				//if ( FAILED(hr) ) { return; };
			}

			hr = m_pNuiSensor->NuiImageStreamReleaseFrame( m_pColorStreamHandle, &imageFrame );
		}
	}

	ResetEvent( mNextColorFrameEvent );

	mNode->context()->global()->scheduleFrame();
}

void KinectNode::depthFrame( HANDLE pHandle )
{
	fugio::Performance	Perf( mNode, "depthFrame", mNode->context()->global()->timestamp() );

	HRESULT			hr;
	NUI_IMAGE_FRAME imageFrame;
	NUI_LOCKED_RECT LockedRect;

	if( m_pDepthStreamHandle )
	{
		BOOL bNearMode = false;
		INuiFrameTexture * pTexture = NULL;

		hr = m_pNuiSensor->NuiImageStreamGetNextFrame( m_pDepthStreamHandle, 0, &imageFrame );

		if( SUCCEEDED( hr ) )
		{
			hr = m_pNuiSensor->NuiImageFrameGetDepthImagePixelFrameTexture(m_pDepthStreamHandle, &imageFrame, &bNearMode, &pTexture);

			if( SUCCEEDED( hr ) )
			{
				hr = pTexture->LockRect(0, &LockedRect, NULL, 0);

				if( SUCCEEDED( hr ) )
				{
					if( LockedRect.Pitch > 0 )
					{
						const NUI_DEPTH_IMAGE_PIXEL		*DepthImage = reinterpret_cast<const NUI_DEPTH_IMAGE_PIXEL *>( LockedRect.pBits );

						//-------------------------------------------------------------------------

						fugio::Image	DepImg = mValOutputDepth->variant().value<fugio::Image>();
						fugio::Image	UsrImg = mValOutputUser->variant().value<fugio::Image>();

						QSize			S = nuiSize( imageFrame.eResolution );

						DepImg.setSize( S.width(), S.height() );
						DepImg.setLineSize( 0, S.width() * 2 );
						DepImg.setFormat( fugio::ImageFormat::GRAY16 );

						UsrImg.setSize( S.width(), S.height() );
						UsrImg.setLineSize( 0, S.width() * 2 );
						UsrImg.setFormat( fugio::ImageFormat::GRAY16 );

						quint16		*DepDst = reinterpret_cast<quint16 *>( DepImg.internalBuffer( 0 ) );
						quint16		*UsrDst = reinterpret_cast<quint16 *>( UsrImg.internalBuffer( 0 ) );

						for( int y = 0 ; y < S.height() ; y++ )
						{
							const NUI_DEPTH_IMAGE_PIXEL		*SRC = &DepthImage[ y * S.width() ];
							quint16							*DEP = &DepDst[ y * S.width() ];
							quint16							*USR = &UsrDst[ y * S.width() ];

							for( int x = 0 ; x < S.width() ; x++ )
							{
								DEP[ x ] = SRC[ x ].depth;
								USR[ x ] = SRC[ x ].playerIndex;
							}
						}

						pinUpdated( mPinOutputDepth );

						if( imageFrame.eImageType == NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX )
						{
							pinUpdated( mPinOutputUser );
						}
					}

					//-------------------------------------------------------------------------

	#if defined( InteractionClient )
					if( mInteractionStream )
					{
						hr = mInteractionStream->ProcessDepth( LockedRect.size, PBYTE( LockedRect.pBits ), imageFrame.liTimeStamp );
					}
	#endif

					//-------------------------------------------------------------------------

					hr = imageFrame.pFrameTexture->UnlockRect(0);
				}
			}

			hr = m_pNuiSensor->NuiImageStreamReleaseFrame( m_pDepthStreamHandle, &imageFrame );
		}
	}

	ResetEvent( mNextDepthFrameEvent );
}

void KinectNode::skeletonFrame( HANDLE pHandle )
{
	fugio::Performance	Perf( mNode, "skeletonFrame", mNode->context()->global()->timestamp() );

	HRESULT			 hr;

	static NUI_TRANSFORM_SMOOTH_PARAMETERS defaultParams = {0.5f, 0.5f, 0.5f, 0.05f, 0.04f};

	NUI_SKELETON_FRAME		SkeletonFrame;

	hr = m_pNuiSensor->NuiSkeletonGetNextFrame( 0, &SkeletonFrame );

	if( SUCCEEDED( hr ) )
	{
		QVector4D		FloorClipPlane( SkeletonFrame.vFloorClipPlane.x,
										SkeletonFrame.vFloorClipPlane.y,
										SkeletonFrame.vFloorClipPlane.z,
										SkeletonFrame.vFloorClipPlane.w );

		if( FloorClipPlane != mValOutputFloorClipPlane->variant().value<QVector4D>() )
		{
			mValOutputFloorClipPlane->setVariant( FloorClipPlane );

			pinUpdated( mPinOutputFloorClipPlane );
		}

		// Smooth the skeleton joint positions
		m_pNuiSensor->NuiTransformSmooth( &SkeletonFrame, &defaultParams );

		QVariantList		SklLst;

		for( int i = 0 ; i < NUI_SKELETON_COUNT ; i++ )
		{
			const NUI_SKELETON_DATA &Skeleton = SkeletonFrame.SkeletonData[ i ];

			QVariantMap		SklMap;

			SklMap.insert( "state", Skeleton.eTrackingState );
			SklMap.insert( "id", int( Skeleton.dwTrackingID ) );

			if( Skeleton.eTrackingState != NUI_SKELETON_NOT_TRACKED )
			{
				SklMap.insert( "position", QVector3D( Skeleton.Position.x, Skeleton.Position.y, Skeleton.Position.z ) );
			}
			else
			{
				SklMap.insert( "position", QVector3D() );
			}

			QVariantList		SklPos;
			QVariantList		SklState;
			QVariantList		SklColour;
			QVariantList		SklDepth;

			if( Skeleton.eTrackingState == NUI_SKELETON_TRACKED )
			{
				for( int j = 0 ; j < NUI_SKELETON_POSITION_COUNT ; j++ )
				{
					const Vector4	&v1 = Skeleton.SkeletonPositions[ j ];

					SklPos.append( QVector3D( v1.x, v1.y, v1.z ) );
					SklState.append( Skeleton.eSkeletonPositionTrackingState[ j ] );

					QVector2D		v;
					LONG			dx, dy;
					USHORT			dz;
					LONG			cx, cy;

					NuiTransformSkeletonToDepthImage( v1, &dx, &dy, &dz, NUI_IMAGE_RESOLUTION_640x480 );

					SklDepth.append( QVector3D( dx, dy, dz ) );

					if( SUCCEEDED( NuiImageGetColorPixelCoordinatesFromDepthPixelAtResolution( NUI_IMAGE_RESOLUTION_640x480, NUI_IMAGE_RESOLUTION_640x480, nullptr, dx, dy, dz, &cx, &cy ) ) )
					{
						v = QVector2D( cx, cy );
					}

					SklColour.append( v );
				}
			}

			SklMap.insert( "positions", SklPos );
			SklMap.insert( "states", SklState );
			SklMap.insert( "colourCoords", SklColour );
			SklMap.insert( "depthCoords", SklDepth );

			SklLst.append( SklMap );
		}

		mValOutputSkeleton->setVariant( SklLst );

		pinUpdated( mPinOutputSkeleton );

		//-------------------------------------------------------------------------

#if defined( InteractionClient )
		if( mInteractionStream )
		{
			Vector4		V4;

			m_pNuiSensor->NuiAccelerometerGetCurrentReading( &V4 );

			hr = mInteractionStream->ProcessSkeleton( NUI_SKELETON_COUNT, SkeletonFrame.SkeletonData, &V4, SkeletonFrame.liTimeStamp );
		}
#endif
	}

	ResetEvent( mSkeletonEvent );
}

#endif


QWidget *KinectNode::gui()
{
	QPushButton		*GUI = new QPushButton( tr( "Setup..." ) );

	if( GUI )
	{
		connect( GUI, SIGNAL(released()), this, SLOT(editConfig()) );
	}

	return( GUI );
}

void KinectNode::editConfig()
{
	KinectConfigDialog		Dialog;

	Dialog.setColourCameraType( mCameraType );
	Dialog.setColourCameraResolution( mCameraResolution );

	Dialog.setDepthCameraResolution( mDepthResolution );

	Dialog.setDetectUsers( mUserTracking );

	Dialog.setSkeletonEnabled( mSkeletonTracking );
	Dialog.setSkeletonNearMode( mSkeletonNearMode );
	Dialog.setSkeletonSeatedMode( mSkeletonSeatedMode );

	if( Dialog.exec() == QDialog::Accepted )
	{
		mCameraType = Dialog.colourCameraType();
		mCameraResolution = Dialog.colourCameraResolution();

		mDepthResolution = Dialog.depthCameraResolution();

		mUserTracking = Dialog.detectUsers();

		mSkeletonTracking = Dialog.skeletonEnabled();
		mSkeletonNearMode = Dialog.skeletonNearMode();
		mSkeletonSeatedMode = Dialog.skeletonSeatedMode();

#ifdef KINECT_SUPPORTED
		deviceClose();

		deviceOpen( variant( mPinInputIndex ).toInt() );
#endif
	}
}

void KinectNode::loadSettings( QSettings &pSettings )
{
	mCameraType = static_cast<KinectCameraType>( pSettings.value( "camera/type", int( mCameraType ) ).toInt() );
	mCameraResolution = static_cast<KinectCameraResolution>( pSettings.value( "camera/resolution", int( mCameraResolution ) ).toInt() );

	mDepthResolution = static_cast<KinectCameraResolution>( pSettings.value( "depth/resolution", int( mDepthResolution ) ).toInt() );

	mUserTracking = pSettings.value( "user/track", mUserTracking ).toBool();

	mSkeletonTracking = pSettings.value( "skeleton/track", mSkeletonTracking ).toBool();

	mSkeletonNearMode = pSettings.value( "skeleton/near", mSkeletonNearMode ).toBool();

	mSkeletonSeatedMode = pSettings.value( "skeleton/seated", mSkeletonSeatedMode ).toBool();
}

void KinectNode::saveSettings( QSettings &pSettings ) const
{
	pSettings.setValue( "camera/type", int( mCameraType ) );
	pSettings.setValue( "camera/resolution", int( mCameraResolution ) );

	pSettings.setValue( "depth/resolution", int( mDepthResolution ) );

	pSettings.setValue( "user/track", mUserTracking );

	pSettings.setValue( "skeleton/track", mSkeletonTracking );
	pSettings.setValue( "skeleton/near", mSkeletonNearMode );
	pSettings.setValue( "skeleton/seated", mSkeletonSeatedMode );
}

