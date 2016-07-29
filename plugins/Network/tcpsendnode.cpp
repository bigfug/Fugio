#include "tcpsendnode.h"

#include <fugio/core/uuid.h>

#include <fugio/context_interface.h>
#include <fugio/core/array_interface.h>
#include <fugio/core/variant_interface.h>
#include <fugio/serialise_interface.h>
#include <fugio/performance.h>

#include "networkplugin.h"

TCPSendNode::TCPSendNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mWriteTime( 0 )
{
	mPinHost = pinInput( "Host" );
	mPinPort = pinInput( "Port" );

	mPinHost->registerPinInputType( PID_STRING );
	mPinPort->registerPinInputType( PID_INTEGER );

	mPinHost->setValue( "localhost" );
	mPinPort->setValue( 7979 );

	mStream.setDevice( &mSocket );
}

bool TCPSendNode::initialise()
{
	if( !NodeControlBase::initialise() )
	{
		return( false );
	}

	connect( &mSocket, SIGNAL(connected()), this, SLOT(socketConnected()) );

	return( true );
}

bool TCPSendNode::deinitialise()
{
	mSocket.disconnectFromHost();

	return( NodeControlBase::deinitialise() );
}

void TCPSendNode::inputsUpdated( qint64 pTimeStamp )
{
	fugio::Performance	Perf( mNode, "inputsUpdated", pTimeStamp );

	if( mPinHost->isUpdated( pTimeStamp ) || mPinPort->isUpdated( pTimeStamp ) )
	{
		mSocket.disconnectFromHost();

		mSocket.connectToHost( variant( mPinHost ).toString(), variant( mPinPort ).toInt() );

		return;
	}

	if( !mSocket.isOpen() || !mSocket.isWritable() )
	{
		return;
	}

	sendData( pTimeStamp );
}

QList<QUuid> TCPSendNode::pinAddTypesInput() const
{
	return( mNode->context()->global()->pinIds().keys() );
}

bool TCPSendNode::canAcceptPin( fugio::PinInterface *pPin ) const
{
	return( pPin->direction() == PIN_OUTPUT && pPin->hasControl() && qobject_cast<SerialiseInterface *>( pPin->control()->qobject() ) );
}

void TCPSendNode::socketConnected()
{
	sendData( NetworkPlugin::instance()->app()->timestamp() );
}

void TCPSendNode::sendData( qint64 pTimeStamp )
{
	for( QSharedPointer<fugio::PinInterface> P : mNode->enumInputPins() )
	{
		if( P == mPinHost || P == mPinPort )
		{
			continue;
		}

		if( !P->isUpdated( mWriteTime ) )
		{
			continue;
		}

		if( SerialiseInterface *S = input<SerialiseInterface *>( P ) )
		{
			qDebug() << P->name();

			mStream << P->globalId();
			mStream << P->connectedPin()->controlUuid();
			mStream << P->name();

			S->serialise( mStream );
		}
	}

	mWriteTime = pTimeStamp + 1;
}
