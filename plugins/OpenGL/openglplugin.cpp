#include "openglplugin.h"

#include <QTranslator>
#include <QApplication>

#include <fugio/opengl/uuid.h>

#include <fugio/global_interface.h>
#include <fugio/global_signals.h>

#include <fugio/text/syntax_highlighter_interface.h>

#include <QCommandLineParser>
#include <QApplication>
#include <QThread>

#include "texturenode.h"
#include "texturecopynode.h"
#include "textureclonenode.h"
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
#include "texturemonitornode.h"
#include "rendertotexturenode.h"

#include "texturepin.h"
#include "renderpin.h"
#include "statepin.h"
#include "bufferpin.h"
#include "shaderpin.h"
#include "bufferentrypin.h"
#include "bufferentryproxypin.h"
#include "vertexarrayobjectpin.h"

#include "syntaxhighlighterglsl.h"

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
	ClassEntry( "Render To Texture", "OpenGL", NID_OPENGL_RENDER_TO_TEXTURE, &RenderToTextureNode::staticMetaObject ),
	ClassEntry( "Shader Instance", "OpenGL", NID_OPENGL_SHADER_INSTANCE, &ShaderInstanceNode::staticMetaObject ),
	ClassEntry( "Texture", "OpenGL", NID_OPENGL_TEXTURE, &TextureNode::staticMetaObject ),
	ClassEntry( "Texture To Image", "OpenGL", NID_OPENGL_TEXTURE_TO_IMAGE, &TextureToImageNode::staticMetaObject ),
	ClassEntry( "Texture Clone", "OpenGL", NID_OPENGL_TEXTURE_CLONE, &TextureCloneNode::staticMetaObject ),
	ClassEntry( "Texture Copy", "OpenGL", NID_OPENGL_TEXTURE_COPY, &TextureCopyNode::staticMetaObject ),
	ClassEntry( "Texture Cube", "OpenGL", NID_OPENGL_TEXTURE_CUBE, &TextureCubeNode::staticMetaObject ),
	ClassEntry( "Texture Monitor", "OpenGL", NID_OPENGL_TEXTURE_MONITOR, &TextureMonitorNode::staticMetaObject ),
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
	: mApp( 0 ),
	   mOpenGLFullScreenOption( "opengl-full-screen", "Open all OpenGL windows as full screen" ), mTriangleCount( 0 )
{
	mInstance = this;

//	QSurfaceFormat	SurfaceFormat;

//	SurfaceFormat.setDepthBufferSize( 24 );
//	SurfaceFormat.setProfile( QSurfaceFormat::CoreProfile );
//	SurfaceFormat.setSamples( 4 );
//	SurfaceFormat.setVersion( 4, 5 );

//#if defined( OPENGL_DEBUG_ENABLE )
//	SurfaceFormat.setOption( QSurfaceFormat::DebugContext );
//#endif

////	SurfaceFormat.setProfile( QSurfaceFormat::CoreProfile );
////	SurfaceFormat.setVersion( 4, 5 );
////	SurfaceFormat.setSwapInterval( 0 );

//	QSurfaceFormat::setDefaultFormat( SurfaceFormat );

	//-------------------------------------------------------------------------
	// Install translator

	static QTranslator		Translator;

	if( Translator.load( QLocale(), QLatin1String( "fugio_opengl" ), QLatin1String( "_" ), ":/translations" ) )
	{
		qApp->installTranslator( &Translator );
	}

	initStaticData();
}

OpenGLPlugin::~OpenGLPlugin( void )
{
	mInstance = nullptr;
}

void OpenGLPlugin::parseShaderErrors( QString pErrorText, QList<SyntaxError> &pErrorData )
{
	if( pErrorText.startsWith( '"' ) )
	{
		pErrorText.remove( 0, 1 );
	}

	if( pErrorText.endsWith( '"' ) )
	{
		pErrorText.chop( 1 );
	}

	QStringList			SL = pErrorText.split( '\n' );
	fugio::SyntaxError	SE;

	SE.mColumnStart = SE.mColumnEnd = -1;

	for( QString S : SL )
	{
		S = S.trimmed();

		if( S.isEmpty() )
		{
			continue;
		}

		QRegExp	P1( "^ERROR: (.+):(\\d+):\\s+(.+)" );

		if( P1.indexIn( S ) > -1 )
		{
			SE.mLineStart = SE.mLineEnd = P1.cap( 2 ).toInt();

			SE.mError = P1.cap( 3 );

			pErrorData << SE;

			continue;
		}

		QRegExp	P2( "^ERROR:\\s?(.+)" );

		if( P2.indexIn( S ) > -1 )
		{
			SE.mLineStart = SE.mLineEnd = -1;

			SE.mError = P2.cap( 1 );

			pErrorData << SE;

			continue;
		}

		QRegExp	P3( "^.+\\((\\d+)\\)\\s?:\\s?(.+):\\s?(.+)" );

		if( P3.indexIn( S ) > -1 )
		{
			SE.mLineStart   = SE.mLineEnd = P3.cap( 1 ).toInt();

			SE.mError = P3.cap( 3 );

			pErrorData << SE;

			continue;
		}

		SE.mLineStart = SE.mLineEnd   = -1;

		SE.mError = S;

		pErrorData << SE;

		qDebug() << S;

	}
}

PluginInterface::InitResult OpenGLPlugin::initialise( fugio::GlobalInterface *pApp, bool pLastChance )
{
	fugio::SyntaxHighlighterInterface	*SyntaxHighlighter = qobject_cast<fugio::SyntaxHighlighterInterface *>( pApp->findInterface( IID_SYNTAX_HIGHLIGHTER ) );

	if( !SyntaxHighlighter && !pLastChance )
	{
		return( INIT_DEFER );
	}

	if( SyntaxHighlighter )
	{
		SyntaxHighlighter->registerSyntaxHighlighter( SYNTAX_HIGHLIGHTER_GLSL, QStringLiteral( "GLSL" ), this );
	}

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

	fugio::SyntaxHighlighterInterface	*SyntaxHighlighter = qobject_cast<fugio::SyntaxHighlighterInterface *>( mApp->findInterface( IID_SYNTAX_HIGHLIGHTER ) );

	if( SyntaxHighlighter )
	{
		SyntaxHighlighter->unregisterSyntaxHighlighter( SYNTAX_HIGHLIGHTER_GLSL );
	}

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
		case GL_FRAMEBUFFER_UNDEFINED:	return( "GL_FRAMEBUFFER_UNDEFINED" );
		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:	return( "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER" );
		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:	return( "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER" );
		case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:	return( "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE" );
		case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:	return( "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS" );
	}

	return( QString::number( pErrorCode, 16 ) );
}

void OpenGLPlugin::deviceConfigGui( QWidget *pParent )
{
	Q_UNUSED( pParent )
}

bool OpenGLPlugin::hasContextStatic()
{
	return( QOpenGLContext::currentContext() );
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
#endif
		INSERT_TARGET( GL_TEXTURE_CUBE_MAP );
	}

	if( mMapFormat.isEmpty() )
	{
		INSERT_FORMAT( GL_RGB );
		INSERT_FORMAT( GL_RGBA );
		INSERT_FORMAT( GL_DEPTH_COMPONENT );

		INSERT_FORMAT( GL_LUMINANCE );

		INSERT_FORMAT( GL_STENCIL_INDEX );

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
		INSERT_FORMAT( GL_DEPTH_STENCIL );
#endif
	}

	if( mMapInternal.isEmpty() )
	{
		INSERT_INTERNAL( GL_LUMINANCE );

		INSERT_INTERNAL( GL_DEPTH_COMPONENT );
		INSERT_INTERNAL( GL_DEPTH_COMPONENT16 );
		INSERT_INTERNAL( GL_RGB );
		INSERT_INTERNAL( GL_RGBA );
		INSERT_INTERNAL( GL_RGBA4 );
		INSERT_INTERNAL( GL_RGB5_A1 );
		INSERT_INTERNAL( GL_DEPTH_COMPONENT24 );
		INSERT_INTERNAL( GL_DEPTH_COMPONENT32F );
		INSERT_INTERNAL( GL_DEPTH_STENCIL );
		INSERT_INTERNAL( GL_RED );
		INSERT_INTERNAL( GL_RG );
		INSERT_INTERNAL( GL_R8 );
		INSERT_INTERNAL( GL_R8_SNORM );
		INSERT_INTERNAL( GL_RG8 );
		INSERT_INTERNAL( GL_RG8_SNORM );
		INSERT_INTERNAL( GL_RGB8 );
		INSERT_INTERNAL( GL_RGB8_SNORM );
		INSERT_INTERNAL( GL_RGBA8 );
		INSERT_INTERNAL( GL_RGBA8_SNORM );
		INSERT_INTERNAL( GL_RGB10_A2 );
#if defined( GL_RGB10_A2UI )
		INSERT_INTERNAL( GL_RGB10_A2UI );
#endif

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

#if !defined( GL_ES_VERSION_3_1 )
		INSERT_INTERNAL( GL_DEPTH_COMPONENT32 );
		INSERT_INTERNAL( GL_LUMINANCE16 );
		INSERT_INTERNAL( GL_R16 );
		INSERT_INTERNAL( GL_R16_SNORM );
		INSERT_INTERNAL( GL_RG16 );
		INSERT_INTERNAL( GL_RG16_SNORM );
		INSERT_INTERNAL( GL_R3_G3_B2 );
		INSERT_INTERNAL( GL_RGB4 );
		INSERT_INTERNAL( GL_RGB5 );
		INSERT_INTERNAL( GL_RGB10 );
		INSERT_INTERNAL( GL_RGB12 );
		INSERT_INTERNAL( GL_RGB16_SNORM );
		INSERT_INTERNAL( GL_RGBA2 );
		INSERT_INTERNAL( GL_RGBA12 );
		INSERT_INTERNAL( GL_RGBA16 );
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
#if !defined( GL_ES_VERSION_3_1 )
		INSERT_TYPE( GL_UNSIGNED_BYTE_3_3_2 );
		INSERT_TYPE( GL_UNSIGNED_BYTE_2_3_3_REV );
		INSERT_TYPE( GL_UNSIGNED_SHORT_5_6_5_REV );
		INSERT_TYPE( GL_UNSIGNED_SHORT_4_4_4_4_REV );
		INSERT_TYPE( GL_UNSIGNED_SHORT_1_5_5_5_REV );
		INSERT_TYPE( GL_UNSIGNED_INT_8_8_8_8 );
		INSERT_TYPE( GL_UNSIGNED_INT_8_8_8_8_REV );
		INSERT_TYPE( GL_UNSIGNED_INT_10_10_10_2 );
#endif
		INSERT_TYPE( GL_UNSIGNED_INT_2_10_10_10_REV );
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
		INSERT_WRAP( GL_CLAMP_TO_BORDER );
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

void OpenGLPlugin::initGLEW()
{
	QOpenGLContext		*Context = QOpenGLContext::currentContext();

	if( !Context )
	{
		return;
	}

	static bool Initialised = false;

	if( !Initialised )
	{
		Initialised = GL_TRUE;

		qDebug() << "GL_VENDOR" << QString( (const char *)glGetString( GL_VENDOR ) );

		qDebug() << "GL_RENDERER" << QString( (const char *)glGetString( GL_RENDERER ) );

		qDebug() << "GL_VERSION" << QString( (const char *)glGetString( GL_VERSION ) );

		//qDebug() << context()->extensions();

		switch( Context->format().profile() )
		{
			case QSurfaceFormat::NoProfile:
				qInfo() << "Profile: None";
				break;

			case QSurfaceFormat::CoreProfile:
				qInfo() << "Profile: Core";
				break;

			case QSurfaceFormat::CompatibilityProfile:
				qInfo() << "Profile: Compatibility";
				break;
		}

		qInfo() << "Samples:" << Context->format().samples();
		qInfo() << "Alpha:" << Context->format().alphaBufferSize();
		qInfo() << "Depth:" << Context->format().depthBufferSize();
		qInfo() << "RGB:" << Context->format().redBufferSize() << Context->format().greenBufferSize() << Context->format().blueBufferSize();
	}
}

SyntaxHighlighterInstanceInterface *OpenGLPlugin::syntaxHighlighterInstance( QUuid pUuid ) const
{
	if( pUuid == SYNTAX_HIGHLIGHTER_GLSL )
	{
		return( new SyntaxHighlighterGLSL( OpenGLPlugin::instance() ) );
	}

	return( nullptr );
}
