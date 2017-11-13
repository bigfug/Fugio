#include "arraytotexturebuffernode.h"

#include <fugio/core/uuid.h>
#include <fugio/opengl/uuid.h>
#include <fugio/performance.h>

#include <fugio/context_interface.h>
#include <fugio/core/variant_interface.h>
#include <fugio/core/array_interface.h>
#include <fugio/node_signals.h>

#include "openglplugin.h"

ArrayToTextureBufferNode::ArrayToTextureBufferNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_ARRAY,		"da4453c5-c72d-4c40-aff6-453380d0cf13" );
	FUGID( PIN_OUTPUT_BUFFER,	"d68e830c-cc15-430f-a444-02590043006f" );

	mPinInputArray  = pinInput( "Array", PIN_INPUT_ARRAY );

	mPinInputArray->registerPinInputType( PID_ARRAY );

	mValOutputBuffer = pinOutput<fugio::OpenGLTextureInterface *>( "Texture", mPinOutputBuffer, PID_OPENGL_TEXTURE, PIN_OUTPUT_BUFFER );

	mNode->pairPins( mPinInputArray, mPinOutputBuffer );
}

bool ArrayToTextureBufferNode::initialise()
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

void ArrayToTextureBufferNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	if( !pTimeStamp )
	{
		return;
	}

	fugio::Performance	Perf( mNode, "inputsUpdated", pTimeStamp );

	if( !OpenGLPlugin::hasContextStatic() )
	{
		return;
	}

	for( fugio::NodeInterface::UuidPair UP : mNode->pairedPins() )
	{
		QSharedPointer<fugio::PinInterface>  PinI = mNode->findPinByLocalId( UP.first );
		QSharedPointer<fugio::PinInterface>  PinO = mNode->findPinByLocalId( UP.second );
		fugio::OpenGLTextureInterface		*BufO = qobject_cast<fugio::OpenGLTextureInterface *>( PinO && PinO->hasControl() ? PinO->control()->qobject() : nullptr );

		if( !PinI || !PinO || !BufO )
		{
			continue;
		}

		if( !PinI->isUpdated( pTimeStamp ) )
		{
			continue;
		}

		fugio::ArrayInterface			*A;

		if( ( A = input<fugio::ArrayInterface *>( PinI ) ) == nullptr )
		{
			continue;
		}

		if( A->type() != QMetaType::QVector4D )
		{
			continue;
		}

		if( BufO->size().x() != A->count() )
		{
			BufO->free();
		}

		if( A->count() <= 0 )
		{
			continue;
		}

		BufO->setTarget( QOpenGLTexture::TargetBuffer );
		BufO->update();

		GLuint		TBO = mBuffers.value( UP.second, 0 );

		if( !TBO )
		{
			glGenBuffers( 1, &TBO );

			mBuffers.insert( UP.second, TBO );
		}

		glBindBuffer( GL_TEXTURE_BUFFER, TBO );
		glBufferData( GL_TEXTURE_BUFFER, A->count() * A->stride(), A->array(), GL_STATIC_DRAW );
		glBindBuffer( GL_TEXTURE_BUFFER, 0 );

		BufO->dstTex()->create();

		glBindTexture( GL_TEXTURE_BUFFER, BufO->dstTexId() );
		glTexBuffer( GL_TEXTURE_BUFFER, GL_RGBA32F, TBO );

		BufO->release();

		OPENGL_DEBUG( mNode->name() );

		pinUpdated( PinO );
	}
}

QUuid ArrayToTextureBufferNode::pairedPinControlUuid( QSharedPointer<fugio::PinInterface> pPin ) const
{
	return( pPin->direction() == PIN_INPUT ? PID_OPENGL_TEXTURE : QUuid() );
}

QList<QUuid> ArrayToTextureBufferNode::pinAddTypesInput() const
{
	static QList<QUuid> PinLst =
	{
		PID_ARRAY
	};

	return( PinLst );
}

bool ArrayToTextureBufferNode::canAcceptPin( PinInterface *pPin ) const
{
	return( pPin->direction() == PIN_OUTPUT );
}
