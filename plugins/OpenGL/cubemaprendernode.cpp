#include "cubemaprendernode.h"

#include <QMatrix4x4>

#include <fugio/core/uuid.h>
#include <fugio/math/uuid.h>

#include <fugio/context_interface.h>
#include <fugio/opengl/texture_interface.h>
#include <fugio/render_interface.h>

#include "openglplugin.h"

QList<QMatrix4x4>		 CubeMapRenderNode::mMatCam;

CubeMapRenderNode::CubeMapRenderNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mFBO( 0 ), mRBO( 0 )
{
	FUGID( PIN_INPUT_RENDER,	"9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_INPUT_MATCAM,	"55524cac-94a0-47e9-883b-b10b8a967c64" );
	FUGID( PIN_OUTPUT_TEXTURE,	"1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );
	FUGID( PIN_OUTPUT_DEPTH,	"b0489356-7f56-424f-8adb-04b88c456839" );
	FUGID( PIN_OUTPUT_MATCAM,	"5528b401-5b97-4f76-a8c0-ba9336a3aff6" );

	mPinInputTrigger = pinInput( "Trigger", PID_FUGIO_NODE_TRIGGER );

	mPinInputRender = pinInput( "Render", PIN_INPUT_RENDER );

	mPinInputMatCam = pinInput( "Matrix", PIN_INPUT_MATCAM );

	mValOutputMatCam = pinOutput<fugio::VariantInterface *>( "Matrix", mPinOutputMatCam, PID_MATRIX4, PIN_OUTPUT_MATCAM );

	mPinOutputTexture = pinOutput( "Texture", PIN_OUTPUT_TEXTURE );

	// Set up cube face project matrices

	if( mMatCam.isEmpty() )
	{
		QMatrix4x4		M4;

		M4.setToIdentity();		M4.lookAt( QVector3D( 0.0, 0.0, 0.0 ), QVector3D(  1.0, 0.0, 0.0 ), QVector3D( 0.0, -1.0, 0.0 ) );	mMatCam << M4;
		M4.setToIdentity();		M4.lookAt( QVector3D( 0.0, 0.0, 0.0 ), QVector3D( -1.0, 0.0, 0.0 ), QVector3D( 0.0, -1.0, 0.0 ) );	mMatCam << M4;

		M4.setToIdentity();		M4.lookAt( QVector3D( 0.0, 0.0, 0.0 ), QVector3D( 0.0,  1.0, 0.0 ), QVector3D( 0.0, 0.0, 1.0 ) );	mMatCam << M4;
		M4.setToIdentity();		M4.lookAt( QVector3D( 0.0, 0.0, 0.0 ), QVector3D( 0.0, -1.0, 0.0 ), QVector3D( 0.0, 0.0, -1.0 ) );	mMatCam << M4;

		M4.setToIdentity();		M4.lookAt( QVector3D( 0.0, 0.0, 0.0 ), QVector3D( 0.0, 0.0,  1.0 ), QVector3D( 0.0, -1.0, 0.0 ) );	mMatCam << M4;
		M4.setToIdentity();		M4.lookAt( QVector3D( 0.0, 0.0, 0.0 ), QVector3D( 0.0, 0.0, -1.0 ), QVector3D( 0.0, -1.0, 0.0 ) );	mMatCam << M4;
	}
}

void CubeMapRenderNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	if( mPinInputTrigger->isUpdated( pTimeStamp ) || mPinInputRender->isUpdated( pTimeStamp ) )
	{
		fugio::OpenGLTextureInterface	*Texture = output<fugio::OpenGLTextureInterface *>( mPinOutputTexture );

		if( Texture && Texture->target() == GL_TEXTURE_CUBE_MAP )
		{
			fugio::RenderInterface		*Render = input<fugio::RenderInterface *>( mPinInputRender );

			if( Render )
			{
				GLuint		TexId = Texture->dstTexId();

				if( TexId )
				{
					GLint		FBOCur, RBCur;

					glGetIntegerv( GL_FRAMEBUFFER_BINDING, &FBOCur );
					glGetIntegerv( GL_RENDERBUFFER_BINDING, &RBCur );

					GLint		VP[ 4 ];

					glGetIntegerv( GL_VIEWPORT, VP );

					if( !mRBO )
					{
						glGenRenderbuffers( 1, &mRBO );

						glBindRenderbuffer( GL_RENDERBUFFER, mRBO );

						glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, Texture->size().x(), Texture->size().y() );
					}

					if( !mFBO )
					{
						glGenFramebuffers( 1, &mFBO );
					}


					glViewport( 0, 0, Texture->size().x(), Texture->size().y() );

					for( int i = 0 ; i < 6 ; i++ )
					{
						mValOutputMatCam->setVariant( mMatCam.at( i ) );

						glBindFramebuffer( GL_FRAMEBUFFER, mFBO );

						glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, TexId, 0 );

						glBindRenderbuffer( GL_RENDERBUFFER, mRBO );

						glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mRBO );

						Render->render( pTimeStamp );
					}

					glViewport( VP[ 0 ], VP[ 1 ], VP[ 2 ], VP[ 3 ] );

					glBindRenderbuffer( GL_RENDERBUFFER, RBCur );
					glBindFramebuffer( GL_FRAMEBUFFER, FBOCur );

					pinUpdated( mPinOutputTexture );
				}
			}
		}
	}
}
