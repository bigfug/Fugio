#include "buffertoarraynode.h"

#include <QOpenGLFunctions_4_5_Core>

#include <fugio/core/uuid.h>
#include <fugio/opengl/uuid.h>

#include <fugio/context_interface.h>
#include <fugio/opengl/texture_interface.h>
#include <fugio/opengl/state_interface.h>
#include <fugio/opengl/buffer_interface.h>

#include "openglplugin.h"

BufferToArrayNode::BufferToArrayNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_BUFFER,	"cf69720c-38fd-4f5e-8a88-3698854b46d9" );
	FUGID( PIN_ARRAY,	"a9f9235a-a47e-4031-97db-cd7532e97858" );

	mPinBuffer = pinInput( "Buffer", PIN_BUFFER );

	mValArray = pinOutput<fugio::ArrayInterface *>( "Array", mPinArray, PID_ARRAY, PIN_ARRAY );
}

bool BufferToArrayNode::initialise()
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

void BufferToArrayNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	fugio::OpenGLBufferInterface	*BufInt = input<fugio::OpenGLBufferInterface *>( mPinBuffer );

	if( !BufInt || !BufInt->buffer().isCreated() )
	{
		return;
	}

	if( BufInt->type() != mValArray->type() ||
			BufInt->size() != mValArray->size() ||
			BufInt->count() != mValArray->count() ||
			BufInt->stride() != mValArray->stride() )
	{
		mValArray->setCount( BufInt->count() );
		mValArray->setSize( BufInt->size() );
		mValArray->setStride( BufInt->stride() );
		mValArray->setType( BufInt->type() );
	}

	int			 ArrLen = mValArray->size() * mValArray->count();

	if( !ArrLen )
	{
		return;
	}

	void		*ArrPtr = mValArray->array();

	if( !ArrPtr )
	{
		return;
	}

	QOpenGLFunctions_4_5_Core	*GL45 = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_4_5_Core>();

	if( GL45 && GL45->initializeOpenGLFunctions() )
	{
		GL45->glGetNamedBufferSubData( BufInt->buffer().bufferId(), 0, ArrLen, ArrPtr );
	}
	else if( BufInt->bind() )
	{
		BufInt->buffer().write( 0, ArrPtr, ArrLen );

		BufInt->release();
	}

	GLfloat		feedback[ 16 ];

	memcpy( feedback, ArrPtr, std::min<size_t>( sizeof( feedback ), ArrLen ) );

	OPENGL_DEBUG( mNode->name() );

	pinUpdated( mPinArray );
}
