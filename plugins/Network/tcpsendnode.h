#ifndef TCPSENDNODE_H
#define TCPSENDNODE_H

#include <QObject>
#include <QTcpSocket>
#include <QDataStream>

#include <fugio/nodecontrolbase.h>

class TCPSendNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Sends data across a network via TCP" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "TCP_Send" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE TCPSendNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~TCPSendNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

	virtual QList<QUuid> pinAddTypesInput() const Q_DECL_OVERRIDE;

	virtual bool canAcceptPin( fugio::PinInterface *pPin ) const Q_DECL_OVERRIDE;

	virtual bool initialise() Q_DECL_OVERRIDE;
	virtual bool deinitialise() Q_DECL_OVERRIDE;

protected slots:
	void hostFound( void );

	void socketError( QAbstractSocket::SocketError pSocketError );

	void socketConnected( void );

	void sendData( qint64 pTimeStamp );

	void socketConnect( void );

protected:
	QSharedPointer<fugio::PinInterface>			 mPinHost;
	QSharedPointer<fugio::PinInterface>			 mPinPort;

	QTcpSocket									 mSocket;
	QDataStream									 mStream;

	qint64										 mWriteTime;
};

#endif // TCPSENDNODE_H
