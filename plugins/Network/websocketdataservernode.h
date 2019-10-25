#ifndef WEBSOCKETSENDNODE_H
#define WEBSOCKETSENDNODE_H

#include <QObject>

#if defined( WEBSOCKET_SUPPORTED )
#include <QtWebSockets/QtWebSockets>
#include <QWebSocketServer>
#endif

#include <fugio/nodecontrolbase.h>

class WebSocketDataServerNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "WebSocketSendNode" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Websocket_Data_Server" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE WebSocketDataServerNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~WebSocketDataServerNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

	virtual bool initialise() Q_DECL_OVERRIDE;
	virtual bool deinitialise() Q_DECL_OVERRIDE;

protected slots:
	void frameFinalise( void );

#if defined( WEBSOCKET_SUPPORTED )
//	void socketConnected( void );

//	void sendData( qint64 pTimeStamp );

	void serverNewConnection( void );

	void socketDisconnected( void );
	void socketTextMessageReceived( QString pMessage );
	void socketBinaryMessageReceived( QByteArray pMessage );

protected:
	void sendSocketData( QWebSocket *pSocket = nullptr );
#endif

protected:
	QSharedPointer<fugio::PinInterface>		 mPinInputPort;
	QSharedPointer<fugio::PinInterface>		 mPinInputData;
	QSharedPointer<fugio::PinInterface>		 mPinOutputData;
	fugio::VariantInterface					*mValOutputData;

	QList<QVariant>							 mInputDataList;
	QMutex									 mInputDataMutex;

#if defined( WEBSOCKET_SUPPORTED )
	QWebSocketServer						 mServer;
	QList<QWebSocket *>						 mClients;
#endif
};

#endif // WEBSOCKETSENDNODE_H
