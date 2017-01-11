#include "websocketclientnode.h"

#include <fugio/core/uuid.h>

#include <fugio/context_interface.h>
#include <fugio/core/array_interface.h>
#include <fugio/core/variant_interface.h>
#include <fugio/serialise_interface.h>
#include <fugio/performance.h>
#include <fugio/context_signals.h>

#include "networkplugin.h"

WebSocketClientNode::WebSocketClientNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mWriteTime( 0 )
{
	FUGID( PIN_INPUT_URL, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_INPUT_TEXT, "261cc653-d7fa-4c34-a08b-3603e8ae71d5" );
	FUGID( PIN_INPUT_BINARY, "249f2932-f483-422f-b811-ab679f006381" );

	FUGID( PIN_OUTPUT_TEXT, "ce8d578e-c5a4-422f-b3c4-a1bdf40facdb" );
	FUGID( PIN_OUTPUT_BINARY, "e6bf944e-5f46-4994-bd51-13c2aa6415b7" );

	mPinInputUrl = pinInput( "URL", PIN_INPUT_URL );
	mPinInputText = pinInput( "Text", PIN_INPUT_TEXT );
	mPinInputBinary = pinInput( "Binary", PIN_INPUT_BINARY );

	mPinInputUrl->registerPinInputType( PID_STRING );
	mPinInputText->registerPinInputType( PID_STRING );
	mPinInputBinary->registerPinInputType( PID_BYTEARRAY );

	mValOutputText = pinOutput<fugio::VariantInterface *>( "Text", mPinOutputText, PID_STRING, PIN_OUTPUT_TEXT );

	mValOutputBinary = pinOutput<fugio::VariantInterface *>( "Binary", mPinOutputBinary, PID_BYTEARRAY, PIN_OUTPUT_BINARY );

	connect( &mSocket, &QWebSocket::connected, this, &WebSocketClientNode::connected );

	connect( &mSocket, &QWebSocket::disconnected, this, &WebSocketClientNode::disconnected );

	connect( &mSocket, &QWebSocket::binaryMessageReceived, this, &WebSocketClientNode::receivedBinary );

	connect( &mSocket, &QWebSocket::textMessageReceived, this, &WebSocketClientNode::receivedText );
}

void WebSocketClientNode::inputsUpdated( qint64 pTimeStamp )
{
	fugio::Performance	Perf( mNode, "inputsUpdated", pTimeStamp );

	if( mPinInputUrl->isUpdated( pTimeStamp ) )
	{
		QString		UrlSrc = variant( mPinInputUrl ).toString();
		QUrl		Url( UrlSrc );

		if( Url.isValid() )
		{
			mNode->setStatus( fugio::NodeInterface::Initialising );
			mNode->setStatusMessage( "Connecting" );

			mSocket.open( Url );
		}
	}

	if( !mSocket.isValid() )
	{
		return;
	}

	if( mPinInputBinary->isUpdated( mWriteTime ) )
	{
		const QByteArray	&Message = variant( mPinInputBinary ).toByteArray();

		if( !Message.isEmpty() )
		{
			mSocket.sendBinaryMessage( Message );
		}
	}

	if( mPinInputText->isUpdated( mWriteTime ) )
	{
		const QString		&Message = variant( mPinInputText ).toString();

		if( !Message.isEmpty() )
		{
			mSocket.sendTextMessage( Message );
		}
	}

	mWriteTime = pTimeStamp + 1;
}

void WebSocketClientNode::contextFrame()
{
	if( !mReceivedBinary.isEmpty() )
	{
		mValOutputBinary->setVariant( mReceivedBinary );

		pinUpdated( mPinOutputBinary );

		mReceivedBinary.clear();
	}

	if( !mReceivedText.isEmpty() )
	{
		mValOutputText->setVariant( mReceivedText );

		pinUpdated( mPinOutputText );

		mReceivedText.clear();
	}

	disconnect( mNode->context()->qobject(), SIGNAL(frameInitialise()), this, SLOT(contextFrame()) );
}

void WebSocketClientNode::receivedText(const QString &pData)
{
	mReceivedText = pData;

	connect( mNode->context()->qobject(), SIGNAL(frameInitialise()), this, SLOT(contextFrame()) );
}

void WebSocketClientNode::receivedBinary( const QByteArray &pData )
{
	mReceivedBinary = pData;

	connect( mNode->context()->qobject(), SIGNAL(frameInitialise()), this, SLOT(contextFrame()) );
}

void WebSocketClientNode::connected()
{
	mNode->setStatus( fugio::NodeInterface::Initialised );
	mNode->setStatusMessage( "Connected" );

	mNode->context()->updateNode( node() );
}

void WebSocketClientNode::disconnected()
{
	mNode->setStatus( fugio::NodeInterface::Initialising );
	mNode->setStatusMessage( "Disconnected" );
}

void WebSocketClientNode::error( QAbstractSocket::SocketError pError )
{
	mNode->setStatus( fugio::NodeInterface::Error );
	mNode->setStatusMessage( mSocket.errorString() );
}

bool WebSocketClientNode::deinitialise()
{
	mSocket.close();

	return( NodeControlBase::deinitialise() );
}
