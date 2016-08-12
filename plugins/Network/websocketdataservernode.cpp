#include "websocketdataservernode.h"

WebSocketDataServerNode::WebSocketDataServerNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
#if defined( WEBSOCKET_SUPPORTED )
	, mServer( QStringLiteral( "Fugio Server" ), QWebSocketServer::NonSecureMode, this )
#endif
{
	mPinInputData = pinInput( "Data" );
}

#if defined( WEBSOCKET_SUPPORTED )

void WebSocketDataServerNode::sendSocketData( QWebSocket *pSocket )
{
	fugio::VariantInterface		*V = input<fugio::VariantInterface *>( mPinInputData );

	if( V && V->variant().type() == QVariant::ByteArray )
	{
		const QByteArray		 A = V->variant().toByteArray();

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
		const QString			 S = variant( mPinInputData ).toString();

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

#if defined( WEBSOCKET_SUPPORTED )
	if( mServer.listen( QHostAddress::Any, 12345 ) )
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

	return( NodeControlBase::deinitialise() );
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
	QWebSocket	*Socket = qobject_cast<QWebSocket *>( sender() );

	if( Socket )
	{
		qDebug() << Socket << pMessage << "(text)";
	}
}

void WebSocketDataServerNode::socketBinaryMessageReceived(QByteArray pMessage)
{
	QWebSocket	*Socket = qobject_cast<QWebSocket *>( sender() );

	if( Socket )
	{
		qDebug() << Socket << pMessage << "(binary)";
	}
}

#endif
