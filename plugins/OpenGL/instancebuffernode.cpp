#include "instancebuffernode.h"

#include <fugio/opengl/buffer_interface.h>

InstanceBufferNode::InstanceBufferNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	mPinInput = pinInput( "Input", next_uuid() );
	mPinOutput = pinOutput( "Output", next_uuid() );
}

void InstanceBufferNode::inputsUpdated( qint64 pTimeStamp )
{
	Q_UNUSED( pTimeStamp )

	if( !mPinInput->isConnected() || mPinOutput->control() != mPinInput->connectedPin()->control() )
	{
		if( mPinOutput->hasControl() )
		{
			fugio::OpenGLBufferInterface	*BufInt = qobject_cast<fugio::OpenGLBufferInterface *>( mPinOutput->control()->qobject() );

			if( BufInt )
			{
				BufInt->setInstanced( false );
			}

			mPinOutput->setControl( QSharedPointer<fugio::PinControlInterface>() );

			pinUpdated( mPinOutput );
		}
	}

	if( mPinInput->isConnected() && !mPinOutput->hasControl() )
	{
		QSharedPointer<fugio::PinControlInterface>		 BufCtl = mPinInput->connectedPin()->control();

		fugio::OpenGLBufferInterface					*BufInt = qobject_cast<fugio::OpenGLBufferInterface *>( BufCtl ? BufCtl->qobject() : nullptr );

		if( BufInt )
		{
			mPinOutput->setControl( BufCtl );

			BufInt->setInstanced( true );

			pinUpdated( mPinOutput );
		}
	}
}
