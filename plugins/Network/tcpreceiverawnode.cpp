#include "tcpreceiverawnode.h"

#include <QtEndian>

#include <fugio/core/uuid.h>
#include <fugio/context_signals.h>
#include <fugio/performance.h>
#include <fugio/serialise_interface.h>

#include <fugio/context_interface.h>
#include <fugio/core/array_interface.h>
#include <fugio/core/variant_interface.h>

#include "networkplugin.h"

TCPReceiveRawNode::TCPReceiveRawNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mStream( nullptr )
{
	FUGID( PIN_OUTPUT_BUFFER,		"70E5469F-A27B-4A54-BDE0-C2547A8EA269" );
	FUGID( PIN_OUTPUT_CONNECTED,	"DF4D11E8-5761-4D3F-B7F9-AF81731D402F" );

	mPinPort = pinInput( "Port" );

	mPinPort->registerPinInputType( PID_INTEGER );

	mPinPort->setValue( 7878 );

	mValOutputBuffer = pinOutput<fugio::VariantInterface *>( "Data", mPinOutputBuffer, PID_BYTEARRAY, PIN_OUTPUT_BUFFER );

	mValOutputConnected = pinOutput<fugio::VariantInterface *>( "Connected", mPinOutputConnected, PID_BOOL, PIN_OUTPUT_CONNECTED );
}

bool TCPReceiveRawNode::initialise()
{
	if( !NodeControlBase::initialise() )
	{
		return( false );
	}

	connect( &mServer, SIGNAL(newConnection()), this, SLOT(serverNewConnection()) );
	connect( &mServer, SIGNAL(acceptError(QAbstractSocket::SocketError)), this, SLOT(serverAcceptError(QAbstractSocket::SocketError)) );

	mNode->setStatus( fugio::NodeInterface::Initialising );
	mNode->setStatusMessage( "Waiting for connection" );

	return( true );
}

bool TCPReceiveRawNode::deinitialise()
{
	return( NodeControlBase::deinitialise() );
}

void TCPReceiveRawNode::frameStart( qint64 pTimeStamp )
{
	QTcpSocket		*S = qobject_cast<QTcpSocket *>( mStream->device() );

	if( S )
	{
		if( !S->isOpen() )
		{
			delete mStream;
			delete S;

			mStream = nullptr;

			if( mNode->status() != fugio::NodeInterface::Warning )
			{
				mNode->setStatus( fugio::NodeInterface::Warning );
				mNode->setStatusMessage( "Socket not open" );
			}
		}
		else
		{
			if( mNode->status() != fugio::NodeInterface::Initialised )
			{
				mNode->setStatus( fugio::NodeInterface::Initialised );
				mNode->setStatusMessage( "Connected" );
			}

			if( S->bytesAvailable() )
			{
				fugio::Performance	Perf( mNode, "frameStart", pTimeStamp );

				mValOutputBuffer->setVariant( S->readAll() );

				pinUpdated( mPinOutputBuffer );
			}
		}
	}

	disconnect( mNode->context()->qobject(), SIGNAL(frameStart(qint64)), this, SLOT(frameStart(qint64)) );
}

void TCPReceiveRawNode::serverNewConnection()
{
	QTcpSocket		*S = mServer.nextPendingConnection();

	if( S )
	{
		if( mStream )
		{
			delete mStream;

			mStream = nullptr;
		}

		mStream = new QDataStream( S );

		if( mStream )
		{
			connect( S,SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketError(QAbstractSocket::SocketError)) );
			connect( S, SIGNAL(readyRead()), this, SLOT(socketReadyRead()) );
		}
	}
}

void TCPReceiveRawNode::serverAcceptError( QAbstractSocket::SocketError pError )
{
	Q_UNUSED( pError )

	mNode->setStatus( fugio::NodeInterface::Error );
	mNode->setStatusMessage( mServer.errorString() );
}

void TCPReceiveRawNode::socketReadyRead()
{
	connect( mNode->context()->qobject(), SIGNAL(frameStart(qint64)), this, SLOT(frameStart(qint64)) );
}

void TCPReceiveRawNode::socketError( QAbstractSocket::SocketError pError )
{
	Q_UNUSED( pError )

	qDebug() << pError << mServer.errorString();

	mNode->setStatus( fugio::NodeInterface::Error );
	mNode->setStatusMessage( mServer.errorString() );
}

void TCPReceiveRawNode::inputsUpdated( qint64 pTimeStamp )
{
	if( mPinPort->isUpdated( pTimeStamp ) )
	{
		int			NewPort = variant( mPinPort ).toInt();

		if( mServer.isListening() && mServer.serverPort() != NewPort )
		{
			mServer.close();
		}

		if( !mServer.isListening() )
		{
			mServer.listen( QHostAddress::AnyIPv4, NewPort );
		}
	}

	bool		Connected = false;

	if( mStream )
	{
		QTcpSocket		*S = qobject_cast<QTcpSocket *>( mStream->device() );

		if( S && S->isOpen() )
		{
			Connected = true;
		}
	}

	if( Connected != mValOutputConnected->variant().toBool() )
	{
		mValOutputConnected->setVariant( Connected );

		pinUpdated( mPinOutputConnected );
	}
}
