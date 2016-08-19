#include "contextnode.h"

#include "openglplugin.h"

ContextNode::ContextNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
}

bool ContextNode::initialise()
{
	if( !NodeControlBase::initialise() )
	{
		return( false );
	}

	if( !mOutput )
	{
		if( ( mOutput = DeviceOpenGLOutput::newDevice( true ) ) == nullptr )
		{
			return( false );
		}

		mOutput->setCurrentNode( mNode );

		mNode->context()->nodeInitialised();
	}

	if( !OpenGLPlugin::hasContextStatic() )
	{
		return( false );
	}

	mOutput->hide();

	return( true );
}

bool ContextNode::deinitialise()
{
	mOutput.clear();

	return( NodeControlBase::deinitialise() );
}
