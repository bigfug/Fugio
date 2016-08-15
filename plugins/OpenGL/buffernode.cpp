#include "buffernode.h"

#include <fugio/core/uuid.h>
#include <fugio/opengl/uuid.h>
#include <fugio/node_signals.h>

#include "bufferentryproxypin.h"

BufferNode::BufferNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mEntrySize( 0 )
{
	FUGID( PIN_INPUT_BUFFER,	"D334BDBD-DFC8-4A67-A08B-1A9BDD790E10" );
	FUGID( PIN_INPUT_ENTRY,		"DD2265A4-72A7-4684-974B-5B8807958C7A" );
	FUGID( PIN_OUTPUT_BUFFER,	"A8B5A71C-6761-4143-941A-C99DA4475BE2" );
	FUGID( PIN_OUTPUT_SIZE,		"BDE144CF-D9A4-482C-B901-5D571D775B11" );

	mPinInputBuffer = pinInput( "Buffer", PIN_INPUT_BUFFER );

	mPinInputEntry = pinInput( "Entry", PIN_INPUT_ENTRY );

	mValOutputSize = pinOutput<fugio::VariantInterface *>( "Size", mPinOutputSize, PID_INTEGER, PIN_OUTPUT_SIZE );

	mValOutputEntry = pinOutput<BufferEntryProxyPin *>( "Entry", mPinOutputEntry, PID_OPENGL_BUFFER_ENTRY_PROXY, PIN_OUTPUT_BUFFER );

	mNode->pairPins( mPinInputEntry, mPinOutputEntry );

	mValOutputEntry->setBufferInput( mPinInputBuffer );

	mValOutputEntry->setBufferEntryInput( mPinInputEntry );
}

void BufferNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	GLuint			EntSiz = 0;

	for( fugio::NodeInterface::UuidPair UP : mNode->pairedPins() )
	{
		QSharedPointer<fugio::PinInterface>		PinI, PinO;

		PinI = mNode->findPinByLocalId( UP.first );
		PinO = mNode->findPinByLocalId( UP.second );

		if( !PinI || !PinO || !PinO->hasControl() )
		{
			continue;
		}

		fugio::OpenGLBufferEntryInterface	*EntI = input<fugio::OpenGLBufferEntryInterface *>( PinI );
		BufferEntryProxyPin					*EntO = qobject_cast<BufferEntryProxyPin *>( PinO->control()->qobject() );

		EntO->setBufferInput( mPinInputBuffer );

		EntO->setBufferEntryInput( PinI );

		EntO->setBufferOffset( EntSiz );

		if( EntI )
		{
			EntSiz += EntI->entrySize();
		}
	}

	if( mEntrySize != EntSiz )
	{
		mEntrySize = EntSiz;

		for( BufferEntryProxyPin *P : enumOutputs<BufferEntryProxyPin *>() )
		{
			P->setBufferStride( EntSiz );
		}

		mValOutputSize->setVariant( mEntrySize );

		pinUpdated( mPinOutputSize );
	}

	pinUpdated( mPinOutputEntry );
}

QList<QUuid> BufferNode::pinAddTypesInput() const
{
	static QList<QUuid>		PinLst =
	{
		PID_OPENGL_BUFFER_ENTRY
	};

	return( PinLst );
}

bool BufferNode::canAcceptPin(fugio::PinInterface *pPin) const
{
	return( pPin->direction() == PIN_OUTPUT );
}

QUuid BufferNode::pairedPinControlUuid( QSharedPointer<fugio::PinInterface> pPin ) const
{
	if( pPin->direction() == PIN_INPUT )
	{
		return( PID_OPENGL_BUFFER_ENTRY_PROXY );
	}

	return( QUuid() );
}
