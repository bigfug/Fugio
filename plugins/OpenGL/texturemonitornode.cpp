#include "texturemonitornode.h"

#include <QMainWindow>

#include "openglplugin.h"

#include <fugio/opengl/uuid.h>

#include <fugio/editor_interface.h>

#include <fugio/opengl/texture_interface.h>

TextureMonitorNode::TextureMonitorNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mDockWidget( nullptr ), mWidget( nullptr ), mDockArea( Qt::BottomDockWidgetArea ),
	  mVAO( 0 ), mProgram( 0 ), mVBO( 0 )
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

	glClearColor( 0, 0, 0, 0 );

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	OPENGL_PLUGIN_DEBUG;

	if( !mVAO )
	{
		glGenVertexArrays( 1, &mVAO );
	}

	if( mVAO )
	{
		glBindVertexArray( mVAO );
	}

	OPENGL_PLUGIN_DEBUG;

	if( !mVBO )
	{
		float Vertices[] = {
			-1, -1,
			-1, 1,
			1, -1,
			1, 1
		};

		glGenBuffers( 1, &mVBO );

		glBindBuffer( GL_ARRAY_BUFFER, mVBO );

		glBufferData( GL_ARRAY_BUFFER, sizeof( Vertices ), Vertices, GL_STATIC_DRAW );
	}

	OPENGL_PLUGIN_DEBUG;

	if( !mProgram )
	{
		const char *vertexSource =
				"#version 330\n"
				"in vec2 position;\n"
				"out vec2 tpos;\n"
				"void main()\n"
				"{\n"
				"	gl_Position = vec4( position, 0.0, 1.0 );\n"
				"	tpos = ( position * 0.5 ) + 0.5;\n"
				"	tpos.y = 1.0 - tpos.y;\n"
				"}\n";

		GLuint vertexShader = glCreateShader( GL_VERTEX_SHADER );

		glShaderSource( vertexShader, 1, &vertexSource, NULL );

		glCompileShader( vertexShader );

		GLint status;

		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);

		if( !status )
		{
			mNode->setStatus( fugio::NodeInterface::Error );

			return;
		}

		const char *fragmentSource =
				"#version 330\n"
				"uniform sampler2D tex;"
				"in vec2 tpos;\n"
				"out vec4 outColor;\n"
				"void main()\n"
				"{\n"
				"	outColor = vec4( texture( tex, tpos ).rgb, 1.0 );\n"
				"}\n";

		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

		glShaderSource(fragmentShader, 1, &fragmentSource, NULL);

		glCompileShader(fragmentShader);

		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);

		if( !status )
		{
			mNode->setStatus( fugio::NodeInterface::Error );

			return;
		}

		mProgram = glCreateProgram();

		glAttachShader( mProgram, vertexShader);
		glAttachShader( mProgram, fragmentShader);

		glLinkProgram( mProgram );

		OPENGL_PLUGIN_DEBUG;

		glGetProgramiv( mProgram, GL_LINK_STATUS, &status );

		if( !status )
		{
			mNode->setStatus( fugio::NodeInterface::Error );

			return;
		}

		glUseProgram( mProgram );

		GLint posAttrib = glGetAttribLocation( mProgram, "position" );

		glVertexAttribPointer( posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0 );

		glEnableVertexAttribArray( posAttrib );
	}

	OPENGL_PLUGIN_DEBUG;

	if( mProgram )
	{
		glUseProgram( mProgram );
	}

	OPENGL_PLUGIN_DEBUG;

	QList<fugio::OpenGLTextureInterface *>	TexLst;

	for( QSharedPointer<fugio::PinInterface> P : mNode->enumInputPins() )
	{
		fugio::OpenGLTextureInterface	*T = input<fugio::OpenGLTextureInterface *>( P );

		if( T )
		{
			TexLst << T;
		}
	}

	if( TexLst.isEmpty() )
	{
		return;
	}

	double	tw = 1.0 / double( TexLst.size() );

	for( int i = 0 ; i < TexLst.size() ; i++ )
	{
		fugio::OpenGLTextureInterface *T = TexLst[ i ];

		glViewport( tw * i * mWidget->width() * mWidget->devicePixelRatio(), 0, tw * mWidget->width() * mWidget->devicePixelRatio(), mWidget->height() * mWidget->devicePixelRatio() );

		T->srcBind();

		glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

		T->release();
	}

	OPENGL_PLUGIN_DEBUG;

	if( mProgram )
	{
		glUseProgram( 0 );
	}

	if( mVAO )
	{
		glBindVertexArray( 0 );
	}

	OPENGL_PLUGIN_DEBUG;
}
