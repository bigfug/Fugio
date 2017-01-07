#include "openglplugin.h"

#include <fugio/opengl/uuid.h>

#include <fugio/global_interface.h>
#include <fugio/global_signals.h>

#include <QCommandLineParser>
#include <QApplication>
#include <QThread>

#include "texturenode.h"
#include "texturecopynode.h"
#include "statenode.h"
#include "previewnode.h"
#include "drawnode.h"
#include "shaderinstancenode.h"
#include "shadercompilernode.h"
#include "viewportnode.h"
#include "bindtexturenode.h"
#include "windownode.h"
#include "texturecubenode.h"
#include "clearnode.h"
#include "rendernode.h"
#include "imagetotexturenode.h"
#include "texturetoimagenode.h"
#include "transformfeedbacknode.h"
#include "buffertoarraynode.h"
#include "bufferentrynode.h"
#include "buffernode.h"
#include "arraytobuffernode.h"
#include "vertexarrayobjectnode.h"
#include "contextnode.h"
#include "arraytoindexnode.h"
#include "instancebuffernode.h"
#include "viewportmatrixnode.h"
#include "cubemaprendernode.h"

#include "texturepin.h"
#include "renderpin.h"
#include "statepin.h"
#include "bufferpin.h"
#include "shaderpin.h"
#include "bufferentrypin.h"
#include "bufferentryproxypin.h"
#include "vertexarrayobjectpin.h"

#define INSERT_TARGET(x)		mMapTargets.insert(#x,x)
#define INSERT_FORMAT(x)		mMapFormat.insert(#x,x)
#define INSERT_INTERNAL(x)		mMapInternal.insert(#x,x)
#define INSERT_TYPE(x)			mMapType.insert(#x,x)
#define INSERT_FILTER_MIN(x)	mMapFilterMin.insert(#x,x)
#define INSERT_FILTER_MAG(x)	mMapFilterMag.insert(#x,x)
#define INSERT_WRAP(x)			mMapWrap.insert(#x,x)
#define INSERT_COMPARE(x)		mMapCompare.insert(#x,x)

QMap<QString,int>				 OpenGLPlugin::mMapTargets;
QMap<QString,int>				 OpenGLPlugin::mMapFormat;
QMap<QString,int>				 OpenGLPlugin::mMapInternal;
QMap<QString,int>				 OpenGLPlugin::mMapType;
QMap<QString,int>				 OpenGLPlugin::mMapFilterMin;
QMap<QString,int>				 OpenGLPlugin::mMapFilterMag;
QMap<QString,int>				 OpenGLPlugin::mMapWrap;
QMap<QString,int>				 OpenGLPlugin::mMapCompare;

QList<QUuid>					 NodeControlBase::PID_UUID;

ClassEntry		OpenGLPlugin::mNodeClasses[] =
{
	ClassEntry( "Array To Buffer", "OpenGL", NID_OPENGL_ARRAY_TO_BUFFER, &ArrayToBufferNode::staticMetaObject ),
	ClassEntry( "Array To Index", "OpenGL", NID_OPENGL_ARRAY_TO_INDEX, &ArrayToIndexNode::staticMetaObject ),
	ClassEntry( "Bind Texture", "OpenGL", NID_OPENGL_BIND_TEXTURE, &BindTextureNode::staticMetaObject ),
	ClassEntry( "Buffer", "OpenGL", NID_OPENGL_BUFFER, &BufferNode::staticMetaObject ),
	ClassEntry( "Buffer Entry", "OpenGL", NID_OPENGL_BUFFER_ENTRY, &BufferEntryNode::staticMetaObject ),
	ClassEntry( "Buffer To Array", "OpenGL", NID_OPENGL_BUFFER_TO_ARRAY, &BufferToArrayNode::staticMetaObject ),
	ClassEntry( "Clear", "OpenGL", NID_OPENGL_CLEAR, &ClearNode::staticMetaObject ),
	ClassEntry( "Context", "OpenGL", NID_OPENGL_CONTEXT, &ContextNode::staticMetaObject ),
	ClassEntry( "CubeMap Render", "OpenGL", NID_OPENGL_CUBEMAP_RENDER, &CubeMapRenderNode::staticMetaObject ),
	ClassEntry( "Draw", "OpenGL", NID_OPENGL_DRAW, &DrawNode::staticMetaObject ),
	ClassEntry( "Image to Texture", "OpenGL", NID_OPENGL_IMAGE_TO_TEXTURE, &ImageToTextureNode::staticMetaObject ),
	ClassEntry( "Instance Buffer", "OpenGL", NID_OPENGL_INSTANCE_BUFFER, &InstanceBufferNode::staticMetaObject ),
//	ClassEntry( "Preview", "OpenGL", NID_OPENGL_PREVIEW, &PreviewNode::staticMetaObject ),
	ClassEntry( "Window", "OpenGL", NID_OPENGL_WINDOW, &WindowNode::staticMetaObject ),
	ClassEntry( "Shader Compiler", "OpenGL", NID_OPENGL_SHADER_COMPILER, &ShaderCompilerNode::staticMetaObject ),
	ClassEntry( "State", "OpenGL", NID_OPENGL_STATE, &StateNode::staticMetaObject ),
	ClassEntry( "Render", "OpenGL", NID_OPENGL_RENDER, &RenderNode::staticMetaObject ),
	ClassEntry( "Shader Instance", "OpenGL", NID_OPENGL_SHADER_INSTANCE, &ShaderInstanceNode::staticMetaObject ),
	ClassEntry( "Texture", "OpenGL", NID_OPENGL_TEXTURE, &TextureNode::staticMetaObject ),
	ClassEntry( "Texture To Image", "OpenGL", NID_OPENGL_TEXTURE_TO_IMAGE, &TextureToImageNode::staticMetaObject ),
	ClassEntry( "Texture Copy", "OpenGL", NID_OPENGL_TEXTURE_COPY, &TextureCopyNode::staticMetaObject ),
	ClassEntry( "Texture Cube", "OpenGL", NID_OPENGL_TEXTURE_CUBE, &TextureCubeNode::staticMetaObject ),
	ClassEntry( "Transform Feedback", "OpenGL", NID_OPENGL_TRANSFORM_FEEDBACK, &TransformFeedbackNode::staticMetaObject ),
	ClassEntry( "Vertex Array Object", "OpenGL", NID_OPENGL_VERTEX_ARRAY_OBJECT, &VertexArrayObjectNode::staticMetaObject ),
	ClassEntry( "Viewport", "OpenGL", NID_OPENGL_VIEWPORT, &ViewportNode::staticMetaObject ),
	ClassEntry( "Viewport Matrix", "OpenGL", NID_OPENGL_VIEWPORT_MATRIX, &ViewportMatrixNode::staticMetaObject ),
	ClassEntry()
};

ClassEntry		OpenGLPlugin::mPinClasses[] =
{
	ClassEntry( "Render", PID_RENDER, &RenderPin::staticMetaObject ),
	ClassEntry( "Buffer", "OpenGL", PID_OPENGL_BUFFER, &BufferPin::staticMetaObject ),
	ClassEntry( "Buffer Entry", "OpenGL", PID_OPENGL_BUFFER_ENTRY, &BufferEntryPin::staticMetaObject ),
	ClassEntry( "Buffer Entry Proxy", "OpenGL", PID_OPENGL_BUFFER_ENTRY_PROXY, &BufferEntryProxyPin::staticMetaObject ),
	ClassEntry( "State", "OpenGL", PID_OPENGL_STATE, &StatePin::staticMetaObject ),
	ClassEntry( "Shader", "OpenGL", PID_OPENGL_SHADER, &ShaderPin::staticMetaObject ),
	ClassEntry( "Vertex Array Object", "OpenGL", PID_OPENGL_VERTEX_ARRAY_OBJECT, &VertexArrayObjectPin::staticMetaObject ),
	ClassEntry( "Texture", "OpenGL", PID_OPENGL_TEXTURE, &TexturePin::staticMetaObject ),
	ClassEntry()
};

OpenGLPlugin		*OpenGLPlugin::mInstance = 0;

#include <QCoreApplication>

OpenGLPlugin::OpenGLPlugin( void )
	: mApp( 0 ), mTriangleCount( 0 ),
	   mOpenGLFullScreenOption( "opengl-full-screen", "Open all OpenGL windows as full screen" )
{
	mInstance = this;

	initStaticData();
}

OpenGLPlugin::~OpenGLPlugin( void )
{
	mInstance = nullptr;
}

PluginInterface::InitResult OpenGLPlugin::initialise( fugio::GlobalInterface *pApp, bool pLastChance )
{
	Q_UNUSED( pLastChance )

	mApp = pApp;

	mApp->commandLineParser().addOption( mOpenGLFullScreenOption );

	mApp->registerInterface( IID_OPENGL, this );

	mApp->registerNodeClasses( mNodeClasses );

	mApp->registerPinClasses( mPinClasses );

	//mApp->registerDeviceFactory( this );

	connect( mApp->qobject(), SIGNAL(configLoad(QSettings&)), this, SLOT(configLoad(QSettings&)) );
	connect( mApp->qobject(), SIGNAL(configSave(QSettings&)), this, SLOT(configSave(QSettings&)) );

	connect( mApp->qobject(), SIGNAL(frameEnd()), this, SLOT(globalFrameEnd()) );

	DeviceOpenGLOutput::deviceInitialise();

	return( INIT_OK );
}

void OpenGLPlugin::deinitialise()
{
	DeviceOpenGLOutput::deviceDeinitialise();

	mApp->qobject()->disconnect( this );

	mApp->unregisterNodeClasses( mNodeClasses );

	mApp->unregisterPinClasses( mPinClasses );

	mApp->unregisterInterface( IID_OPENGL );

	mApp = 0;

	mInstance = 0;
}

#if defined( OPENGL_DEBUG_ENABLE )
void OpenGLPlugin::checkErrors( void )
{
	if( !hasContext() )
	{
		return;
	}

#if defined( Q_OS_RASPBERRY_PI )
	for( GLenum e = eglGetError() ; e != EGL_SUCCESS ; e = eglGetError() )
	{
		qDebug() << "EGL" << QString::number( e, 16 );
	}
#endif

	for( GLenum e = glGetError() ; e != GL_NO_ERROR ; e = glGetError() )
	{
#if defined( GLU_VERSION )
		qDebug() << "GL" << QString::fromLatin1( (const char *)gluErrorString( e ) );
#else
		qDebug() << "GL" << QString::number( e, 16 );
#endif
	}
}

void OpenGLPlugin::checkErrors( const char *file, int line )
{
#if !defined( GLU_VERSION )
	Q_UNUSED( file )
	Q_UNUSED( line )
#endif

	if( !hasContext() )
	{
		return;
	}

#if defined( Q_OS_RASPBERRY_PI )
	for( GLenum e = eglGetError() ; e != EGL_SUCCESS ; e = eglGetError() )
	{
		qDebug() << "EGL" << file << line << ":" << QString::number( e, 16 );
	}
#endif

	for( GLenum e = glGetError() ; e != GL_NO_ERROR ; e = glGetError() )
	{
#if defined( GLU_VERSION )
		qDebug() << "GL" << file << line << ":" << QString::fromLatin1( (const char *)gluErrorString( e ) );
#else
		qDebug() << "GL" << file << line << ":" << QString::number( e, 16 );
#endif
	}
}

void OpenGLPlugin::checkErrors(const QString &pContext, const char *file, int line)
{
#if !defined( GLU_VERSION )
	Q_UNUSED( file )
	Q_UNUSED( line )
#endif

	if( !hasContext() )
	{
		return;
	}

#if defined( Q_OS_RASPBERRY_PI )
	for( GLenum e = eglGetError() ; e != EGL_SUCCESS ; e = eglGetError() )
	{
		qDebug() << "EGL" << file << line << ":" << QString::number( e, 16 );
	}
#endif

	for( GLenum e = glGetError() ; e != GL_NO_ERROR ; e = glGetError() )
	{
#if defined( GLU_VERSION )
		qDebug() << "GL" << pContext << file << line << ":" << QString::fromLatin1( (const char *)gluErrorString( e ) );
#else
		qDebug() << "GL" << pContext << file << line << ":" << QString::number( e, 16 );
#endif
	}
}
#endif

void OpenGLPlugin::registerOutputWindowHook(QObject *pObject, const char *pMember)
{
	connect( this, SIGNAL(windowHook(QWindow*)), pObject, pMember );
}

bool OpenGLPlugin::hasContext()
{
	return( hasContextStatic() );
}

QString OpenGLPlugin::framebufferError( GLenum pErrorCode )
{
	switch( pErrorCode )
	{
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:	return( "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT" );
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:	return( "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT" );
		case GL_FRAMEBUFFER_UNSUPPORTED:	return( "GL_FRAMEBUFFER_UNSUPPORTED" );
		case GL_FRAMEBUFFER_COMPLETE:	return( "GL_FRAMEBUFFER_COMPLETE" );

#if !defined( GL_ES_VERSION_2_0 )
		case GL_FRAMEBUFFER_UNDEFINED:	return( "GL_FRAMEBUFFER_UNDEFINED" );
		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:	return( "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER" );
		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:	return( "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER" );
		case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:	return( "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE" );
		case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:	return( "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS" );
#endif
	}

	return( QString::number( pErrorCode, 16 ) );
}

void OpenGLPlugin::deviceConfigGui( QWidget *pParent )
{
	Q_UNUSED( pParent )
}

bool OpenGLPlugin::hasContextStatic()
{
#if defined( GL_ES_VERSION_2_0 )
	return( eglGetCurrentContext() != 0 );
#else
	return( QOpenGLContext::currentContext() && glewExperimental == GL_TRUE );
#endif
}

bool OpenGLPlugin::openWindowFullScreen() const
{
	return( mApp->commandLineParser().isSet( mOpenGLFullScreenOption ) );
}

void OpenGLPlugin::onWindowCreate( QWindow *pWindow )
{
	emit windowHook( pWindow );
}

void OpenGLPlugin::configLoad( QSettings &pSettings )
{
	pSettings.beginGroup( "opengl" );

	pSettings.endGroup();
}

void OpenGLPlugin::configSave( QSettings &pSettings ) const
{
	pSettings.beginGroup( "opengl" );

	pSettings.endGroup();
}

void OpenGLPlugin::globalFrameEnd()
{
	DeviceOpenGLOutput::devicePacketEnd();
}

void OpenGLPlugin::appAboutToQuit()
{
	DeviceOpenGLOutput::deviceDeinitialise();
}

void OpenGLPlugin::initStaticData( void )
{
	if( mMapTargets.isEmpty() )
	{
		INSERT_TARGET( GL_TEXTURE_2D );
#if !defined( GL_ES_VERSION_2_0 )
		INSERT_TARGET( GL_TEXTURE_1D );
		INSERT_TARGET( GL_TEXTURE_3D );
		INSERT_TARGET( GL_TEXTURE_RECTANGLE );
		INSERT_TARGET( GL_TEXTURE_CUBE_MAP );
#endif
	}

	if( mMapFormat.isEmpty() )
	{
		INSERT_FORMAT( GL_RGB );
		INSERT_FORMAT( GL_RGBA );
		INSERT_FORMAT( GL_DEPTH_COMPONENT );

		INSERT_FORMAT( GL_LUMINANCE );

#if !defined( GL_ES_VERSION_2_0 )
		INSERT_FORMAT( GL_RED );
		INSERT_FORMAT( GL_RG );
		INSERT_FORMAT( GL_BGR );
		INSERT_FORMAT( GL_BGRA );
		INSERT_FORMAT( GL_RED_INTEGER );
		INSERT_FORMAT( GL_RG_INTEGER );
		INSERT_FORMAT( GL_RGB_INTEGER );
		INSERT_FORMAT( GL_BGR_INTEGER );
		INSERT_FORMAT( GL_RGBA_INTEGER );
		INSERT_FORMAT( GL_BGRA_INTEGER );
		INSERT_FORMAT( GL_STENCIL_INDEX );
		INSERT_FORMAT( GL_DEPTH_STENCIL );
#endif
	}

	if( mMapInternal.isEmpty() )
	{
		INSERT_INTERNAL( GL_LUMINANCE );

		INSERT_INTERNAL( GL_DEPTH_COMPONENT );
		INSERT_INTERNAL( GL_DEPTH_COMPONENT16 );
		INSERT_INTERNAL( GL_DEPTH_COMPONENT24 );
		INSERT_INTERNAL( GL_DEPTH_COMPONENT32 );
		INSERT_INTERNAL( GL_DEPTH_COMPONENT32F );
		INSERT_INTERNAL( GL_RGB );
		INSERT_INTERNAL( GL_RGBA );
		INSERT_INTERNAL( GL_RGBA4 );
		INSERT_INTERNAL( GL_RGB5_A1 );

#if !defined( GL_ES_VERSION_2_0 )
		INSERT_INTERNAL( GL_LUMINANCE16 );
		INSERT_INTERNAL( GL_DEPTH_STENCIL );
		INSERT_INTERNAL( GL_RED );
		INSERT_INTERNAL( GL_RG );
		INSERT_INTERNAL( GL_R8 );
		INSERT_INTERNAL( GL_R8_SNORM );
		INSERT_INTERNAL( GL_R16 );
		INSERT_INTERNAL( GL_R16_SNORM );
		INSERT_INTERNAL( GL_RG8 );
		INSERT_INTERNAL( GL_RG8_SNORM );
		INSERT_INTERNAL( GL_RG16 );
		INSERT_INTERNAL( GL_RG16_SNORM );
		INSERT_INTERNAL( GL_R3_G3_B2 );
		INSERT_INTERNAL( GL_RGB4 );
		INSERT_INTERNAL( GL_RGB5 );
		INSERT_INTERNAL( GL_RGB8 );
		INSERT_INTERNAL( GL_RGB8_SNORM );
		INSERT_INTERNAL( GL_RGB10 );
		INSERT_INTERNAL( GL_RGB12 );
		INSERT_INTERNAL( GL_RGB16_SNORM );
		INSERT_INTERNAL( GL_RGBA2 );
		INSERT_INTERNAL( GL_RGBA8 );
		INSERT_INTERNAL( GL_RGBA8_SNORM );
		INSERT_INTERNAL( GL_RGB10_A2 );
#if defined( GL_RGB10_A2UI )
		INSERT_INTERNAL( GL_RGB10_A2UI );
#endif
		INSERT_INTERNAL( GL_RGBA12 );
		INSERT_INTERNAL( GL_RGBA16 );
		INSERT_INTERNAL( GL_SRGB8 );
		INSERT_INTERNAL( GL_SRGB8_ALPHA8 );
		INSERT_INTERNAL( GL_R16F );
		INSERT_INTERNAL( GL_RG16F );
		INSERT_INTERNAL( GL_RGB16F );
		INSERT_INTERNAL( GL_RGBA16F );
		INSERT_INTERNAL( GL_R32F );
		INSERT_INTERNAL( GL_RG32F );
		INSERT_INTERNAL( GL_RGB32F );
		INSERT_INTERNAL( GL_RGBA32F );
		INSERT_INTERNAL( GL_R11F_G11F_B10F );
		INSERT_INTERNAL( GL_RGB9_E5 );
		INSERT_INTERNAL( GL_R8I );
		INSERT_INTERNAL( GL_R8UI );
		INSERT_INTERNAL( GL_R16I );
		INSERT_INTERNAL( GL_R16UI );
		INSERT_INTERNAL( GL_R32I );
		INSERT_INTERNAL( GL_R32UI );
		INSERT_INTERNAL( GL_RG8I );
		INSERT_INTERNAL( GL_RG8UI );
		INSERT_INTERNAL( GL_RG16I );
		INSERT_INTERNAL( GL_RG16UI );
		INSERT_INTERNAL( GL_RG32I );
		INSERT_INTERNAL( GL_RG32UI );
		INSERT_INTERNAL( GL_RGB8I );
		INSERT_INTERNAL( GL_RGB8UI );
		INSERT_INTERNAL( GL_RGB16I );
		INSERT_INTERNAL( GL_RGB16UI );
		INSERT_INTERNAL( GL_RGB32I );
		INSERT_INTERNAL( GL_RGB32UI );
		INSERT_INTERNAL( GL_RGBA8I );
		INSERT_INTERNAL( GL_RGBA8UI );
		INSERT_INTERNAL( GL_RGBA16I );
		INSERT_INTERNAL( GL_RGBA16UI );
		INSERT_INTERNAL( GL_RGBA32I );
		INSERT_INTERNAL( GL_RGBA32UI );
#endif
	}

	if( mMapType.isEmpty() )
	{
		INSERT_TYPE( GL_UNSIGNED_BYTE );
		INSERT_TYPE( GL_BYTE );
		INSERT_TYPE( GL_UNSIGNED_SHORT );
		INSERT_TYPE( GL_SHORT );
		INSERT_TYPE( GL_UNSIGNED_INT );
		INSERT_TYPE( GL_INT );
		INSERT_TYPE( GL_FLOAT );
		INSERT_TYPE( GL_UNSIGNED_SHORT_5_6_5 );
		INSERT_TYPE( GL_UNSIGNED_SHORT_4_4_4_4 );
		INSERT_TYPE( GL_UNSIGNED_SHORT_5_5_5_1 );
#if !defined( GL_ES_VERSION_2_0 )
		INSERT_TYPE( GL_UNSIGNED_BYTE_3_3_2 );
		INSERT_TYPE( GL_UNSIGNED_BYTE_2_3_3_REV );
		INSERT_TYPE( GL_UNSIGNED_SHORT_5_6_5_REV );
		INSERT_TYPE( GL_UNSIGNED_SHORT_4_4_4_4_REV );
		INSERT_TYPE( GL_UNSIGNED_SHORT_1_5_5_5_REV );
		INSERT_TYPE( GL_UNSIGNED_INT_8_8_8_8 );
		INSERT_TYPE( GL_UNSIGNED_INT_8_8_8_8_REV );
		INSERT_TYPE( GL_UNSIGNED_INT_10_10_10_2 );
		INSERT_TYPE( GL_UNSIGNED_INT_2_10_10_10_REV );
#endif
	}

	if( mMapFilterMin.isEmpty() )
	{
		INSERT_FILTER_MIN( GL_NEAREST );
		INSERT_FILTER_MIN( GL_LINEAR );
		INSERT_FILTER_MIN( GL_NEAREST_MIPMAP_NEAREST );
		INSERT_FILTER_MIN( GL_LINEAR_MIPMAP_NEAREST );
		INSERT_FILTER_MIN( GL_NEAREST_MIPMAP_LINEAR );
		INSERT_FILTER_MIN( GL_LINEAR_MIPMAP_LINEAR );
	}

	if( mMapFilterMag.isEmpty() )
	{
		INSERT_FILTER_MAG( GL_NEAREST );
		INSERT_FILTER_MAG( GL_LINEAR );
	}

	if( mMapWrap.isEmpty() )
	{
		INSERT_WRAP( GL_REPEAT );
		INSERT_WRAP( GL_CLAMP_TO_EDGE );
		INSERT_WRAP( GL_MIRRORED_REPEAT );
#if !defined( GL_ES_VERSION_2_0 )
		INSERT_WRAP( GL_CLAMP_TO_BORDER );
		INSERT_WRAP( GL_CLAMP );
		INSERT_WRAP( GL_MIRROR_CLAMP_TO_EDGE );
#endif
	}

	if( mMapCompare.isEmpty() )
	{
		INSERT_COMPARE( GL_NONE );
		INSERT_COMPARE( GL_LEQUAL );
		INSERT_COMPARE( GL_GEQUAL );
		INSERT_COMPARE( GL_LESS );
		INSERT_COMPARE( GL_GREATER );
		INSERT_COMPARE( GL_EQUAL );
		INSERT_COMPARE( GL_NOTEQUAL );
		INSERT_COMPARE( GL_ALWAYS );
		INSERT_COMPARE( GL_NEVER );
	}
}
