#include "timeserver.h"

#include <QtEndian>

TimeServer::TimeServer( QObject *pParent )
	: QObject( pParent )
{
	mUniverseTimer.start();

	mSocket = new QUdpSocket( this );

	connect( mSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketError(QAbstractSocket::SocketError)) );

	if( mSocket->bind( 45456 ) )
	{
		connect( mSocket, SIGNAL(readyRead()), this, SLOT(responseReady()) );
	}
	else
	{
		 qWarning() << "Couldn't bind socket";
	}

//	qDebug() << "TimeSync Port:" << mSocket->localPort();
}

void TimeServer::socketError( QAbstractSocket::SocketError pError )
{
	qWarning() << logtime() << "sendError" << pError << mSocket->errorString();
}

void TimeServer::responseReady( void )
{
	TimeDatagram	TDG;
	QByteArray		DatagramBuffer;

	while( mSocket->hasPendingDatagrams() )
	{
		DatagramBuffer.resize( mResponseSocket->pendingDatagramSize() );

		if( DatagramBuffer.size() != mResponseSocket->pendingDatagramSize() )
		{
			break;
		}

		mResponseSocket->readDatagram( DatagramBuffer.data(), DatagramBuffer.size() );

		if( DatagramBuffer.size() != sizeof( TDG ) )
		{
			continue;
		}

		memcpy( &TDG, DatagramBuffer.constData(), sizeof( TDG ) );

		qint64		ServerTimestamp = mUniverseTimer.elapsed();
		qint64		RTT             = ServerTimestamp - qFromBigEndian<qint64>( TDG.mServerTimestamp );

//		qDebug() << logtime() << "Received PING from" << DG.senderAddress().toString() << "RC:" << qFromBigEndian<qint64>( TDG.mClientTimestamp ) << "ST:" << ServerTimestamp;

		// Send the response packet

		TDG.mServerTimestamp = qToBigEndian<qint64>( ServerTimestamp );

//		qDebug() << logtime() << "PONG" << DG.senderAddress() << DG.senderPort();

		if( mSocket->writeDatagram( (const char *)&TDG, sizeof( TDG ), DG.senderAddress(), DG.senderPort() ) != sizeof( TimeDatagram ) )
		{
			qWarning() << logtime() << "Couldn't write packet";
		}

		emit clientResponse( DG.senderAddress(), DG.senderPort(), ServerTimestamp, RTT );
	}
}
