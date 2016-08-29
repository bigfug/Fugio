#include "syphonsendernode.h"

#include <fugio/core/uuid.h>
#include <fugio/opengl/uuid.h>
#include <fugio/syphon/uuid.h>

#include <fugio/context_interface.h>
#include <fugio/context_signals.h>

#include "syphonplugin.h"
#include "syphonpin.h"

SyphonSenderNode::SyphonSenderNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mWidth( 0 ), mHeight( 0 )
{
	FUGID( PIN_INPUT_TEXTURE, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_INPUT_NAME, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );

	memset( mName, 0, sizeof( mName ) );

	mPinInputTexture = pinInput( "Texture", PIN_INPUT_TEXTURE );

	mPinInputName    = pinInput( "Name", PIN_INPUT_NAME );

	mPinInputTexture->registerPinInputType( PID_OPENGL_TEXTURE );

	mPinInputName->registerPinInputType( PID_STRING );

	static int		SenderId = 0;

	mPinInputName->setValue( QString( "Fugio-Sender-%1" ).arg( SenderId++ ) );
}

bool SyphonSenderNode::initialise()
{
	if( !NodeControlBase::initialise() )
	{
		return( false );
	}

	if( !SyphonPlugin::instance()->hasOpenGLContext() )
	{
		return( false );
	}

	//	connect( mNode->context()->context(), SIGNAL(frameInitialise()), this, SLOT(onContextFrame()) );

	return( true );
}

bool SyphonSenderNode::deinitialise()
{
	//	disconnect( mNode->context()->context(), SIGNAL(frameInitialise()), this, SLOT(onContextFrame()) );

	return( NodeControlBase::deinitialise() );
}

void SyphonSenderNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

#if defined( SYPHON_SUPPORTED )
	if( mPinInputName->isUpdated( pTimeStamp ) )
	{
		mSender.setServerName( variant( mPinInputName ).toString().toStdString() );
	}

	if( mPinInputTexture->isUpdated( pTimeStamp ) )
	{
		OpenGLTextureInterface		*TexSrc = input<OpenGLTextureInterface *>( mPinInputTexture );

		if( TexSrc && TexSrc->dstTexId() && !TexSrc->size().isNull() && ( TexSrc->target() == GL_TEXTURE_2D || TexSrc->target() == GL_TEXTURE_RECTANGLE ) )
		{
			mSender.publishTexture( TexSrc->dstTexId(), TexSrc->target(), TexSrc->size().x(), TexSrc->size().y(), false );
		}
	}
#endif
}
