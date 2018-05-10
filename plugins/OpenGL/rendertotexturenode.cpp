#include "rendertotexturenode.h"

#include <QTimer>
#include <QCoreApplication>

#include <fugio/opengl/texture_interface.h>

#include "openglplugin.h"

RenderToTextureNode::RenderToTextureNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mFBO( 0 )
{
	FUGID( PIN_INPUT_RENDER, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );
	FUGID( PIN_OUTPUT_TEXTURE, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );

	mPinInputTrigger = pinInput( "Trigger", PID_FUGIO_NODE_TRIGGER );

	mPinInputRender = pinInput( "Render", PIN_INPUT_RENDER );

	mPinOutputTexture = pinOutput( "Texture", PIN_OUTPUT_TEXTURE );
}

bool RenderToTextureNode::initialise()
{
	if( !NodeControlBase::initialise() )
	{
		return( false );
	}

	if( !OpenGLPlugin::hasContextStatic() )
	{
		return( false );
	}

	return( true );
}

void RenderToTextureNode::inputsUpdated( qint64 pTimeStamp )
{
	if( !OpenGLPlugin::hasContextStatic() )
	{
		return;
	}

	if( QThread::currentThread() != QCoreApplication::instance()->thread() )
	{
		return;
	}

	initializeOpenGLFunctions();

	fugio::RenderInterface			*Render = input<fugio::RenderInterface *>( mPinInputRender );
	fugio::OpenGLTextureInterface	*Texture = output<fugio::OpenGLTextureInterface *>( mPinOutputTexture );

	if( Render && Texture && !Texture->size().isNull() )
	{
		GLint		CurVP[ 4 ];

		glGetIntegerv( GL_VIEWPORT, CurVP );

		GLint		FBOCur;

		glGetIntegerv( GL_FRAMEBUFFER_BINDING, &FBOCur );

		QSize		TexSze = QSize( Texture->size().x(), Texture->size().y() );

		if( mFBO && mFBOSize != TexSze )
		{
			glDeleteFramebuffers( 1, &mFBO );

			mFBO = 0;
			mFBOSize = QSize();
		}

		if( !mFBO )
		{
			glGenFramebuffers( 1, &mFBO );

			if( mFBO )
			{
				mFBOSize = TexSze;
			}
		}

		if( mFBO )
		{
			glBindFramebuffer( GL_FRAMEBUFFER, mFBO );

			glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, Texture->target(), Texture->dstTexId(), 0 );

			glViewport( 0, 0, TexSze.width(), TexSze.height() );

			Render->render( pTimeStamp );

			glBindFramebuffer( GL_FRAMEBUFFER, FBOCur );

			glViewport( CurVP[ 0 ], CurVP[ 1 ], CurVP[ 2 ], CurVP[ 3 ] );

			pinUpdated( mPinOutputTexture );
		}
	}
}
