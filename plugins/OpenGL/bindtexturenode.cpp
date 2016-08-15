#include "bindtexturenode.h"

#include <fugio/core/uuid.h>
#include <fugio/opengl/uuid.h>

#include <fugio/context_interface.h>
#include <fugio/opengl/texture_interface.h>
#include <fugio/opengl/state_interface.h>

#include "openglplugin.h"

BindTextureNode::BindTextureNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	for( int i = 0 ; i < 4 ; i++ )
	{
		QSharedPointer<fugio::PinInterface>	P = pinInput( QString( "Texture%1" ).arg( i ) );

		P->setDescription( tr( "The OpenGL texture to bind to texture unit %1" ).arg( i ) );
	}

	mPinState    = pinInput( "State" );
	mPinGeometry = pinInput( "Geometry" );

	mValOutput = pinOutput<fugio::RenderInterface *>( "Render", mPinOutput, PID_RENDER );

	mPinState->setDescription( tr( "The OpenGL rendering state to apply" ) );

	mPinGeometry->setDescription( tr( "The input 3D Geometry to render" ) );

	mPinOutput->setDescription( tr( "The output geometry" ) );
}

bool BindTextureNode::initialise()
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

void BindTextureNode::inputsUpdated( qint64 pTimeStamp )
{
	if( !pTimeStamp )
	{
		return;
	}

	pinUpdated( mPinOutput );
}

void BindTextureNode::render( qint64 pTimeStamp, QUuid pSourcePinId )
{
	Q_UNUSED( pSourcePinId )

	OpenGLTextureInterface		*T;

	QList<QSharedPointer<fugio::PinInterface>>		PinLst = mNode->enumInputPins();

	for( int i = 0 ; i < 4 ; i++ )
	{
		glActiveTexture( GL_TEXTURE0 + i );

		if( ( T = input<OpenGLTextureInterface *>( PinLst[ i ] ) ) != nullptr )
		{
			T->srcBind();

			glEnable( T->target() );
		}
	}

	//-------------------------------------------------------------------------

	OpenGLStateInterface		*CurrentState = 0;
	OpenGLStateInterface		*NextState;
	fugio::RenderInterface		*Geometry;

	//-------------------------------------------------------------------------

	for( QSharedPointer<fugio::PinInterface> InpPin : PinLst )
	{
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

	//-------------------------------------------------------------------------

	for( int i = 0 ; i < PinLst.size() ; i++ )
	{
		glActiveTexture( GL_TEXTURE0 + i );

		if( ( T = input<OpenGLTextureInterface *>( PinLst[ i ] ) ) != nullptr )
		{
			T->release();

			glDisable( T->target() );
		}
	}
}
