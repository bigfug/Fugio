#include "texturecopynode.h"

#include <fugio/core/uuid.h>
#include <fugio/context_interface.h>
#include <fugio/performance.h>

#include <QSurfaceFormat>

#if QT_VERSION >= QT_VERSION_CHECK( 5, 6, 0 )
#include <QOpenGLExtraFunctions>
#endif

#include <QOpenGLFunctions_4_3_Core>

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

	initializeOpenGLFunctions();

	fugio::OpenGLTextureInterface	*TexSrc = input<fugio::OpenGLTextureInterface *>( mPinTexSrc );
	fugio::OpenGLTextureInterface	*TexDst = mTexDst;

	if( !TexSrc || !TexDst )
	{
		return;
	}

	fugio::OpenGLTextureDescription		SrcDsc = TexSrc->textureDescription();
	fugio::OpenGLTextureDescription		DstDsc = TexDst->textureDescription();

	if( memcmp( &SrcDsc, &DstDsc, sizeof( fugio::OpenGLTextureDescription ) ) != 0 )
	{
		TexDst->setTextureDescription( SrcDsc );

		TexDst->update();
	}

	if( TexDst->dstTexId() )
	{
		const QVector3D		TexSze = TexSrc->size();

#if !defined( QT_OPENGL_ES_2 )
		QOpenGLFunctions_4_3_Core	*GL43 = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_4_3_Core>();

		if( GL43 && !GL43->initializeOpenGLFunctions() )
		{
			GL43 = Q_NULLPTR;
		}

		if( GL43 )
		{
			GL43->glCopyImageSubData( TexSrc->srcTexId(), TexSrc->target(), 0, 0, 0, 0,
									  TexDst->dstTexId(), TexDst->target(), 0, 0, 0, 0,
									  TexSze.x(), TexSze.y(), qMax<GLsizei>( TexSze.z(), 1 ) );
		}
		else
#endif
		{
#if defined( QOPENGLEXTRAFUNCTIONS_H )
			QOpenGLExtraFunctions	*GLEX = QOpenGLContext::currentContext()->extraFunctions();

			if( GLEX )
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

					GLEX->glDrawBuffers( 1, drawBuffers );

					GLEX->glBlitFramebuffer( 0, 0, TexSze.x(), TexSze.y(), 0, 0, TexSze.x(), TexSze.y(), GL_COLOR_BUFFER_BIT, GL_NEAREST );

					glBindFramebuffer( GL_FRAMEBUFFER, 0 );
				}
			}
#endif
		}

		pinUpdated( mPinTexDst );
	}
}

