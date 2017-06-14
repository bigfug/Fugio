#include "timesync.h"

#include <QtEndian>

#include "fugio.h"

#include "globalprivate.h"

TimeSync::TimeSync( QObject *pParent )
	: QObject( pParent )
{
	QHostAddress	groupAddress = QHostAddress( "239.255.43.21" );

	mSocket = new QUdpSocket( this );

	mSocket->bind( QHostAddress::AnyIPv4, 45454, QUdpSocket::ShareAddress );

	mSocket->joinMulticastGroup( groupAddress );

	connect( mSocket, SIGNAL(readyRead()), this, SLOT(processPendingDatagrams()) );
}

void TimeSync::processPendingDatagrams()
{
	GlobalPrivate	*GP = qobject_cast<GlobalPrivate *>( fugio::fugio()->qobject() );
	qint64			 t  = -1;

	QByteArray datagram;

	while( mSocket->hasPendingDatagrams() )
	{
		datagram.resize( mSocket->pendingDatagramSize() );

		mSocket->readDatagram( datagram.data(), datagram.size() );

		if( datagram.size() == sizeof( qint64 ) )
		{
			t = qFromBigEndian<qint64>( datagram.constData() );
		}
	}

	if( t >= 0 )
	{
		GP->updateUniversalTimestamp( t );
	}
}
