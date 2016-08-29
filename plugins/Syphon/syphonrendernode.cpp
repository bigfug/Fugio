#include "syphonrendernode.h"

#include <fugio/core/uuid.h>
#include <fugio/opengl/uuid.h>
#include <fugio/syphon/uuid.h>
#include <fugio/opengl/state_interface.h>
#include <fugio/render_interface.h>

#include <fugio/context_interface.h>
#include <fugio/context_signals.h>

#include "syphonplugin.h"
#include "syphonpin.h"

SyphonRenderNode::SyphonRenderNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_TEXTURE, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_INPUT_NAME, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );
	FUGID( PIN_INPUT_SIZE, "261cc653-d7fa-4c34-a08b-3603e8ae71d5" );

	mPinInputRender = pinInput( "Render", PIN_INPUT_TEXTURE );

	mPinInputName    = pinInput( "Name", PIN_INPUT_NAME );

	mPinInputSize = pinInput( "Size", PIN_INPUT_SIZE );

	mPinInputRender->registerPinInputType( PID_RENDER );

	mPinInputName->registerPinInputType( PID_STRING );

	mPinInputSize->registerPinInputType( PID_SIZE );

	static int		SenderId = 0;

	mPinInputName->setValue( QString( "Fugio-Sender-%1" ).arg( SenderId++ ) );
}

bool SyphonRenderNode::initialise()
{
	if( !NodeControlBase::initialise() )
	{
		return( false );
	}

	if( !SyphonPlugin::instance()->hasOpenGLContext() )
	{
		return( false );
	}

	return( true );
}

void SyphonRenderNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

#if defined( SYPHON_SUPPORTED )
	if( mPinInputName->isUpdated( pTimeStamp ) )
	{
		mSender.setServerName( variant( mPinInputName ).toString().toStdString() );
	}

	if( mPinInputRender->isUpdated( pTimeStamp ) )
	{
		const QSize		S = variant( mPinInputSize ).toSize();

		if( !S.isEmpty() )
		{
			fugio::RenderInterface *Render = input<fugio::RenderInterface *>( mPinInputRender );

			if( mSender.bindToDrawFrameOfSize( S.width(), S.height() ) )
			{
				Render->render( pTimeStamp );

				mSender.unbindAndPublish();
			}
		}
	}
#endif
}
