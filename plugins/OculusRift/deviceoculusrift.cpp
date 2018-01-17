#include "deviceoculusrift.h"

#include <QCoreApplication>

#if defined( OCULUS_PLUGIN_SUPPORTED ) && defined( Q_OS_WIN )

//#include "wglext.h"
#include <fugio/opengl/uuid.h>

#include "oculusriftplugin.h"

//bool WGLExtensionSupported(const char *extension_name)
//{
//	// this is pointer to function which returns pointer to string with list of all wgl extensions
//	PFNWGLGETEXTENSIONSSTRINGEXTPROC _wglGetExtensionsStringEXT = NULL;

//	// determine pointer to wglGetExtensionsStringEXT function
//	_wglGetExtensionsStringEXT = (PFNWGLGETEXTENSIONSSTRINGEXTPROC) wglGetProcAddress("wglGetExtensionsStringEXT");

//	if (strstr(_wglGetExtensionsStringEXT(), extension_name) == NULL)
//	{
//		// string was not found
//		return false;
//	}

//	// extension is supported
//	return true;
//}
#endif

QList<QWeakPointer<DeviceOculusRift>>		 DeviceOculusRift::mDeviceList;

void DeviceOculusRift::deviceInitialise()
{

}

void DeviceOculusRift::deviceDeinitialise()
{
	if( !mDeviceList.isEmpty() )
	{
		mDeviceList.clear();

#if defined( OCULUS_PLUGIN_SUPPORTED )
		ovr_Shutdown();
#endif
	}
}

void DeviceOculusRift::devicePacketStart( qint64 pTimeStamp )
{
#if !defined( OCULUS_PLUGIN_SUPPORTED )
	Q_UNUSED( pTimeStamp )
#endif

	if( mDeviceList.isEmpty() )
	{
		return;
	}

	for( int i = 0 ; i < mDeviceList.size() ; )
	{
		QSharedPointer<DeviceOculusRift>	SP = mDeviceList[ i ].toStrongRef();

		if( !SP )
		{
			mDeviceList.removeAt( i );

			continue;
		}

#if defined( OCULUS_PLUGIN_SUPPORTED )
		SP->globalFrameStart( pTimeStamp );
#endif

		i++;
	}

	if( mDeviceList.isEmpty() )
	{
#if defined( OCULUS_PLUGIN_SUPPORTED )
		ovr_Shutdown();
#endif
	}
}

void DeviceOculusRift::devicePacketEnd()
{

}

void DeviceOculusRift::deviceCfgSave(QSettings &pDataStream)
{
	pDataStream.beginGroup( "oculus-rift" );

	pDataStream.beginWriteArray( "devices", mDeviceList.size() );

	for( int i = 0 ; i < mDeviceList.size() ; i++ )
	{
		pDataStream.setArrayIndex( i );

		//DeviceOpenGLOutput *DevCur = mDeviceList[ i ];

		//DevCur->cfgSave( pDataStream );

		//DevCur->cfgSavePins( pDataStream );

		//pDataStream.setValue( "path", DevCur->path() );

		//pDataStream.setValue( "enabled", DevCur->isEnabled() );
	}

	pDataStream.endArray();

	pDataStream.endGroup();
}

void DeviceOculusRift::deviceCfgLoad( QSettings &pDataStream )
{
	QSharedPointer<DeviceOculusRift>	DevCur;

	mDeviceList.clear();

	pDataStream.beginGroup( "oculus-rift" );

	int		DevCnt = pDataStream.beginReadArray( "devices" );

	for( int i = 0 ; i < DevCnt ; i++ )
	{
		pDataStream.setArrayIndex( i );

		if( ( DevCur = newDevice() ) == nullptr )
		{
			continue;
		}

		//DevCur->setPath( pDataStream.value( "path", DevCur->path() ).toString() );

		//DevCur->cfgLoad( pDataStream );

		//DevCur->cfgLoadPins( pDataStream, DevCur );

		if( pDataStream.value( "enabled", false ).toBool() )
		{
			//DevCur->setEnabled( true );
		}
	}

	pDataStream.endArray();

	pDataStream.endGroup();
}

QSharedPointer<DeviceOculusRift> DeviceOculusRift::newDevice()
{
	if( !mDeviceList.isEmpty() )
	{
		return( mDeviceList.first() );
	}

#if defined( OCULUS_PLUGIN_SUPPORTED )
	ovrInitParams initParams = { ovrInit_RequestVersion, OVR_MINOR_VERSION, NULL, 0, 0 };

	ovrResult result = ovr_Initialize( &initParams );

	if( OVR_FAILURE( result ) )
	{
		ovrErrorInfo errorInfo;

		ovr_GetLastErrorInfo( &errorInfo );

		qWarning() << "ovr_Initialize failed:" << errorInfo.ErrorString;

		return( QSharedPointer<DeviceOculusRift>() );
	}

	QStringList		SL;

	SL << "EngineName: Fugio";
	SL << QString( "EngineVersion: %1" ).arg( qApp->applicationVersion() );
	SL << "EnginePluginName: fugio-oculusrift";
	SL << "EnginePluginVersion: 1.0.0";
	SL << "EngineEditor: true";

	QString			IC = SL.join( '\n' );

	ovr_IdentifyClient( IC.toLatin1().constData() );

#else
	qWarning() << "Oculus Rift plugin is not supported";
#endif

	QSharedPointer<DeviceOculusRift>	NewDev = QSharedPointer<DeviceOculusRift>( new DeviceOculusRift() );

	if( NewDev )
	{
		mDeviceList.append( NewDev );

		//gApp->devices().registerDevice( NewDev );
	}

	return( NewDev );
}

void DeviceOculusRift::delDevice( QSharedPointer<DeviceOculusRift> pDelDev )
{
	//	if( pDelDev != 0 )
	//	{
	//		//gApp->devices().unregisterDevice( pDelDev );

	//		mDeviceList.removeAll( pDelDev );

	//		delete pDelDev;
	//	}
}

QList<QWeakPointer<DeviceOculusRift>> DeviceOculusRift::devices()
{
	return( mDeviceList );
}

#if defined(_WIN32)
	#include <dxgi.h> // for GetDefaultAdapterLuid
	#pragma comment(lib, "dxgi.lib")
#endif

#if defined( OCULUS_PLUGIN_SUPPORTED )
static ovrGraphicsLuid GetDefaultAdapterLuid()
{
	ovrGraphicsLuid luid = ovrGraphicsLuid();

	#if defined(_WIN32)
		IDXGIFactory* factory = nullptr;

		if (SUCCEEDED(CreateDXGIFactory(IID_PPV_ARGS(&factory))))
		{
			IDXGIAdapter* adapter = nullptr;

			if (SUCCEEDED(factory->EnumAdapters(0, &adapter)))
			{
				DXGI_ADAPTER_DESC desc;

				adapter->GetDesc(&desc);
				memcpy(&luid, &desc.AdapterLuid, sizeof(luid));
				adapter->Release();
			}

			factory->Release();
		}
	#endif

	return luid;
}
#endif

//-----------------------------------------------------------------------------
// Instance

DeviceOculusRift::DeviceOculusRift( void )
#if defined( OCULUS_PLUGIN_SUPPORTED )
	: mHMD( 0 ), frameIndex( 0 ), mirrorFBO( 0 ), mirrorTexture( nullptr )
#endif
{
#if defined( OCULUS_PLUGIN_SUPPORTED )
	memset( &mSessionStatus, 0, sizeof( mSessionStatus ) );

	eyeDepthBuffer[ 0 ] = nullptr;
	eyeDepthBuffer[ 1 ] = nullptr;

	eyeRenderTexture[ 0 ] = nullptr;
	eyeRenderTexture[ 1 ] = nullptr;
#endif
}

DeviceOculusRift::~DeviceOculusRift()
{
#if defined( OCULUS_PLUGIN_SUPPORTED )
	deviceClose();
#endif
}

void DeviceOculusRift::drawStart()
{
#if defined( OCULUS_PLUGIN_SUPPORTED )
	if( !mHMD && !deviceOpen() )
	{
		return;
	}

	glGetIntegerv( GL_FRAMEBUFFER_BINDING, &CurFBO );

	ovrHmdDesc hmdDesc = ovr_GetHmdDesc( mHMD );

	EyeRenderDesc[0] = ovr_GetRenderDesc(mHMD, ovrEye_Left, hmdDesc.DefaultEyeFov[0]);
	EyeRenderDesc[1] = ovr_GetRenderDesc(mHMD, ovrEye_Right, hmdDesc.DefaultEyeFov[1]);

	// Get eye poses, feeding in correct IPD offset
	ViewOffset[ 0 ] = EyeRenderDesc[ 0 ].HmdToEyeOffset;
	ViewOffset[ 1 ] = EyeRenderDesc[ 1 ].HmdToEyeOffset;

//	//ovrFrameTiming   ftiming = ovr_GetFrameTiming(mHMD, 0);
//	ovrTrackingState hmdState = ovr_GetTrackingState( mHMD, 0, ovrFalse ); //ftiming.DisplayMidpointSeconds);

	//ovr_CalcEyePoses( hmdState.HeadPose.ThePose, ViewOffset, EyeRenderPose );
	ovr_GetEyePoses( mHMD, frameIndex, ovrTrue, ViewOffset, EyeRenderPose, &sensorSampleTime );
#endif
}

void DeviceOculusRift::drawEnd()
{
#if defined( OCULUS_PLUGIN_SUPPORTED )
	if( !mHMD )
	{
		return;
	}

	ovrHmdDesc hmdDesc = ovr_GetHmdDesc( mHMD );

	// Do distortion rendering, Present and flush/sync

	ovrLayerEyeFov ld;
	ld.Header.Type  = ovrLayerType_EyeFov;
	ld.Header.Flags = ovrLayerFlag_TextureOriginAtBottomLeft;   // Because OpenGL.

	for (int eye = 0; eye < 2; ++eye)
	{
		ld.ColorTexture[eye] = eyeRenderTexture[eye]->TextureChain;
		ld.Viewport[eye]     = Recti(eyeRenderTexture[eye]->GetSize());
		ld.Fov[eye]          = hmdDesc.DefaultEyeFov[eye];
		ld.RenderPose[eye]   = EyeRenderPose[eye];
		ld.SensorSampleTime  = sensorSampleTime;
	}

	ovrLayerHeader* layers = &ld.Header;
	ovrResult result = ovr_SubmitFrame( mHMD, frameIndex, nullptr, &layers, 1 );
	// exit the rendering loop if submit returns an error, will retry on ovrError_DisplayLost
	if (!OVR_SUCCESS(result))
	{
		//return;
	}

	isVisible = (result == ovrSuccess);

	ovr_GetSessionStatus( mHMD, &mSessionStatus );
//    if (sessionStatus.ShouldQuit)
//        goto Done;
	if (mSessionStatus.ShouldRecenter)
		ovr_RecenterTrackingOrigin( mHMD );

	if( isVisible )
	{
		// Blit mirror texture to back buffer
		glBindFramebuffer(GL_READ_FRAMEBUFFER, mirrorFBO);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, CurFBO);
		GLint w = hmdDesc.Resolution.w;
		GLint h = hmdDesc.Resolution.h;
		glBlitFramebuffer(0, h, w, 0,
						  0, 0, w, h,
						  GL_COLOR_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	}

	frameIndex++;

	if( result == ovrError_DisplayLost )
	{
		deviceClose();
	}
#endif
}

void DeviceOculusRift::drawEyeStart(int pEyeIdx)
{
#if !defined( OCULUS_PLUGIN_SUPPORTED )
	Q_UNUSED( pEyeIdx )
#else
	if( !mHMD )
	{
		return;
	}

	// Switch to eye render target
	eyeRenderTexture[ pEyeIdx ]->SetAndClearRenderSurface( eyeDepthBuffer[ pEyeIdx ] );
#endif
}

void DeviceOculusRift::drawEyeEnd(int pEyeIdx)
{
#if !defined( OCULUS_PLUGIN_SUPPORTED )
	Q_UNUSED( pEyeIdx )
#else
	if( !mHMD )
	{
		return;
	}

	// Avoids an error when calling SetAndClearRenderSurface during next iteration.
	// Without this, during the next while loop iteration SetAndClearRenderSurface
	// would bind a framebuffer with an invalid COLOR_ATTACHMENT0 because the texture ID
	// associated with COLOR_ATTACHMENT0 had been unlocked by calling wglDXUnlockObjectsNV.
	eyeRenderTexture[ pEyeIdx ]->UnsetRenderSurface();

	// Commit changes to the textures so they get picked up frame
	eyeRenderTexture[ pEyeIdx ]->Commit();
#endif
}

#if defined( OCULUS_PLUGIN_SUPPORTED )

ovrFovPort DeviceOculusRift::defaultEyeFOV( int pEyeIdx ) const
{
	ovrHmdDesc hmdDesc = ovr_GetHmdDesc( mHMD );

	return( hmdDesc.DefaultEyeFov[ pEyeIdx ] );
}

void DeviceOculusRift::globalFrameStart( qint64 pTimeStamp )
{
	Q_UNUSED( pTimeStamp )

	if( !mHMD && !deviceOpen() )
	{
		return;
	}

	if( ovr_GetSessionStatus( mHMD, &mSessionStatus ) != ovrSuccess )
	{
		return;
	}

	if( mSessionStatus.ShouldRecenter )
	{
		ovr_RecenterTrackingOrigin( mHMD );
	}

	if( mSessionStatus.DisplayLost )
	{
		deviceClose();
	}
}

bool DeviceOculusRift::deviceOpen()
{
	if( !OculusRiftPlugin::instance()->hasOpenGLContext() )
	{
		return( false );
	}

#if defined( OCULUS_PLUGIN_SUPPORTED )
	ovrHmdDesc hmdDesc;

	hmdDesc = ovr_GetHmdDesc( nullptr );

	if( hmdDesc.Type == ovrHmd_None )
	{
		return( false );
	}

	QSurfaceFormat	Format = QOpenGLContext::currentContext()->format();

	Format.setSwapBehavior( QSurfaceFormat::DoubleBuffer );
	Format.setSwapInterval( 0 );

	QOpenGLContext::currentContext()->setFormat( Format );

//#if defined( Q_OS_WIN )
//	PFNWGLSWAPINTERVALEXTPROC       wglSwapIntervalEXT = NULL;
//	PFNWGLGETSWAPINTERVALEXTPROC    wglGetSwapIntervalEXT = NULL;

//	if (WGLExtensionSupported("WGL_EXT_swap_control"))
//	{
//		// Extension is supported, init pointers.
//		wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC) wglGetProcAddress ("wglSwapIntervalEXT");

//		// this is another function from WGL_EXT_swap_control extension
//		wglGetSwapIntervalEXT = (PFNWGLGETSWAPINTERVALEXTPROC) wglGetProcAddress ("wglGetSwapIntervalEXT");
//	}

//	if( wglSwapIntervalEXT )
//	{
//		wglSwapIntervalEXT( 0 );
//	}
//#endif

	ovrResult result;

	result = ovr_Create( &mHMD, &mLUID );

	//	if( result != ovrSuccess || !mHMD )
	//	{
	//		result = ovr_CreateDebug( ovrHmd_DK2, &mHMD );
	//	}

	if( !OVR_SUCCESS( result ) )
	{
		return( false );
	}

	if (memcmp( &mLUID, &GetDefaultAdapterLuid(), sizeof(mLUID))) // If luid that the Rift is on is not the default adapter LUID...
	{
		qWarning() << "OpenGL supports only the default graphics adapter.";

		deviceClose();

		return( false );
	}

	//ovr_SetInt( mHMD, OVR_PERF_HUD_MODE, int( ovrPerfHud_RenderTiming ) );

	hmdDesc = ovr_GetHmdDesc( mHMD );

	if( hmdDesc.Type == ovrHmd_None )
	{
		deviceClose();

		return( false );
	}

	// Get more details about the HMD.
	ovrSizei resolution = hmdDesc.Resolution;

	qDebug() << "Oculus Rift Resolution:" << resolution.w << "x" << resolution.h;
	// Do something with the HMD.

	// FloorLevel will give tracking poses where the floor height is 0
	result = ovr_SetTrackingOriginType( mHMD, ovrTrackingOrigin_FloorLevel);

	if( OVR_SUCCESS( result ) )
	{
		ovrTrackingState ts  = ovr_GetTrackingState( mHMD, 0, ovr_GetTimeInSeconds() );

		if (ts.StatusFlags & (ovrStatus_OrientationTracked | ovrStatus_PositionTracked))
		{
			ovrPoseStatef pose = ts.HeadPose;

			qDebug() << pose.ThePose.Orientation.x << pose.ThePose.Orientation.y << pose.ThePose.Orientation.z << pose.ThePose.Orientation.w;
		}
	}

	ovrSizei windowSize = { hmdDesc.Resolution.w, hmdDesc.Resolution.h };

	for (int i=0; i<2; i++)
	{
		ovrSizei idealTextureSize = ovr_GetFovTextureSize( mHMD, ovrEyeType( i ), hmdDesc.DefaultEyeFov[i], 1 );
		eyeRenderTexture[i] = new TextureBuffer( mHMD, true, true, idealTextureSize, 1, NULL, 1);
		eyeDepthBuffer[i]   = new DepthBuffer( eyeRenderTexture[i]->GetSize(), 0);

		if( !eyeRenderTexture[ i ]->TextureChain )
		{
			deviceClose();

			return( false );
		}
	}

	qDebug() << "VR Eye Tex Size:" << eyeRenderTexture[ 0 ]->GetSize().w << "x" << eyeRenderTexture[ 0 ]->GetSize().h << eyeRenderTexture[ 1 ]->GetSize().w << "x" << eyeRenderTexture[ 1 ]->GetSize().h;

	ovrMirrorTextureDesc desc;
	memset(&desc, 0, sizeof(desc));
	desc.Width = windowSize.w;
	desc.Height = windowSize.h;
	desc.Format = OVR_FORMAT_R8G8B8A8_UNORM_SRGB;

	// Create mirror texture and an FBO used to copy mirror texture to back buffer
	if( ovr_CreateMirrorTextureGL( mHMD, &desc, &mirrorTexture) != ovrSuccess )
	{
		deviceClose();

		return( false );
	}

	// Configure the mirror read buffer
	GLuint texId;

	ovr_GetMirrorTextureBufferGL( mHMD, mirrorTexture, &texId );

	glGenFramebuffers(1, &mirrorFBO);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, mirrorFBO);
	glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texId, 0);
	glFramebufferRenderbuffer(GL_READ_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

	// FloorLevel will give tracking poses where the floor height is 0
	ovr_SetTrackingOriginType( mHMD, ovrTrackingOrigin_EyeLevel );
#endif

	qDebug() << "Oculus Rift Opened";

	return( true );
}

void DeviceOculusRift::deviceClose()
{
#if defined( OCULUS_PLUGIN_SUPPORTED )
	if( mirrorFBO )
	{
		glDeleteFramebuffers(1, &mirrorFBO);

		mirrorFBO = 0;
	}

	if( mHMD )
	{
		ovr_DestroyMirrorTexture( mHMD, mirrorTexture );

		ovr_DestroyTextureSwapChain( mHMD, eyeRenderTexture[0]->TextureChain );
		ovr_DestroyTextureSwapChain( mHMD, eyeRenderTexture[1]->TextureChain );

		eyeDepthBuffer[ 0 ] = nullptr;
		eyeDepthBuffer[ 1 ] = nullptr;

		eyeRenderTexture[ 0 ] = nullptr;
		eyeRenderTexture[ 1 ] = nullptr;

		ovr_Destroy( mHMD );

		mHMD = nullptr;
	}

	frameIndex = 0;
#endif

	qDebug() << "Oculus Rift Closed";
}

#endif
