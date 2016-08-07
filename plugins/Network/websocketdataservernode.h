#ifndef WEBSOCKETSENDNODE_H
#define WEBSOCKETSENDNODE_H

#include <QObject>

#include <QtWebSockets/QtWebSockets>
#include <QWebSocketServer>
#include <fugio/nodecontrolbase.h>

class WebSocketDataServerNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "WebSocketSendNode" )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/WebSocket_Send" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE WebSocketDataServerNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~WebSocketDataServerNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

	virtual bool initialise() Q_DECL_OVERRIDE;
	virtual bool deinitialise() Q_DECL_OVERRIDE;

protected slots:
//	void socketConnected( void );

//	void sendData( qint64 pTimeStamp );

	void serverNewConnection( void );

	void socketDisconnected( void );
	void socketTextMessageReceived( QString pMessage );
	void socketBinaryMessageReceived( QByteArray pMessage );

protected:
	void sendSocketData( QWebSocket *pSocket = nullptr );

protected:
	QSharedPointer<fugio::PinInterface>		 mPinInputData;
	QWebSocketServer						 mServer;
	QList<QWebSocket *>						 mClients;
};

#endif // WEBSOCKETSENDNODE_H
