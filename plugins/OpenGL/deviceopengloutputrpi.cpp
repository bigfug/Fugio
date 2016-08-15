#include "deviceopengloutputrpi.h"

#include "openglplugin.h"

QSize DeviceOpenGLOutputRPi::size() const
{
	return( QSize( DstRct.width, DstRct.height ) );
}

bool DeviceOpenGLOutputRPi::hasDebugContext() const
{
	return( false );
}

void DeviceOpenGLOutputRPi::renderStart()
{
	glViewport( 0, 0, DstRct.width, DstRct.height );

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

void DeviceOpenGLOutputRPi::renderEnd()
{
	if( mDisplay && mSurface )
	{
		glFlush();

		eglSwapBuffers( mDisplay, mSurface );
	}
}

void DeviceOpenGLOutputRPi::processEvents()
{

}

void DeviceOpenGLOutputRPi::setGeometry(QRect pRect)
{
	Q_UNUSED( pRect )
}

DeviceOpenGLOutputRPi::DeviceOpenGLOutputRPi( bool pContextOnly )
	: DeviceOpenGLOutput( pContextOnly ), mDisplay( 0 ), mConfig( 0 ), mNumConfig( 0 ), mContext( 0 ), mSurface( 0 )
{
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

	static const EGLint AttribList[] =
	{
		EGL_RED_SIZE,		8,
		EGL_GREEN_SIZE,		8,
		EGL_BLUE_SIZE,		8,
		EGL_ALPHA_SIZE,		8,
		EGL_DEPTH_SIZE,		16,
		EGL_SURFACE_TYPE,	EGL_WINDOW_BIT,
		EGL_NONE
	};

	if( eglSaneChooseConfigBRCM( mDisplay, AttribList, &mConfig, 1, &mNumConfig ) != EGL_TRUE )
	{
		return;
	}

	if( eglBindAPI( EGL_OPENGL_ES_API ) != EGL_TRUE )
	{
		return;
	}

	static const EGLint ContextAttrib[] =
	{
		EGL_CONTEXT_CLIENT_VERSION,	2,
		EGL_NONE
	};

	if( ( mContext = eglCreateContext( mDisplay, mConfig, EGL_NO_CONTEXT, ContextAttrib ) ) == EGL_NO_CONTEXT )
	{
		return;
	}

	quint32		ScreenWidth, ScreenHeight;

	if( graphics_get_display_size( 0, &ScreenWidth, &ScreenHeight ) < 0 )
	{
		return;
	}

	ScreenWidth /= 4;
	ScreenHeight /= 4;

	DstRct.x = 200;
	DstRct.y = 200;
	DstRct.width = ScreenWidth;
	DstRct.height = ScreenHeight;

	SrcRct.x = 200;
	SrcRct.y = 200;
	SrcRct.width = ScreenWidth << 16;
	SrcRct.height = ScreenHeight << 16;

	DispManDisplay = vc_dispmanx_display_open( 0 );
	DispManUpdate  = vc_dispmanx_update_start( 0 );

	DispManElement = vc_dispmanx_element_add( DispManUpdate, DispManDisplay,
											  0, // layer
											  &DstRct,
											  0, // src
											  &SrcRct,
											  DISPMANX_PROTECTION_NONE, 0, 0, DISPMANX_NO_ROTATE );

	NativeWindow.element = DispManElement;
	NativeWindow.width   = ScreenWidth;
	NativeWindow.height  = ScreenHeight;

	vc_dispmanx_update_submit_sync( DispManUpdate );

	if( ( mSurface = eglCreateWindowSurface( mDisplay, mConfig, &NativeWindow, 0 ) ) == EGL_NO_SURFACE )
	{
		return;
	}

	if( eglMakeCurrent( mDisplay, mSurface, mSurface, mContext ) == EGL_FALSE )
	{
		return;
	}

	Q_ASSERT( eglGetCurrentContext() != EGL_NO_CONTEXT );
	Q_ASSERT( eglGetCurrentDisplay() != EGL_NO_DISPLAY );
	Q_ASSERT( eglGetCurrentSurface( EGL_DRAW ) != EGL_NO_SURFACE );
	Q_ASSERT( eglGetCurrentSurface( EGL_READ ) != EGL_NO_SURFACE );

	glViewport( 0, 0, ScreenWidth, ScreenHeight );

	glClearColor( 0.15f, 0.25f, 0.35f, 1.0f );

	glClear( GL_COLOR_BUFFER_BIT );
}

DeviceOpenGLOutputRPi::~DeviceOpenGLOutputRPi()
{
	closeOutput();
}

void DeviceOpenGLOutputRPi::closeOutput()
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
