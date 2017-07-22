#include "timesync.h"

#include <QtEndian>
#include <QNetworkDatagram>

#include "fugio.h"

#include "globalprivate.h"

TimeSync::TimeSync( QObject *pParent )
	: QObject( pParent ), mSocket( nullptr ), mResponseSocket( nullptr ), mServerTimestamp( 0 ), mRTT( 0 )
{
	QHostAddress	groupAddress = QHostAddress( "239.255.43.21" );

	mSocket = new QUdpSocket( this );

	if( mSocket->bind( QHostAddress::AnyIPv4, 45454, QUdpSocket::ShareAddress ) )
	{
		mSocket->joinMulticastGroup( groupAddress );

		connect( mSocket, SIGNAL(readyRead()), this, SLOT(processPendingDatagrams()) );
	}

	mResponseSocket = new QUdpSocket( this );

	if( mResponseSocket->bind() )
	{
		connect( mSocket, SIGNAL(readyRead()), this, SLOT(responseReady()) );
	}

	qDebug() << "TimeSync port:" << mResponseSocket->localPort();
}

void TimeSync::processPendingDatagrams()
{
//	qDebug() << "TimeSync::processPendingDatagrams()";

	GlobalPrivate	*GP = qobject_cast<GlobalPrivate *>( fugio::fugio()->qobject() );
	qint64			 t  = -1;
	QHostAddress	 ServerAddress;
	quint16			 ServerPort;
	bool			 NeedPing = false;

	TimeDatagram	 TDG;

	while( mSocket->hasPendingDatagrams() )
	{
		QNetworkDatagram	DG = mSocket->receiveDatagram();

		if( !DG.isValid() || DG.data().size() != sizeof( TDG ) )
		{
			continue;
		}

		memcpy( &TDG, DG.data(), sizeof( TDG ) );

//		qDebug() << DG.senderAddress() << DG.senderPort() << DG.data().size() << qFromBigEndian<qint64>( TDG.mServerTimestamp ) << qFromBigEndian<qint64>( TDG.mClientTimestamp );

		t = qFromBigEndian<qint64>( TDG.mServerTimestamp );

		if( qAbs( t - mServerTimestamp ) >= 1000 )
		{
			 ServerAddress = DG.senderAddress();
			 ServerPort    = DG.senderPort();

			 NeedPing = true;
		}
	}

	qDebug() << "t =" << t;

	if( t >= 0 )
	{
		GP->updateUniversalTimestamp( t + mRTT );

		if( NeedPing && !ServerAddress.isNull() )
		{
			mClientTimestamp = GP->timestamp();

//			qDebug() << "Sending PING to" << ServerAddress << ServerPort << t << mClientTimestamp;

			TDG.mServerTimestamp = qToBigEndian<qint64>( t );
			TDG.mClientTimestamp = qToBigEndian<qint64>( mClientTimestamp );

			if( mResponseSocket->writeDatagram( (const char *)&TDG, sizeof( TDG ), ServerAddress, ServerPort ) == sizeof( TDG ) )
			{
				mServerTimestamp = t;
			}
		}
	}
}

void TimeSync::responseReady()
{
	GlobalPrivate	*GP = qobject_cast<GlobalPrivate *>( fugio::fugio()->qobject() );

//	qDebug() << "TimeSync::responseReady()";

	TimeDatagram	 TDG;

	while( mResponseSocket->hasPendingDatagrams() )
	{
		QNetworkDatagram	DG = mResponseSocket->receiveDatagram();

		if( !DG.isValid() || DG.data().size() != sizeof( TDG ) )
		{
			continue;
		}

		memcpy( &TDG, DG.data(), sizeof( TDG ) );

		TDG.mServerTimestamp = qFromBigEndian<qint64>( TDG.mServerTimestamp );
		TDG.mClientTimestamp = qFromBigEndian<qint64>( TDG.mClientTimestamp );

		qDebug() << "PONG" << DG.senderAddress() << DG.data().size() << TDG.mServerTimestamp << TDG.mClientTimestamp << mClientTimestamp;

		if( TDG.mClientTimestamp == mClientTimestamp )
		{
			mRTT = GP->timestamp() - TDG.mClientTimestamp;

			qDebug() << "RTT:" << mRTT;
		}
	}
}
