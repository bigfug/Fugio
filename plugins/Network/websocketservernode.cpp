#include "websocketservernode.h"

#include <QJsonDocument>

#include <fugio/context_signals.h>

#include <fugio/core/uuid.h>

WebSocketServerNode::WebSocketServerNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mLastSend( -1 )
#if defined( WEBSOCKET_SUPPORTED )
	, mServer( QStringLiteral( "Fugio Server" ), QWebSocketServer::NonSecureMode, this )
#endif
{
	FUGID( PIN_INPUT_VALUE, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_OUTPUT_VALUE, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );

	QSharedPointer<fugio::PinInterface>			 mPinInput;
	QSharedPointer<fugio::PinInterface>			 mPinOutput;

	mPinInput = pinInput( tr( "Input" ), PIN_INPUT_VALUE );

	mPinOutput = pinOutput( "Output", PIN_OUTPUT_VALUE );

	mPinInput->setAutoRename( true );

	mNode->pairPins( mPinInput, mPinOutput );

	connect( mNode->qobject(), SIGNAL(pinLinked(QSharedPointer<fugio::PinInterface>,QSharedPointer<fugio::PinInterface>)), this, SLOT(pinLinked(QSharedPointer<fugio::PinInterface>,QSharedPointer<fugio::PinInterface>)) );

	connect( mNode->qobject(), SIGNAL(pinUnlinked(QSharedPointer<fugio::PinInterface>,QSharedPointer<fugio::PinInterface>)), this, SLOT(pinUnlinked(QSharedPointer<fugio::PinInterface>,QSharedPointer<fugio::PinInterface>)) );
}

bool WebSocketServerNode::initialise()
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

	connect( mNode->context()->qobject(), SIGNAL(frameStart()), this, SLOT(receiveUpdates()) );
	connect( mNode->context()->qobject(), SIGNAL(frameEnd(qint64)), this, SLOT(sendUpdates(qint64)) );

	return( true );
#else
	return( false );
#endif
}

bool WebSocketServerNode::deinitialise()
{
#if defined( WEBSOCKET_SUPPORTED )
	disconnect( mNode->context()->qobject(), SIGNAL(frameStart()), this, SLOT(receiveUpdates()) );
	disconnect( mNode->context()->qobject(), SIGNAL(frameEnd(qint64)), this, SLOT(sendUpdates(qint64)) );

	mServer.close();
#endif

	return( NodeControlBase::deinitialise() );
}

#if defined( WEBSOCKET_SUPPORTED )

void WebSocketServerNode::serverNewConnection()
{
	QWebSocket	*Socket = mServer.nextPendingConnection();

	if( Socket )
	{
		connect( Socket, SIGNAL(textMessageReceived(QString)), this, SLOT(socketTextMessageReceived(QString)) );
		connect( Socket, SIGNAL(binaryMessageReceived(QByteArray)), this, SLOT(socketBinaryMessageReceived(QByteArray)) );
		connect( Socket, SIGNAL(disconnected()), this, SLOT(socketDisconnected()) );

		mClients << Socket;
	}
}

void WebSocketServerNode::socketDisconnected()
{
	QWebSocket	*Socket = qobject_cast<QWebSocket *>( sender() );

	if( Socket )
	{
		Socket->deleteLater();

		mClients.removeAll( Socket );
	}
}

void WebSocketServerNode::socketTextMessageReceived(QString pMessage)
{
	QWebSocket	*Socket = qobject_cast<QWebSocket *>( sender() );

	if( !Socket )
	{
		return;
	}

	QJsonDocument	JD = QJsonDocument::fromJson( pMessage.toLatin1() );

	if( !JD.isArray() )
	{
		return;
	}

	for( QJsonValue JV : JD.array() )
	{
		if( !JV.isObject() )
		{
			continue;
		}

		QJsonObject	JO = JV.toObject();

		QString		PN = JO.value( "name" ).toString();
		QString		PT = JO.value( "type" ).toString();

		QJsonArray	JA = JO.value( "values" ).toArray();

		QSharedPointer<fugio::PinInterface>		PI = mNode->findOutputPinByName( PN );

		if( PI && PI->hasControl() )
		{
			fugio::VariantInterface	*V = qobject_cast<fugio::VariantInterface *>( PI->control()->qobject() );

			if( V )
			{
				V->setVariantType( QMetaType::Type( QVariant::nameToType( PT.toLatin1().constData() ) ) );

				V->setVariantCount( JA.size() );

				for( int i = 0 ; i < JA.size() ; i++ )
				{
					V->setVariant( i, JA.at( i ).toVariant() );
				}

				pinUpdated( PI );
			}
		}
	}
}

void WebSocketServerNode::socketBinaryMessageReceived(QByteArray pMessage)
{
	QWebSocket	*Socket = qobject_cast<QWebSocket *>( sender() );

	if( Socket )
	{
		qDebug() << Socket << pMessage << "(binary)";
	}
}

void WebSocketServerNode::sendUpdates( qint64 pTimeStamp )
{
	QJsonDocument	JD;
	QJsonArray		JA;

	for( fugio::NodeInterface::UuidPair UP : mNode->pairedPins() )
	{
		QSharedPointer<fugio::PinInterface> SrcPin = mNode->findPinByLocalId( UP.first );

		if( !SrcPin || !SrcPin->isUpdated( pTimeStamp ) )
		{
			continue;
		}

		fugio::VariantInterface		*V = input<fugio::VariantInterface *>( SrcPin );

		QJsonObject	JO;

		JO[ "name" ]  = SrcPin->name();
		JO[ "type" ]  = QVariant::typeToName( V->variantType() );

		QJsonArray		JV;

		for( int i = 0 ; i < V->variantCount() ; i++ )
		{
			JV.append( QJsonValue::fromVariant( V->variant( i ) ) );
		}

		JO[ "values" ] = JV;

		JA.append( JO );
	}

	if( !JA.isEmpty() )
	{
		JD.setArray( JA );

		QByteArray	JS = JD.toJson();

		qDebug() << JS;

		for( QWebSocket *S : mClients )
		{
			S->sendTextMessage( JS );
		}
	}

	mLastSend = pTimeStamp;
}
#endif

void WebSocketServerNode::inputsUpdated( qint64 pTimeStamp )
{
	for( fugio::NodeInterface::UuidPair UP : mNode->pairedPins() )
	{
		QSharedPointer<fugio::PinInterface>  SrcPin = mNode->findPinByLocalId( UP.first );
		QSharedPointer<fugio::PinInterface>  DstPin = mNode->findPinByLocalId( UP.second );

		if( SrcPin->isUpdated( pTimeStamp ) )
		{
			pinUpdated( DstPin );
		}
	}
}

QList<QUuid> WebSocketServerNode::pinAddTypesInput() const
{
	static QList<QUuid>	PinLst =
	{
		PID_BOOL,
		PID_INTEGER,
		PID_FLOAT,
		PID_STRING,
		PID_VARIANT
	};

	return( PinLst );
}

bool WebSocketServerNode::canAcceptPin(fugio::PinInterface *pPin) const
{
	return( pPin->direction() == PIN_OUTPUT );
}

bool WebSocketServerNode::pinShouldAutoRename( fugio::PinInterface *pPin ) const
{
	Q_UNUSED( pPin )

	return( true );
}

QUuid WebSocketServerNode::pairedPinControlUuid( QSharedPointer<fugio::PinInterface> pPin ) const
{
	return( QUuid() );
}

void WebSocketServerNode::pinLinked( QSharedPointer<fugio::PinInterface> P1, QSharedPointer<fugio::PinInterface> P2 )
{
	if( P1->direction() != PIN_OUTPUT )
	{
		std::swap( P1, P2 );
	}

	if( P1->node() == mNode )
	{
		return;
	}

//	qDebug() << "link" << P1->node()->name() << ":" << P1->name() << "->" << P2->node()->name() << ":" << P2->name();

	QSharedPointer<fugio::PinControlInterface>	PCI = P1->control();

	for( fugio::NodeInterface::UuidPair UP : mNode->pairedPins() )
	{
		if( UP.first != P2->localId() )
		{
			continue;
		}

		QSharedPointer<fugio::PinInterface>  DstPin = mNode->findPinByLocalId( UP.second );

		if( !DstPin )
		{
			continue;
		}

		DstPin->setControl( PCI );
	}
}

void WebSocketServerNode::pinUnlinked(QSharedPointer<fugio::PinInterface> P1, QSharedPointer<fugio::PinInterface> P2)
{
	if( P1->direction() != PIN_OUTPUT )
	{
		std::swap( P1, P2 );
	}

	if( P1->node() == mNode )
	{
		return;
	}

//	qDebug() << "unlink" << P1->node()->name() << ":" << P1->name() << "->" << P2->node()->name() << ":" << P2->name();

	for( fugio::NodeInterface::UuidPair UP : mNode->pairedPins() )
	{
		if( UP.first != P2->localId() )
		{
			continue;
		}

		QSharedPointer<fugio::PinInterface>  DstPin = mNode->findPinByLocalId( UP.second );

		if( !DstPin )
		{
			continue;
		}

		DstPin->setControl( QSharedPointer<fugio::PinControlInterface>() );
	}
}
