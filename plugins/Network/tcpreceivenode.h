#ifndef TCPRECEIVENODE_H
#define TCPRECEIVENODE_H

#include <QObject>
#include <QTcpSocket>
#include <QTcpServer>
#include <QDataStream>
#include <QPointer>

#include <fugio/nodecontrolbase.h>

class TCPReceiveNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Receives data across a network via TCP" )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/TCP_Receive" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE TCPReceiveNode( QSharedPointer<fugio::NodeInterface> pNode);

	virtual ~TCPReceiveNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

	virtual bool initialise() Q_DECL_OVERRIDE;

	virtual bool deinitialise() Q_DECL_OVERRIDE;

protected slots:
	void frameStart( qint64 pTimeStamp );

	void serverNewConnection( void );

protected:
	QSharedPointer<fugio::PinInterface>			 mPinPort;

	QTcpServer									 mServer;

	QList<QDataStream *>						 mStreams;
};

#endif // TCPRECEIVENODE_H
