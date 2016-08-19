#include "rendernode.h"

#include <fugio/core/uuid.h>
#include <fugio/opengl/uuid.h>

#include <fugio/context_interface.h>
#include <fugio/opengl/texture_interface.h>
#include <fugio/opengl/state_interface.h>

#include "openglplugin.h"

RenderNode::RenderNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	mValOutput = pinOutput<fugio::RenderInterface *>( "Render", mPinOutput, PID_RENDER );

	mPinOutput->setDescription( tr( "The output render" ) );
}

bool RenderNode::initialise()
{
	if( !fugio::NodeControlBase::initialise() )
	{
		return( false );
	}

	if( !OpenGLPlugin::hasContextStatic() )
	{
		return( false );
	}

	return( true );
}

QList<QUuid> RenderNode::pinAddTypesInput() const
{
	static QList<QUuid>		PinLst;

	if( PinLst.isEmpty() )
	{
		PinLst << PID_RENDER;
		PinLst << PID_OPENGL_STATE;
	}

	return( PinLst );
}

bool RenderNode::canAcceptPin( fugio::PinInterface *pPin ) const
{
	if( pPin->direction() == PIN_INPUT )
	{
		return( false );
	}

	return( pinAddTypesInput().contains( pPin->controlUuid() ) );
}

void RenderNode::render( qint64 pTimeStamp, QUuid pSourcePinId )
{
	Q_UNUSED( pSourcePinId )

	OpenGLStateInterface							*CurrentState = 0;
	OpenGLStateInterface							*NextState;
	fugio::RenderInterface							*Geometry;
	QList<QSharedPointer<fugio::PinInterface>>		 InpPinLst = mNode->enumInputPins();

	for( QList< QSharedPointer<fugio::PinInterface> >::iterator it = InpPinLst.begin() ; it != InpPinLst.end() ; it++ )
	{
		QSharedPointer<fugio::PinInterface>	InpPin = *it;

		if( !InpPin->isConnectedToActiveNode() )
		{
			continue;
		}

		QSharedPointer<PinControlInterface>		 PinCtl = InpPin->connectedPin()->control();

		if( PinCtl.isNull() )
		{
			continue;
		}

		if( ( Geometry = qobject_cast<fugio::RenderInterface *>( PinCtl->qobject() ) ) != 0 )
		{
			Geometry->render( pTimeStamp );

			continue;
		}

		if( ( NextState = qobject_cast<OpenGLStateInterface *>( PinCtl->qobject() ) ) != 0 )
		{
			if( CurrentState != 0 )
			{
				CurrentState->stateEnd();
			}

			CurrentState = NextState;

			CurrentState->stateBegin();
		}
	}

	if( CurrentState != 0 )
	{
		CurrentState->stateEnd();
	}
}


void RenderNode::inputsUpdated( qint64 pTimeStamp )
{
	Q_UNUSED( pTimeStamp )

	pinUpdated( mPinOutput );
}
