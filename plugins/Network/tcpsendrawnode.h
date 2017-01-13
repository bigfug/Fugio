#ifndef TCPSENDRAWNODE_H
#define TCPSENDRAWNODE_H

#include <QObject>
#include <QTcpSocket>
#include <QDataStream>

#include <fugio/nodecontrolbase.h>

class TCPSendRawNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Sends raw data across a network via TCP" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "TCP_Send_Raw" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE TCPSendRawNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~TCPSendRawNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

	virtual bool initialise() Q_DECL_OVERRIDE;
	virtual bool deinitialise() Q_DECL_OVERRIDE;

protected slots:
	void socketConnected( void );
	void socketDisconnected( void );
	void socketError( QAbstractSocket::SocketError pError );
	void socketHostFound( void );

	void sendData( qint64 pTimeStamp );

protected:
	QSharedPointer<fugio::PinInterface>			 mPinHost;
	QSharedPointer<fugio::PinInterface>			 mPinPort;
	QSharedPointer<fugio::PinInterface>			 mPinData;

	QTcpSocket									 mSocket;
	QDataStream									 mStream;

	qint64										 mWriteTime;
};

#endif // TCPSENDRAWNODE_H
