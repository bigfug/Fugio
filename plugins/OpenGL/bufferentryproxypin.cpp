#include "bufferentryproxypin.h"
#include <fugio/node_interface.h>
#include <fugio/node_control_interface.h>
#include <fugio/opengl/buffer_interface.h>

BufferEntryProxyPin::BufferEntryProxyPin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin ), mBufferOffset( 0 ), mBufferStride( 0 )
{

}

const GLvoid *BufferEntryProxyPin::bind( GLuint pIndex, GLsizei pStride, const GLvoid *pPointer )
{
	if( !mPin->node() || !mPin->node()->hasControl() )
	{
		return( pPointer );
	}

	fugio::OpenGLBufferEntryInterface	*BufEnt = qobject_cast<fugio::OpenGLBufferEntryInterface *>( mPin->node()->control()->qobject() );

	if( BufEnt )
	{
		return( BufEnt->bind( pIndex, pStride, pPointer ) );
	}

	return( pPointer );
}

void BufferEntryProxyPin::bind( GLuint pIndex )
{
	if( !mBufferPin || !mBufferPin->isConnectedToActiveNode() || !mBufferPin->connectedPin()->hasControl() )
	{
		return;
	}

	fugio::OpenGLBufferInterface			*BufInt = qobject_cast<fugio::OpenGLBufferInterface *>( mBufferPin->connectedPin()->control()->qobject() );

	if( !BufInt )
	{
		return;
	}

	if( !mBufferEntryPin || !mBufferEntryPin->isConnectedToActiveNode() || !mBufferEntryPin->connectedPin()->hasControl() )
	{
		return;
	}

	fugio::OpenGLBufferEntryInterface		*BufEnt = qobject_cast<fugio::OpenGLBufferEntryInterface *>( mBufferEntryPin->connectedPin()->control()->qobject() );

	if( !BufEnt )
	{
		return;
	}

	if( BufInt->bind() )
	{
		BufEnt->bind( pIndex, mBufferStride, reinterpret_cast<GLvoid *>( mBufferOffset ) );
	}
}

void BufferEntryProxyPin::release( GLuint pIndex )
{
	if( !mBufferEntryPin || !mBufferEntryPin->isConnectedToActiveNode() || !mBufferEntryPin->connectedPin()->hasControl() )
	{
		return;
	}

	fugio::OpenGLBufferEntryInterface		*BufEnt = qobject_cast<fugio::OpenGLBufferEntryInterface *>( mBufferEntryPin->connectedPin()->control()->qobject() );

	if( !BufEnt )
	{
		return;
	}

	BufEnt->release( pIndex );
}

GLuint BufferEntryProxyPin::typeSize() const
{
	if( !mPin->node() || !mPin->node()->hasControl() )
	{
		return( 0 );
	}

	fugio::OpenGLBufferEntryInterface	*BufEnt = qobject_cast<fugio::OpenGLBufferEntryInterface *>( mPin->node()->control()->qobject() );

	return( BufEnt ? BufEnt->typeSize() : 0 );
}

GLuint BufferEntryProxyPin::entrySize() const
{
	if( !mBufferEntryPin || !mBufferEntryPin->isConnectedToActiveNode() || !mBufferEntryPin->connectedPin()->hasControl() )
	{
		return( 0 );
	}

	fugio::OpenGLBufferEntryInterface		*BufEnt = qobject_cast<fugio::OpenGLBufferEntryInterface *>( mBufferEntryPin->connectedPin()->control()->qobject() );

	return( BufEnt ? BufEnt->entrySize() : 0 );
}

bool BufferEntryProxyPin::normalised() const
{
	if( !mBufferEntryPin || !mBufferEntryPin->isConnectedToActiveNode() || !mBufferEntryPin->connectedPin()->hasControl() )
	{
		return( 0 );
	}

	fugio::OpenGLBufferEntryInterface		*BufEnt = qobject_cast<fugio::OpenGLBufferEntryInterface *>( mBufferEntryPin->connectedPin()->control()->qobject() );

	return( BufEnt ? BufEnt->normalised() : false );
}

void BufferEntryProxyPin::setSize(GLint pSize)
{
	if( !mBufferEntryPin || !mBufferEntryPin->isConnectedToActiveNode() || !mBufferEntryPin->connectedPin()->hasControl() )
	{
		return;
	}

	fugio::OpenGLBufferEntryInterface		*BufEnt = qobject_cast<fugio::OpenGLBufferEntryInterface *>( mBufferEntryPin->connectedPin()->control()->qobject() );

	if( BufEnt )
	{
		BufEnt->setSize( pSize );
	}
}

void BufferEntryProxyPin::setType(GLenum pType)
{
	if( !mBufferEntryPin || !mBufferEntryPin->isConnectedToActiveNode() || !mBufferEntryPin->connectedPin()->hasControl() )
	{
		return;
	}

	fugio::OpenGLBufferEntryInterface		*BufEnt = qobject_cast<fugio::OpenGLBufferEntryInterface *>( mBufferEntryPin->connectedPin()->control()->qobject() );

	if( BufEnt )
	{
		BufEnt->setType( pType );
	}
}

void BufferEntryProxyPin::setNormalised(GLboolean pNormalised)
{
	if( !mBufferEntryPin || !mBufferEntryPin->isConnectedToActiveNode() || !mBufferEntryPin->connectedPin()->hasControl() )
	{
		return;
	}

	fugio::OpenGLBufferEntryInterface		*BufEnt = qobject_cast<fugio::OpenGLBufferEntryInterface *>( mBufferEntryPin->connectedPin()->control()->qobject() );

	if( BufEnt )
	{
		BufEnt->setNormalised( pNormalised );
	}
}
