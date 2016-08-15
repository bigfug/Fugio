#include "viewportnode.h"

#include <QSurface>

#include <fugio/core/uuid.h>
#include <fugio/opengl/uuid.h>

#include <fugio/context_interface.h>
#include <fugio/opengl/state_interface.h>

#include "openglplugin.h"

ViewportNode::ViewportNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mUpdateViewport( true )
{
	mPinX = pinInput( "X" );
	mPinY = pinInput( "Y" );
	mPinW = pinInput( "Width" );
	mPinH = pinInput( "Height" );

	mPinX->setValue( 0.0 );
	mPinY->setValue( 0.0 );
	mPinW->setValue( 1.0 );
	mPinH->setValue( 1.0 );

	mValOutput = pinOutput<fugio::RenderInterface *>( "Render", mPinOutput, PID_RENDER );

	mPinX->setDescription( tr( "The X position of the Viewport, either as a ratio of the current viewport (0.0 to 1.0), or in pixels" ) );
	mPinY->setDescription( tr( "The Y position of the Viewport, either as a ratio of the current viewport (0.0 to 1.0), or in pixels" ) );
	mPinW->setDescription( tr( "The width of the Viewport, either as a ratio of the current viewport (0.0 to 1.0), or in pixels" ) );
	mPinH->setDescription( tr( "The height of the Viewport, either as a ratio of the current viewport (0.0 to 1.0), or in pixels" ) );

	mPinOutput->setDescription( tr( "The output geometry that can be connected to an OpenGL Window" ) );
}

void ViewportNode::inputsUpdated( qint64 pTimeStamp )
{
	Q_UNUSED( pTimeStamp )

	mUpdateViewport = true;

	pinUpdated( mPinOutput );
}


QList<QUuid> ViewportNode::pinAddTypesInput() const
{
	QList<QUuid>		PinIds;

	PinIds << PID_OPENGL_STATE;
	PinIds << PID_RENDER;

	return( PinIds );
}

bool ViewportNode::canAcceptPin( fugio::PinInterface *pPin ) const
{
	return( pPin->direction() == PIN_OUTPUT );
}

void ViewportNode::render( qint64 pTimeStamp, QUuid pSourcePinId )
{
	Q_UNUSED( pTimeStamp )
	Q_UNUSED( pSourcePinId )

	if( !OpenGLPlugin::hasContextStatic() )
	{
		return;
	}

#if 0
	GLint		CurVP[ 4 ];

	glGetIntegerv( GL_VIEWPORT, CurVP );

	QSurface		*Surface = QOpenGLContext::currentContext()->surface();

	if( !Surface )
	{
		return;
	}

	if( mUpdateViewport )
	{
		mVP.setX( variant( mPinX ).toReal() );
		mVP.setY( variant( mPinY ).toReal() );
		mVP.setWidth( variant( mPinW ).toReal() );
		mVP.setHeight( variant( mPinH ).toReal() );

		QSizeF			 SurfaceSize = Surface->size();

		if( mVP.x() <= 1.0 )
		{
			mVP.setX( mVP.x() * SurfaceSize.width() );
		}

		if( mVP.y() <= 1.0 )
		{
			mVP.setY( mVP.y() * SurfaceSize.height() );
		}

		if( mVP.width() <= 1.0 )
		{
			mVP.setWidth( mVP.width() * SurfaceSize.width() );
		}

		if( mVP.height() <= 1.0 )
		{
			mVP.setHeight( mVP.height() * SurfaceSize.height() );
		}

		mUpdateViewport = false;
	}

	glViewport( mVP.x(), mVP.y(), mVP.width(), mVP.height() );

	int		PinIdx = 0;

	fugio::OpenGLStateInterface		*CurrentState = 0;

	foreach( QSharedPointer<fugio::PinInterface> P, mNode->enumInputPins() )
	{
		Q_ASSERT( PinIdx == P->order() );

		PinIdx++;

		if( !P->isConnected() || P->connectedPin().isNull() || !P->connectedNode()->isActive() || P->connectedPin()->control().isNull() )
		{
			continue;
		}

		QObject					*O = P->connectedPin()->control()->object();

		if( O == 0 )
		{
			continue;
		}

		if( true )
		{
			fugio::RenderInterface		*Geometry = qobject_cast<fugio::RenderInterface *>( O );

			if( Geometry != 0 )
			{
				Geometry->drawGeometry();

				continue;
			}
		}

		if( true )
		{
			fugio::OpenGLStateInterface		*NextState = qobject_cast<fugio::OpenGLStateInterface *>( O );

			if( NextState != 0 )
			{
				if( CurrentState != 0 )
				{
					CurrentState->stateEnd();
				}

				CurrentState = NextState;

				CurrentState->stateBegin();

				continue;
			}
		}
	}

	if( CurrentState != 0 )
	{
		CurrentState->stateEnd();
	}

	glViewport( CurVP[ 0 ], CurVP[ 1 ], CurVP[ 2 ], CurVP[ 3 ] );
#endif
}
