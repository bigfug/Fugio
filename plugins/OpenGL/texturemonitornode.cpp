#include "texturemonitornode.h"

#include <QMainWindow>

#include "openglplugin.h"

#include <fugio/opengl/uuid.h>

#include <fugio/editor_interface.h>

#include <fugio/opengl/texture_interface.h>

TextureMonitorNode::TextureMonitorNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mDockWidget( nullptr ), mWidget( nullptr ), mDockArea( Qt::BottomDockWidgetArea ),
	  mBuffer( QOpenGLBuffer::VertexBuffer ), mVertexAttribLocation( -1 )
{
	FUGID( PIN_INPUT_TEXTURE, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );

	pinInput( "Texture", PIN_INPUT_TEXTURE );
}

bool TextureMonitorNode::initialise()
{
	if( !NodeControlBase::initialise() )
	{
		return( false );
	}

	if( !mWidget )
	{
		fugio::EditorInterface	*EI = qobject_cast<fugio::EditorInterface *>( OpenGLPlugin::instance()->app()->findInterface( IID_EDITOR ) );

		if( !EI )
		{
			return( false );
		}

		if( ( mDockWidget = new QDockWidget( QString( "Texture Monitor: %1" ).arg( mNode->name() ), EI->mainWindow() ) ) == 0 )
		{
			return( false );
		}

		mDockWidget->setObjectName( mNode->uuid().toString() );

		if( ( mWidget = new TextureMonitor() ) != nullptr )
		{
			mWidget->setNode( this );

			mDockWidget->setWidget( mWidget );
		}

		EI->mainWindow()->addDockWidget( mDockArea, mDockWidget );
	}

	if( !QOpenGLContext::currentContext() )
	{
		return( false );
	}

	return( true );
}

bool TextureMonitorNode::deinitialise()
{
	if( mDockWidget )
	{
		mDockWidget->deleteLater();

		mDockWidget = nullptr;

		mWidget = nullptr;
	}

	return( NodeControlBase::deinitialise() );
}

void TextureMonitorNode::inputsUpdated( qint64 pTimeStamp )
{
	Q_UNUSED( pTimeStamp )

	mWidget->update();
}

QList<QUuid> TextureMonitorNode::pinAddTypesInput() const
{
	static const QList<QUuid> PinLst =
	{
		PID_OPENGL_TEXTURE
	};

	return( PinLst );
}

bool TextureMonitorNode::canAcceptPin(fugio::PinInterface *pPin) const
{
	return( pPin->direction() == PIN_OUTPUT );
}

bool TextureMonitorNode::pinShouldAutoRename(fugio::PinInterface *pPin) const
{
	return( pPin->direction() == PIN_OUTPUT );
}

void TextureMonitorNode::paintGL()
{
	if( mNode->status() != fugio::NodeInterface::Initialised )
	{
		return;
	}

	initializeOpenGLFunctions();

	glClearColor( 0, 0, 0, 0 );

	glClear( GL_COLOR_BUFFER_BIT );

	static const GLfloat Vertices[] =
	{
		-1, -1,
		-1,  1,
		 1, -1,
		 1,  1
	};

	if( !mShader.isLinked() )
	{
		if( !QOpenGLContext::currentContext()->isOpenGLES() && QOpenGLContext::currentContext()->format().majorVersion() >= 3 )
		{
			const char *vertexSource =
					"#version 330\n"
					"in vec2 position;\n"
					"out vec2 tpos;\n"
					"void main()\n"
					"{\n"
					"	gl_Position = vec4( position, 0.0, 1.0 );\n"
					"	tpos = ( position * 0.5 ) + 0.5;\n"
					"}\n";

			const char *fragmentSource =
					"#version 330\n"
					"uniform sampler2D tex;"
					"in vec2 tpos;\n"
					"out vec4 outColor;\n"
					"void main()\n"
					"{\n"
					"	outColor = vec4( texture( tex, tpos ).rgb, 1.0 );\n"
					"}\n";

			mShader.addShaderFromSourceCode( QOpenGLShader::Vertex, vertexSource );

			mShader.addShaderFromSourceCode( QOpenGLShader::Fragment, fragmentSource );

			mShader.link();
		}

		if( !mShader.isLinked() )
		{
			mShader.removeAllShaders();

			mShader.addShaderFromSourceCode( QOpenGLShader::Vertex,
				 "attribute highp vec2 position;\n"
				 "varying highp vec2 tpos;\n"
				 "void main()\n"
				 "{\n"
				 "	gl_Position = vec4( position, 0.0, 1.0 );\n"
				 "	tpos = ( position * 0.5 ) + 0.5;\n"
				 "}\n" );

			mShader.addShaderFromSourceCode( QOpenGLShader::Fragment,
				 "uniform sampler2D tex;"
				 "varying highp vec2 tpos;\n"
				 "void main()\n"
				 "{\n"
				 "	gl_FragColor = vec4( texture2D( tex, tpos ).rgb, 1.0 );\n"
				 "}\n" );

			mShader.link();
		}

		mVertexAttribLocation = mShader.attributeLocation( "position" );
	}

	QOpenGLVertexArrayObject::Binder VAOBinder( &mVAO );

	mBuffer.create();
	mBuffer.bind();

	mBuffer.allocate( Vertices, sizeof( Vertices ) );

	glVertexAttribPointer( mVertexAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, 0 );

	glEnableVertexAttribArray( mVertexAttribLocation );

	mShader.bind();

	QList<fugio::OpenGLTextureInterface *>	TexLst;

	for( QSharedPointer<fugio::PinInterface> P : mNode->enumInputPins() )
	{
		fugio::OpenGLTextureInterface	*T = input<fugio::OpenGLTextureInterface *>( P );

		if( T && T->target() == QOpenGLTexture::Target2D )
		{
			TexLst << T;
		}
	}

	if( TexLst.isEmpty() )
	{
		return;
	}

	glActiveTexture( GL_TEXTURE0 );

	double	tw = 1.0 / double( TexLst.size() );

	for( int i = 0 ; i < TexLst.size() ; i++ )
	{
		fugio::OpenGLTextureInterface *T = TexLst[ i ];

		GLint		VP[ 4 ];

		VP[ 0 ] = tw * i * mWidget->width() * mWidget->devicePixelRatio();
		VP[ 1 ] = 0;
		VP[ 2 ] = tw * mWidget->width() * mWidget->devicePixelRatio();
		VP[ 3 ] = mWidget->height() * mWidget->devicePixelRatio();

		glViewport( VP[ 0 ], VP[ 1 ], VP[ 2 ], VP[ 3 ] );

		T->srcBind();

		glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

		T->release();
	}

	OPENGL_PLUGIN_DEBUG;

	mShader.release();
}
