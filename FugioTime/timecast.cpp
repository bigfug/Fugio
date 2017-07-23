#include "timecast.h"

#include <QTimer>
#include <QDebug>
#include <QtEndian>

TimeCast::TimeCast( void )
	: mPort( 45454 )
{
	mGroupAddress = QHostAddress( "239.255.43.21" );

	mSocket = new QUdpSocket( this );

	mSocket->setSocketOption( QAbstractSocket::MulticastTtlOption, 5 );
}

void TimeCast::sendTime( qint64 pTimeStamp )
{
	TimeDatagram		TDG;

	TDG.mServerTimestamp = qToBigEndian<qint64>( pTimeStamp );
	TDG.mClientTimestamp = 0;

	if( mSocket->writeDatagram( reinterpret_cast<const char *>( &TDG ), sizeof( TDG ), mGroupAddress, mPort ) != sizeof( TDG ) )
	{
		qWarning() << "FAIL" << pTimeStamp;
	}
	else
	{
//		qDebug() << logtime() << "SENT" << pTimeStamp;
	}
}
