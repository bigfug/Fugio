#include "clearnode.h"

#include <fugio/core/uuid.h>
#include <fugio/opengl/uuid.h>

#include <fugio/context_interface.h>
#include <fugio/opengl/texture_interface.h>
#include <fugio/opengl/state_interface.h>

#include "openglplugin.h"

ClearNode::ClearNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	mValOutput = pinOutput<fugio::RenderInterface *>( "Render", mPinOutput, PID_RENDER );

	mPinOutput->setDescription( tr( "The output geometry" ) );
}

bool ClearNode::initialise()
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

void ClearNode::render( qint64 pTimeStamp, QUuid pSourcePinId )
{
	Q_UNUSED( pTimeStamp )
	Q_UNUSED( pSourcePinId )

	initializeOpenGLFunctions();

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}
