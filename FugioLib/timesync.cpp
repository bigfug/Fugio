#include "timesync.h"

#include <QtEndian>
#include <QNetworkDatagram>
#include <QDateTime>

#include "fugio.h"

#include "globalprivate.h"

TimeSync::TimeSync( QObject *pParent )
	: QObject( pParent ), mSocket( nullptr ), mResponseSocket( nullptr ), mServerTimestamp( 0 ), mClientTimestamp( 0 ), mRTT( 0 )
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

	qDebug() << logtime() << "TimeSync port:" << mResponseSocket->localPort();
}

QString TimeSync::logtime()
{
	return( QDateTime::currentDateTimeUtc().toString( "HH:mm:ss.zzz" ) );
}

void TimeSync::processPendingDatagrams()
{
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
			qDebug() << logtime() << "RECV FAIL" << DG.senderAddress() << DG.senderPort() << DG.data().size() << qFromBigEndian<qint64>( TDG.mServerTimestamp ) << qFromBigEndian<qint64>( TDG.mClientTimestamp );

			continue;
		}

		memcpy( &TDG, DG.data(), sizeof( TDG ) );

		qDebug() << logtime() << "RECV" << "RS:" << qFromBigEndian<qint64>( TDG.mServerTimestamp ) << "RC:" << qFromBigEndian<qint64>( TDG.mClientTimestamp );

		t = qFromBigEndian<qint64>( TDG.mServerTimestamp );

		if( qAbs( t - mServerTimestamp ) >= 1000 )
		{
			 ServerAddress = DG.senderAddress();
			 ServerPort    = DG.senderPort();

			 NeedPing = true;
		}
	}

	qDebug() << logtime() << "t =" << t;

	if( t >= 0 )
	{
		if( !mRTTArray.isEmpty() )
		{
			GP->updateUniversalTimestamp( t + qMax( mRTT, mRTTArray[ mRTTArray.size() / 2 ] ) / 2 );
		}

		if( NeedPing && !ServerAddress.isNull() )
		{
			mClientTimestamp = GP->timestamp();

			//mClientTimestamp = mClientTimestamp + 1;

			qDebug() << logtime() << "Sending PING to" << ServerAddress << ServerPort << "T:" << t << "LC:" << mClientTimestamp;

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

		qDebug() << logtime() << "PONG" << DG.senderAddress() << "RS:" << TDG.mServerTimestamp << "RC:" << TDG.mClientTimestamp << "LC:" << mClientTimestamp;

		//if( TDG.mClientTimestamp == mClientTimestamp )
		{
			mRTT = GP->timestamp() - TDG.mClientTimestamp;

			if( mRTTArray.size() >= 7 )
			{
				mRTTArray.removeFirst();
			}

			mRTTArray << mRTT;

			mRTTSortedArray = mRTTArray;

			std::sort( mRTTSortedArray.begin(), mRTTSortedArray.end() );

			qDebug() << logtime() << "RTT:" << mRTT << mRTTSortedArray;
		}
	}
}
