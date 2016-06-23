#include "tcpsendrawnode.h"

#include <fugio/core/uuid.h>

#include <fugio/context_interface.h>
#include <fugio/core/array_interface.h>
#include <fugio/core/variant_interface.h>
#include <fugio/serialise_interface.h>
#include <fugio/performance.h>

#include "networkplugin.h"

TCPSendRawNode::TCPSendRawNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mWriteTime( 0 )
{
	mPinHost = pinInput( "Host" );
	mPinPort = pinInput( "Port" );
	mPinData = pinInput( "Data" );

	mPinHost->registerPinInputType( PID_STRING );
	mPinPort->registerPinInputType( PID_INTEGER );
	mPinData->registerPinInputType( PID_BYTEARRAY );

	mPinHost->setValue( "localhost" );
	mPinPort->setValue( 7878 );

	mStream.setDevice( &mSocket );
}

bool TCPSendRawNode::initialise()
{
	if( !NodeControlBase::initialise() )
	{
		return( false );
	}

	connect( &mSocket, SIGNAL(connected()), this, SLOT(socketConnected()) );

	return( true );
}

bool TCPSendRawNode::deinitialise()
{
	mSocket.disconnectFromHost();

	return( NodeControlBase::deinitialise() );
}

void TCPSendRawNode::inputsUpdated( qint64 pTimeStamp )
{
	fugio::Performance	Perf( mNode, "inputsUpdated", pTimeStamp );

	if( !pTimeStamp || mPinHost->isUpdated( pTimeStamp ) || mPinPort->isUpdated( pTimeStamp ) )
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

void TCPSendRawNode::socketConnected()
{
	sendData( NetworkPlugin::instance()->app()->timestamp() );
}

void TCPSendRawNode::sendData( qint64 pTimeStamp )
{
	if( !mPinData->isUpdated( mWriteTime ) )
	{
		return;
	}

	QByteArray				 A = variant( mPinData ).toByteArray();

	if( A.isEmpty() )
	{
		mStream.writeRawData( A.data(), A.length() );
	}

	mWriteTime = pTimeStamp + 1;
}
