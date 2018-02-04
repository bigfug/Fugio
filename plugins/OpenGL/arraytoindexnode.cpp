#include "arraytoindexnode.h"

#include <fugio/core/uuid.h>
#include <fugio/opengl/uuid.h>
#include <fugio/performance.h>

#include <fugio/context_interface.h>
#include <fugio/core/variant_interface.h>
#include <fugio/node_signals.h>

#include "openglplugin.h"
#include "bufferpin.h"

ArrayToIndexNode::ArrayToIndexNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_ARRAY,		"da4453c5-c72d-4c40-aff6-453380d0cf13" );
	FUGID( PIN_OUTPUT_BUFFER,	"d68e830c-cc15-430f-a444-02590043006f" );

	mPinInputArray  = pinInput( "Array", PIN_INPUT_ARRAY );

//	mPinInputArray->registerPinInputType( PID_ARRAY );

	mValOutputBuffer = pinOutput<fugio::OpenGLBufferInterface *>( "Index", mPinOutputBuffer, PID_OPENGL_BUFFER, PIN_OUTPUT_BUFFER );

	mNode->pairPins( mPinInputArray, mPinOutputBuffer );
}

bool ArrayToIndexNode::initialise()
{
	if( !NodeControlBase::initialise() )
	{
		return( false );
	}

	if( !OpenGLPlugin::hasContextStatic() )
	{
		return( false );
	}

	return( true );
}

void ArrayToIndexNode::inputsUpdated( qint64 pTimeStamp )
{
	fugio::Performance	Perf( mNode, "inputsUpdated", pTimeStamp );

	NodeControlBase::inputsUpdated( pTimeStamp );

	if( !OpenGLPlugin::hasContextStatic() )
	{
		return;
	}

	for( fugio::NodeInterface::UuidPair UP : mNode->pairedPins() )
	{
		QSharedPointer<fugio::PinInterface>  PinI = mNode->findPinByLocalId( UP.first );
		QSharedPointer<fugio::PinInterface>  PinO = mNode->findPinByLocalId( UP.second );
		fugio::OpenGLBufferInterface		*BufO = qobject_cast<fugio::OpenGLBufferInterface *>( PinO && PinO->hasControl() ? PinO->control()->qobject() : nullptr );

		if( !PinI || !PinO || !BufO )
		{
			continue;
		}

		if( !PinI->isUpdated( pTimeStamp ) )
		{
			continue;
		}

		if( BufO->target() != QOpenGLBuffer::IndexBuffer )
		{
			BufO->setTarget( QOpenGLBuffer::IndexBuffer );
		}

		fugio::VariantInterface			*A;

		if( ( A = input<fugio::VariantInterface *>( PinI ) ) == nullptr )
		{
			continue;
		}

		if( BufO->buffer() && BufO->buffer()->isCreated() && ( A->variantType() != BufO->type() || A->variantElementCount() != BufO->size() || A->variantStride() != BufO->stride() || A->variantCount() != BufO->count() ) )
		{
			BufO->clear();
		}

		if( A->variantCount() <= 0 )
		{
			continue;
		}

		if( ( !BufO->buffer() || !BufO->buffer()->isCreated() ) && !BufO->alloc( A->variantType(), A->variantElementCount(), A->variantStride(), A->variantCount() ) )
		{
			BufO->clear();

			continue;
		}
		else if( !BufO->bind() )
		{
			continue;
		}

		BufO->setIndex( true );

		BufO->buffer()->write( 0, A->variantArray(), A->variantArraySize() );

		BufO->release();

		OPENGL_DEBUG( mNode->name() );

		pinUpdated( PinO );
	}
}

QUuid ArrayToIndexNode::pairedPinControlUuid( QSharedPointer<fugio::PinInterface> pPin ) const
{
	return( pPin->direction() == PIN_INPUT ? PID_OPENGL_BUFFER : QUuid() );
}


QList<QUuid> ArrayToIndexNode::pinAddTypesInput() const
{
	static QList<QUuid> PinLst =
	{
		PID_ARRAY
	};

	return( PinLst );
}

bool ArrayToIndexNode::canAcceptPin( PinInterface *pPin ) const
{
	return( pPin->direction() == PIN_OUTPUT );
}
