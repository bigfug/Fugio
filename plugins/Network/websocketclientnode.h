#ifndef WEBSOCKETCLIENTNODE_H
#define WEBSOCKETCLIENTNODE_H

#include <QObject>
#include <QWebSocket>
#include <QHostInfo>
#include <QHostAddress>

#include <fugio/nodecontrolbase.h>

class WebSocketClientNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Sends and receives data over WebSocket" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Websocket_Client" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE WebSocketClientNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~WebSocketClientNode( void ) {}

	// NodeControlInterface interface

	virtual bool deinitialise() Q_DECL_OVERRIDE;

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

protected slots:
	void contextFrame( void );

	void receivedText( const QString &pData );

	void receivedBinary( const QByteArray &pData );

	void connected( void );

	void disconnected( void );

	void error( QAbstractSocket::SocketError pError );

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInputUrl;
	QSharedPointer<fugio::PinInterface>			 mPinInputText;
	QSharedPointer<fugio::PinInterface>			 mPinInputBinary;

	QSharedPointer<fugio::PinInterface>			 mPinOutputText;
	fugio::VariantInterface						*mValOutputText;

	QSharedPointer<fugio::PinInterface>			 mPinOutputBinary;
	fugio::VariantInterface						*mValOutputBinary;

	QUrl										 mUrl;
	QWebSocket									 mSocket;

	QString										 mReceivedText;
	QByteArray									 mReceivedBinary;

	qint64										 mWriteTime;
};

#endif // WEBSOCKETCLIENTNODE_H
