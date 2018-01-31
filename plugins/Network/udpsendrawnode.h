#ifndef UDPSENDRAWNODE_H
#define UDPSENDRAWNODE_H

#include <QObject>
#include <QUdpSocket>
#include <QHostInfo>
#include <QHostAddress>

#include <fugio/nodecontrolbase.h>

class UDPSendRawNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Sends raw data across a network via UDP" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "UDP_Send_Raw" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE UDPSendRawNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~UDPSendRawNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

protected slots:
	void hostLookup( QHostInfo pHostInfo );

protected:
	QSharedPointer<fugio::PinInterface>			 mPinHost;
	QSharedPointer<fugio::PinInterface>			 mPinPort;
	QSharedPointer<fugio::PinInterface>			 mPinData;

	QUdpSocket									 mSocket;
	QHostAddress								 mAddress;

	qint64										 mWriteTime;
};

#endif // UDPSENDRAWNODE_H
