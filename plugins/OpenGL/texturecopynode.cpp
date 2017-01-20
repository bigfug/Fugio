#include "texturecopynode.h"

#include <fugio/core/uuid.h>
#include <fugio/context_interface.h>
#include <fugio/performance.h>

#include <QSurfaceFormat>

#include "openglplugin.h"

TextureCopyNode::TextureCopyNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mFBO( 0 )
{
	mPinTexSrc = pinInput( "Texture" );

	mTexDst = pinOutput<fugio::OpenGLTextureInterface *>( "Texture", mPinTexDst, PID_OPENGL_TEXTURE );

	mPinTexSrc->setDescription( tr( "The source OpenGL Texture" ) );

	mPinTexDst->setDescription( tr( "The destination OpenGL Texture to copy the source texture into" ) );
}

TextureCopyNode::~TextureCopyNode()
{
	if( mFBO )
	{
		glDeleteFramebuffers( 1, &mFBO );

		mFBO = 0;
	}
}

void TextureCopyNode::inputsUpdated( qint64 pTimeStamp )
{
	fugio::Performance	Perf( mNode, "inputsUpdated", pTimeStamp );

	NodeControlBase::inputsUpdated( pTimeStamp );

	if( !OpenGLPlugin::hasContextStatic() )
	{
		return;
	}

	if( !mPinTexSrc->isConnected() || !mPinTexDst->isConnectedToActiveNode() )
	{
		return;
	}

	fugio::OpenGLTextureInterface	*TexSrc = qobject_cast<fugio::OpenGLTextureInterface *>( mPinTexSrc->connectedPin()->control()->qobject() );
	fugio::OpenGLTextureInterface	*TexDst = mTexDst;
	QVector3D				 SrcSze = TexSrc->size();

	quint32				 SrcCnt = 0;

	for( int i = 0 ; i < 3 ; i++ )
	{
		SrcCnt += SrcSze[ i ];
	}

	if( SrcCnt == 0 )
	{
		return;
	}

	QVector3D			 DstSze = TexDst->size();

	quint32				 DstCnt = 0;

	for( int i = 0 ; i < 3 ; i++ )
	{
		DstCnt += ( SrcSze[ i ] != DstSze[ i ] ? 1 : 0 );
	}

	if( DstCnt )
	{
		TexDst->setSize( SrcSze[ 0 ], SrcSze[ 1 ], SrcSze[ 2 ] );
		TexDst->setFormat( TexSrc->format() );
		TexDst->setInternalFormat( TexSrc->internalFormat() );
		TexDst->setTarget( TexSrc->target() );
		TexDst->setType( TexSrc->type() );
		TexDst->setWrap( TexSrc->wrapS(), TexSrc->wrapT(), TexSrc->wrapR() );

		TexDst->update();
	}

	if( TexDst->dstTexId() )
	{
#if !defined( GL_ES_VERSION_2_0 )
		if( glCopyImageSubData )
		{
			glCopyImageSubData( TexSrc->srcTexId(), TexSrc->target(), 0, 0, 0, 0,
								TexDst->dstTexId(), TexDst->target(), 0, 0, 0, 0,
								SrcSze[ 0 ], SrcSze[ 1 ], qMax<GLsizei>( SrcSze[ 2 ], 1 ) );
		}
		else
		{
			if( !mFBO )
			{
				glGenFramebuffers( 1, &mFBO );
			}

			if( mFBO )
			{
				glBindFramebuffer( GL_FRAMEBUFFER, mFBO );

				glFramebufferTexture2D( GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, TexSrc->target(), TexSrc->srcTexId(), 0 );

				glFramebufferTexture2D( GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, TexDst->target(), TexDst->dstTexId(), 0 );

				GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT1 };

				glDrawBuffers( 1, drawBuffers );

				glBlitFramebuffer( 0, 0, SrcSze[ 0 ], SrcSze[ 1 ], 0, 0, SrcSze[ 0 ], SrcSze[ 1 ], GL_COLOR_BUFFER_BIT, GL_NEAREST );

				glBindFramebuffer( GL_FRAMEBUFFER, 0 );
			}
		}
#endif
	}

	mNode->context()->pinUpdated( mPinTexDst );
}

