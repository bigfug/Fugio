#include "websocketdataservernode.h"

#include <fugio/core/uuid.h>

#include <fugio/pin_variant_iterator.h>

#include <fugio/context_signals.h>

WebSocketDataServerNode::WebSocketDataServerNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
#if defined( WEBSOCKET_SUPPORTED )
	, mServer( QStringLiteral( "Fugio Server" ), QWebSocketServer::NonSecureMode, this )
#endif
{
	FUGID( PIN_INPUT_PORT, "261cc653-d7fa-4c34-a08b-3603e8ae71d5" )
	FUGID( PIN_INPUT_DATA, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" )
	FUGID( PIN_OUTPUT_DATA, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" )

	mPinInputPort = pinInput( tr( "Port" ), PIN_INPUT_PORT );
	mPinInputData = pinInput( tr( "Input" ), PIN_INPUT_DATA );

	mPinInputPort->setValue( 12345 );

	mValOutputData = pinOutput<fugio::VariantInterface *>( tr( "Output" ), mPinOutputData, PID_BYTEARRAY, PIN_OUTPUT_DATA );

	mValOutputData->setVariantCount( 0 );
}

#if defined( WEBSOCKET_SUPPORTED )

void WebSocketDataServerNode::sendSocketData( QWebSocket *pSocket )
{
	fugio::PinVariantIterator	 VI( mPinInputData );

	for( int i = 0 ; i < VI.count() ; i++ )
	{
		QVariant	V = VI.index( i );

		if( V.type() == QVariant::ByteArray )
		{
			const QByteArray		 A = V.toByteArray();

			if( !A.isEmpty() )
			{
				if( pSocket )
				{
					pSocket->sendBinaryMessage( A );
				}
				else
				{
					for( QWebSocket *Socket : mClients )
					{
						Socket->sendBinaryMessage( A );
					}
				}
			}
		}
		else
		{
			const QString			 S = V.toString();

			if( !S.isEmpty() )
			{
				if( pSocket )
				{
					pSocket->sendTextMessage( S );
				}
				else
				{
					for( QWebSocket *Socket : mClients )
					{
						Socket->sendTextMessage( S );
					}
				}
			}
		}
	}
}
#endif

void WebSocketDataServerNode::inputsUpdated( qint64 pTimeStamp )
{
#if defined( WEBSOCKET_SUPPORTED )
	if( mPinInputData->isUpdated( pTimeStamp ) )
	{
		sendSocketData();
	}
#endif
}

bool WebSocketDataServerNode::initialise()
{
	if( !NodeControlBase::initialise() )
	{
		return( false );
	}

	connect( mNode->context()->qobject(), SIGNAL(frameFinalise()), this, SLOT(frameFinalise()) );

#if defined( WEBSOCKET_SUPPORTED )
	if( mServer.listen( QHostAddress::Any, mPinInputPort->value().toInt() ) )
	{
		connect( &mServer, SIGNAL(newConnection()), this, SLOT(serverNewConnection()) );

		qInfo() << mServer.serverAddress();
	}

	return( true );
#else
	return( false );
#endif
}

bool WebSocketDataServerNode::deinitialise()
{
#if defined( WEBSOCKET_SUPPORTED )
	mServer.close();
#endif

	disconnect( mNode->context()->qobject(), SIGNAL(frameFinalise()), this, SLOT(frameFinalise()) );

	return( NodeControlBase::deinitialise() );
}

void WebSocketDataServerNode::frameFinalise()
{
	QMutexLocker		L( &mInputDataMutex );

	if( !mInputDataList.isEmpty() )
	{
		mValOutputData->setVariantCount( mInputDataList.size() );

		for( int i = 0 ; i < mInputDataList.size() ; i++ )
		{
			mValOutputData->setVariant( i, mInputDataList.at( i ) );
		}

		pinUpdated( mPinOutputData );

		mInputDataList.clear();
	}
}

#if defined( WEBSOCKET_SUPPORTED )

void WebSocketDataServerNode::serverNewConnection()
{
	QWebSocket	*Socket = mServer.nextPendingConnection();

	if( Socket )
	{
		connect( Socket, SIGNAL(textMessageReceived(QString)), this, SLOT(socketTextMessageReceived(QString)) );
		connect( Socket, SIGNAL(binaryMessageReceived(QByteArray)), this, SLOT(socketBinaryMessageReceived(QByteArray)) );
		connect( Socket, SIGNAL(disconnected()), this, SLOT(socketDisconnected()) );

		sendSocketData( Socket );

		mClients << Socket;
	}
}

void WebSocketDataServerNode::socketDisconnected()
{
	QWebSocket	*Socket = qobject_cast<QWebSocket *>( sender() );

	if( Socket )
	{
		Socket->deleteLater();

		mClients.removeAll( Socket );
	}
}

void WebSocketDataServerNode::socketTextMessageReceived(QString pMessage)
{
	QMutexLocker		L( &mInputDataMutex );

	mInputDataList << pMessage;

	nodeUpdate();
}

void WebSocketDataServerNode::socketBinaryMessageReceived( QByteArray pMessage )
{
	QMutexLocker		L( &mInputDataMutex );

	mInputDataList << pMessage;

	nodeUpdate();
}

#endif
