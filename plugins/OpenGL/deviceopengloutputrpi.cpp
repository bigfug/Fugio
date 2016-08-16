#include "deviceopengloutputrpi.h"

#include <QApplication>

#include "openglplugin.h"

void DeviceOpenGLOutput::deviceInitialise()
{
}

void DeviceOpenGLOutput::deviceDeinitialise()
{
}

void DeviceOpenGLOutput::devicePacketStart( qint64 pTimeStamp )
{
	Q_UNUSED( pTimeStamp )
}

void DeviceOpenGLOutput::devicePacketEnd()
{
}

QSharedPointer<DeviceOpenGLOutput> DeviceOpenGLOutput::newDevice( bool pContextOnly )
{
	Q_UNUSED( pContextOnly )

	QSharedPointer<DeviceOpenGLOutput>	NewDev;

	NewDev = QSharedPointer<DeviceOpenGLOutput>( new DeviceOpenGLOutput() );

	return( NewDev );
}

//-----------------------------------------------------------------------------
// Instance

DeviceOpenGLOutput::DeviceOpenGLOutput( void )
	: mInputEvents( nullptr ), mUpdatePending( false ), mDisplay( 0 ), mConfig( 0 ), mNumConfig( 0 ), mContext( 0 ), mSurface( 0 )
{
	quint32		ScreenWidth, ScreenHeight;

	if( graphics_get_display_size( 0, &ScreenWidth, &ScreenHeight ) < 0 )
	{
		return;
	}

//	ScreenWidth /= 4;
//	ScreenHeight /= 4;

	mDstRct.x = 256;
	mDstRct.y = 256;
	mDstRct.width = ScreenWidth / 4;
	mDstRct.height = ScreenHeight / 4;

	mSrcRct.x = 0;
	mSrcRct.y = 0;
	mSrcRct.width = ScreenWidth << 16;
	mSrcRct.height = ScreenHeight << 16;

	mDispManDisplay = vc_dispmanx_display_open( 0 );
	mDispManUpdate  = vc_dispmanx_update_start( 0 );

	if( !mDispManDisplay || !mDispManUpdate )
	{
		qWarning() << "vc_dispmanx_display_open or vc_dispmanx_update_start failed";

		return;
	}

	mDispManElement = vc_dispmanx_element_add( mDispManUpdate, mDispManDisplay,
											  0, // layer
											  &mDstRct,
											  0, // src
											  &mSrcRct,
											  DISPMANX_PROTECTION_NONE, 0, 0, DISPMANX_NO_ROTATE );

	if( !mDispManElement )
	{
		qWarning() << "vc_dispmanx_element_add failed";

		return;
	}

	mNativeWindow.element = mDispManElement;
	mNativeWindow.width   = ScreenWidth;
	mNativeWindow.height  = ScreenHeight;

	vc_dispmanx_update_submit_sync( mDispManUpdate );

	mDisplay = eglGetDisplay( EGL_DEFAULT_DISPLAY );

	if( mDisplay == EGL_NO_DISPLAY )
	{
		return;
	}

	EGLint		VerMaj, VerMin;

	if( eglInitialize( mDisplay, &VerMaj, &VerMin ) != EGL_TRUE )
	{
		return;
	}

	qDebug() << "EGL VERSION" << VerMaj << VerMin;

	qDebug() << "EGL_CLIENT_APIS" << QString::fromLatin1( eglQueryString( mDisplay, EGL_CLIENT_APIS ) );
	qDebug() << "EGL_VENDOR" << QString::fromLatin1( eglQueryString( mDisplay, EGL_VENDOR ) );
	qDebug() << "EGL_VERSION" << QString::fromLatin1( eglQueryString( mDisplay, EGL_VERSION ) );
	qDebug() << "EGL_EXTENSIONS" << QString::fromLatin1( eglQueryString( mDisplay, EGL_EXTENSIONS ) );

	static const EGLint AttribList[] =
	{
		EGL_RED_SIZE,		8,
		EGL_GREEN_SIZE,		8,
		EGL_BLUE_SIZE,		8,
		EGL_ALPHA_SIZE,		8,
		EGL_DEPTH_SIZE,		24,
		EGL_SURFACE_TYPE,	EGL_WINDOW_BIT,
		EGL_NONE
	};

	QVector<EGLConfig>		Configs;

	if( eglChooseConfig( mDisplay, AttribList, 0, 0, &mNumConfig ) != EGL_TRUE )
	{
		return;
	}

	if( mNumConfig <= 0 )
	{
		return;
	}

	Configs.resize( mNumConfig );

	if( eglChooseConfig( mDisplay, AttribList, Configs.data(), Configs.size(), &mNumConfig ) != EGL_TRUE )
	{
		return;
	}

	for( int i = 0 ; i < mNumConfig ; i++ )
	{
		EGLBoolean		Success;
		EGLint			Red, Green, Blue, Alpha, Depth, SurfaceType, Conformant;

		Success = eglGetConfigAttrib( mDisplay, Configs[ i ], EGL_RED_SIZE, &Red );
		Success = eglGetConfigAttrib( mDisplay, Configs[ i ], EGL_GREEN_SIZE, &Green );
		Success = eglGetConfigAttrib( mDisplay, Configs[ i ], EGL_BLUE_SIZE, &Blue );
		Success = eglGetConfigAttrib( mDisplay, Configs[ i ], EGL_ALPHA_SIZE, &Alpha );
		Success = eglGetConfigAttrib( mDisplay, Configs[ i ], EGL_DEPTH_SIZE, &Depth );
		Success = eglGetConfigAttrib( mDisplay, Configs[ i ], EGL_SURFACE_TYPE, &SurfaceType );
		Success = eglGetConfigAttrib( mDisplay, Configs[ i ], EGL_CONFORMANT, &Conformant );

		qDebug() << i << Red << Green << Blue << Alpha << Depth;

		if( ( Conformant & EGL_OPENGL_BIT ) != 0 ) qDebug() << "EGL_OPENGL_BIT";
		if( ( Conformant & EGL_OPENGL_ES_BIT ) != 0 ) qDebug() << "EGL_OPENGL_ES_BIT";
		if( ( Conformant & EGL_OPENGL_ES2_BIT ) != 0 ) qDebug() << "EGL_OPENGL_ES2_BIT";
		if( ( Conformant & EGL_OPENVG_BIT ) != 0 ) qDebug() << "EGL_OPENVG_BIT";

		if( ( SurfaceType & EGL_MULTISAMPLE_RESOLVE_BOX_BIT ) != 0 ) qDebug() << "EGL_MULTISAMPLE_RESOLVE_BOX_BIT";
		if( ( SurfaceType & EGL_PBUFFER_BIT ) != 0 ) qDebug() << "EGL_PBUFFER_BIT";
		if( ( SurfaceType & EGL_PIXMAP_BIT ) != 0 ) qDebug() << "EGL_PIXMAP_BIT";
		if( ( SurfaceType & EGL_SWAP_BEHAVIOR_PRESERVED_BIT ) != 0 ) qDebug() << "EGL_SWAP_BEHAVIOR_PRESERVED_BIT";
		if( ( SurfaceType & EGL_VG_ALPHA_FORMAT_PRE_BIT ) != 0 ) qDebug() << "EGL_VG_ALPHA_FORMAT_PRE_BIT";
		if( ( SurfaceType & EGL_VG_COLORSPACE_LINEAR_BIT ) != 0 ) qDebug() << "EGL_VG_COLORSPACE_LINEAR_BIT";
		if( ( SurfaceType & EGL_WINDOW_BIT ) != 0 ) qDebug() << "EGL_WINDOW_BIT";
	}

	mConfig = Configs[ 0 ];

	if( eglBindAPI( EGL_OPENGL_ES_API ) != EGL_TRUE )
	{
		qWarning() << "eglBindAPI";

		return;
	}

	if( ( mSurface = eglCreateWindowSurface( mDisplay, mConfig, &mNativeWindow, 0 ) ) == EGL_NO_SURFACE )
	{
		EGLint Error;

		while( ( Error = eglGetError() ) != EGL_SUCCESS )
		{
			switch( Error )
			{
				case EGL_BAD_DISPLAY:	qWarning() << "EGL_BAD_DISPLAY"; break;
				case EGL_NOT_INITIALIZED:	qWarning() << "EGL_NOT_INITIALIZED"; break;
				case EGL_BAD_CONFIG:	qWarning() << "EGL_BAD_CONFIG"; break;
				case EGL_BAD_NATIVE_WINDOW:	qWarning() << "EGL_BAD_NATIVE_WINDOW"; break;
				case EGL_BAD_ALLOC:	qWarning() << "EGL_BAD_ALLOC"; break;
				case EGL_BAD_MATCH:	qWarning() << "EGL_BAD_MATCH"; break;
				default:	qWarning() << QString::number( Error ); break;
			}
		}

		return;
	}

	static const EGLint ContextAttrib[] =
	{
		EGL_CONTEXT_CLIENT_VERSION,	2,
		EGL_NONE
	};

	if( ( mContext = eglCreateContext( mDisplay, mConfig, EGL_NO_CONTEXT, ContextAttrib ) ) == EGL_NO_CONTEXT )
	{
		qWarning() << "eglCreateContext";

		return;
	}

	if( eglMakeCurrent( mDisplay, mSurface, mSurface, mContext ) == EGL_FALSE )
	{
		qWarning() << "eglMakeCurrent";

		return;
	}

	Q_ASSERT( eglGetCurrentContext() != EGL_NO_CONTEXT );
	Q_ASSERT( eglGetCurrentDisplay() != EGL_NO_DISPLAY );
	Q_ASSERT( eglGetCurrentSurface( EGL_DRAW ) != EGL_NO_SURFACE );
	Q_ASSERT( eglGetCurrentSurface( EGL_READ ) != EGL_NO_SURFACE );

	glViewport( 0, 0, ScreenWidth, ScreenHeight );

	glClearColor( 0.15f, 0.25f, 0.35f, 1.0f );

	glClear( GL_COLOR_BUFFER_BIT );

	eglSwapBuffers( mDisplay, mSurface );

	qDebug() << "EGL Initialised";
}

DeviceOpenGLOutput::~DeviceOpenGLOutput( void )
{
	if( mDisplay != EGL_NO_DISPLAY )
	{
		if( mSurface != EGL_NO_SURFACE )
		{
			eglSwapBuffers( mDisplay, mSurface );

			eglMakeCurrent( mDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT );

			eglDestroySurface( mDisplay, mSurface );
		}

		if( mContext != EGL_NO_CONTEXT )
		{
			eglDestroyContext( mDisplay, mContext );
		}

		eglTerminate( mDisplay );
	}

	mDisplay = EGL_NO_DISPLAY;
	mSurface = EGL_NO_SURFACE;
	mContext = EGL_NO_CONTEXT;
}

void DeviceOpenGLOutput::setCurrentNode( QSharedPointer<fugio::NodeInterface> pNode )
{
	mNode = pNode;
}

void DeviceOpenGLOutput::unsetCurrentNode( QSharedPointer<fugio::NodeInterface> pNode )
{
	QSharedPointer<fugio::NodeInterface>	CurNod = mNode.toStrongRef();

	if( CurNod && CurNod != pNode )
	{
		return;
	}

	mNode.clear();

	mInputEvents = nullptr;
}

void DeviceOpenGLOutput::renderLater( void )
{
	mUpdatePending = true;
}

bool DeviceOpenGLOutput::renderInit( void )
{
	return( true );
}

void DeviceOpenGLOutput::renderStart( void )
{
	glViewport( 0, 0, mDstRct.width, mDstRct.height );

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

void DeviceOpenGLOutput::renderEnd( void )
{
	if( mDisplay && mSurface )
	{
		glFlush();

		eglSwapBuffers( mDisplay, mSurface );
	}
}

QSize DeviceOpenGLOutput::windowSize( void ) const
{
	return( QSize( mDstRct.width, mDstRct.height ) );
}

QSize DeviceOpenGLOutput::framebufferSize( void ) const
{
	return( QSize( mDstRct.width, mDstRct.height ) );
}

