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
	connect( &mSocket, SIGNAL(disconnected()), this, SLOT(socketDisconnected() ) );
	connect( &mSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketError(QAbstractSocket::SocketError)) );
	connect( &mSocket, SIGNAL(hostFound()), this, SLOT(socketHostFound()) );

	return( true );
}

bool TCPSendRawNode::deinitialise()
{
	mSocket.disconnectFromHost();

	return( NodeControlBase::deinitialise() );
}

void TCPSendRawNode::inputsUpdated( qint64 pTimeStamp )
{
	if( mPinHost->isUpdated( pTimeStamp ) || mPinPort->isUpdated( pTimeStamp ) )
	{
		if( mSocket.state() == QAbstractSocket::ConnectedState )
		{
			mSocket.disconnectFromHost();
		}

		if( mSocket.state() != QAbstractSocket::UnconnectedState )
		{
			return;
		}

		mNode->setStatus( fugio::NodeInterface::Initialising );
		mNode->setStatusMessage( "Connecting" );

		mSocket.connectToHost( variant( mPinHost ).toString(), variant( mPinPort ).toInt() );

		return;
	}

	if( mSocket.state() != QAbstractSocket::ConnectedState )
	{
		return;
	}

	fugio::Performance	Perf( mNode, "inputsUpdated", pTimeStamp );

	sendData( pTimeStamp );
}

void TCPSendRawNode::socketConnected()
{
	mNode->setStatus( fugio::NodeInterface::Initialised );
	mNode->setStatusMessage( "Connected" );

	sendData( NetworkPlugin::instance()->app()->timestamp() );
}

void TCPSendRawNode::socketDisconnected()
{
	mNode->setStatus( fugio::NodeInterface::Warning );
	mNode->setStatusMessage( "Disconnected" );
}

void TCPSendRawNode::socketError( QAbstractSocket::SocketError pError )
{
	Q_UNUSED( pError )

	mNode->setStatus( fugio::NodeInterface::Error );
	mNode->setStatusMessage( mSocket.errorString() );
}

void TCPSendRawNode::socketHostFound()
{
	mNode->setStatusMessage( "Host Found" );
}

void TCPSendRawNode::sendData( qint64 pTimeStamp )
{
	if( !mPinData->isUpdated( mWriteTime ) )
	{
		return;
	}

	const QByteArray		 A = variant( mPinData ).toByteArray();

	if( !A.isEmpty() )
	{
		qDebug() << "Sending:" << A.length();

		mStream.writeRawData( A.data(), A.length() );
	}

	mWriteTime = pTimeStamp + 1;
}
