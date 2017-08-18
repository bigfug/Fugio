#include "timesync.h"

#include <QtEndian>
#include <QDateTime>
#include <QTimer>

#include "fugio.h"

#include "globalprivate.h"

TimeSync::TimeSync( QObject *pParent )
	: QObject( pParent ), mSocket( nullptr ), mResponseSocket( nullptr ), mServerTimestamp( 0 ), mClientTimestamp( 0 ), mRTT( 0 )
{
//	QHostAddress	groupAddress = QHostAddress( "226.0.0.1" );

	mSocket = new QUdpSocket( this );

	if( mSocket->bind( QHostAddress::AnyIPv4, 45454, QUdpSocket::ShareAddress ) )
	{
//		mSocket->joinMulticastGroup( groupAddress );

		connect( mSocket, SIGNAL(readyRead()), this, SLOT(processPendingDatagrams()) );
	}
	else
	{
		qWarning() << "Couldn't bind socket";
	}

	mResponseSocket = new QUdpSocket( this );

	if( mResponseSocket->bind() )
	{
		connect( mResponseSocket, SIGNAL(readyRead()), this, SLOT(responseReady()) );
	}
	else
	{
		qWarning() << "Couldn't bind response socket";
	}

//	qDebug() << logtime() << "TimeSync port:" << mResponseSocket->localPort();

	QTimer::singleShot( 1000, this, SLOT(sendPing()) );
}

QString TimeSync::logtime()
{
	return( QDateTime::currentDateTimeUtc().toString( "HH:mm:ss.zzz" ) );
}

void TimeSync::processPendingDatagrams()
{
	TimeDatagram	 TDG;
	QByteArray		 DatagramBuffer;
	QHostAddress	 ServerAddress;

	while( mSocket->hasPendingDatagrams() )
	{
		DatagramBuffer.resize( mResponseSocket->pendingDatagramSize() );

		if( DatagramBuffer.size() != mResponseSocket->pendingDatagramSize() )
		{
			break;
		}

		mResponseSocket->readDatagram( DatagramBuffer.data(), DatagramBuffer.size(), &ServerAddress );

		if( DatagramBuffer.size() != sizeof( TDG ) )
		{
			continue;
		}

		memcpy( &TDG, DatagramBuffer.data(), sizeof( TDG ) );

		mServerAddress = ServerAddress;
		mServerPort    = 45456;
	}
}

void TimeSync::responseReady()
{
	GlobalPrivate	*GP = qobject_cast<GlobalPrivate *>( fugio::fugio()->qobject() );

	TimeDatagram	 TDG;
	QByteArray		 DatagramBuffer;

	while( mResponseSocket->hasPendingDatagrams() )
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

		memcpy( &TDG, DatagramBuffer.data(), sizeof( TDG ) );

		TDG.mServerTimestamp = qFromBigEndian<qint64>( TDG.mServerTimestamp );
		TDG.mClientTimestamp = qFromBigEndian<qint64>( TDG.mClientTimestamp );

//		qDebug() << logtime() << "PONG" << DG.senderAddress() << "RS:" << TDG.mServerTimestamp << "RC:" << TDG.mClientTimestamp << "LC:" << mClientTimestamp;

		if( TDG.mClientTimestamp == mClientTimestamp )
		{
			mRTT = GP->timestamp() - TDG.mClientTimestamp;

			if( mRTTArray.size() >= 7 )
			{
				mRTTArray.removeFirst();
			}

			mRTTArray << mRTT;

			mRTTSortedArray = mRTTArray;

			std::sort( mRTTSortedArray.begin(), mRTTSortedArray.end() );

			GP->updateUniversalTimestamp( TDG.mServerTimestamp + ( mRTT / 2 ) ); //( qMax( mRTT, mRTTArray[ mRTTArray.size() / 2 ] ) / 2 ) );

//			qDebug() << logtime() << "RTT:" << mRTT << mRTTSortedArray << GP->universalTimestamp();
		}
	}
}

void TimeSync::sendPing()
{
	GlobalPrivate	*GP = qobject_cast<GlobalPrivate *>( fugio::fugio()->qobject() );

	if( !mServerAddress.isNull() )
	{
		mClientTimestamp = GP->timestamp();
		mServerTimestamp = GP->universalTimestamp();

//		qDebug() << logtime() << "Sending PING to" << mServerAddress << mServerPort << "T:" << 0 << "LC:" << mClientTimestamp;

		TimeDatagram	 TDG;

		TDG.mServerTimestamp = qToBigEndian<qint64>( mServerTimestamp );
		TDG.mClientTimestamp = qToBigEndian<qint64>( mClientTimestamp );

		if( mResponseSocket->writeDatagram( (const char *)&TDG, sizeof( TDG ), mServerAddress, mServerPort ) == sizeof( TDG ) )
		{
//			mServerTimestamp = 0;
		}
	}

	QTimer::singleShot( qMax( 2500LL, 5000LL - ( QDateTime::currentMSecsSinceEpoch() % 5000LL ) ), this, SLOT(sendPing()) );
}

void TimeSync::setServer( const QString &pServer, int pPort )
{
	mServerAddress = pServer;
	mServerPort    = pPort;
}
