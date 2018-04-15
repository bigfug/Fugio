#ifndef WEBSOCKETSERVERNODE_H
#define WEBSOCKETSERVERNODE_H

#include <QObject>

#if defined( WEBSOCKET_SUPPORTED )
#include <QtWebSockets/QtWebSockets>
#include <QWebSocketServer>
#endif

#include <fugio/nodecontrolbase.h>

class WebSocketServerNode : public fugio::NodeControlBase, public fugio::PairedPinsHelperInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::PairedPinsHelperInterface )

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "WebSocketServerNode" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "WebSocket_Server" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE WebSocketServerNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~WebSocketServerNode( void ) {}

	// NodeControlInterface interface

	virtual bool initialise() Q_DECL_OVERRIDE;
	virtual bool deinitialise() Q_DECL_OVERRIDE;

	virtual QList<QUuid> pinAddTypesInput() const Q_DECL_OVERRIDE;

	virtual bool canAcceptPin( fugio::PinInterface *pPin ) const Q_DECL_OVERRIDE;

	virtual bool pinShouldAutoRename(fugio::PinInterface *pPin) const Q_DECL_OVERRIDE;

	// PairedPinsHelperInterface interface
public:
	virtual QUuid pairedPinControlUuid( QSharedPointer<fugio::PinInterface> pPin ) const Q_DECL_OVERRIDE;

protected slots:
#if defined( WEBSOCKET_SUPPORTED )
	void serverNewConnection( void );

	void socketDisconnected( void );
	void socketTextMessageReceived( QString pMessage );
	void socketBinaryMessageReceived( QByteArray pMessage );

	void sendUpdates( qint64 pTimeStamp );
#endif

	void pinLinked( QSharedPointer<fugio::PinInterface> P1, QSharedPointer<fugio::PinInterface> P2 );
	void pinUnlinked( QSharedPointer<fugio::PinInterface> P1, QSharedPointer<fugio::PinInterface> P2 );

protected:
	qint64									 mLastSend;

#if defined( WEBSOCKET_SUPPORTED )
	QWebSocketServer						 mServer;
	QList<QWebSocket *>						 mClients;
#endif

	// NodeControlInterface interface
public:
	virtual void inputsUpdated(qint64 pTimeStamp) Q_DECL_OVERRIDE;
};

#endif // WEBSOCKETSERVERNODE_H
