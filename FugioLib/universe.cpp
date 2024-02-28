#include "universe.h"

#include <QTimer>
#include <QNetworkInterface>
#include <QNetworkAddressEntry>
#include <QDataStream>
#include <QUdpSocket>

Universe::Universe( QObject *pParent )
	: QObject( pParent ), mPort( 45455 )
{
	mSocket = new QUdpSocket( this );

	if( mSocket->bind( mPort, QAbstractSocket::ShareAddress ) )
	{
		connect( mSocket, &QIODevice::readyRead, this, &Universe::readyRead );
	}
	else
	{
		qWarning() << "Couldn't bind socket";
	}

	QTimer::singleShot( 100, this, SLOT(updateCasters()) );
}

qint64 Universe::data( qint64 pTime, const QUuid &pUuid, QString &pName, QUuid &pType, QByteArray &pData ) const
{
	QMutexLocker	 L( &mMutex );
	int				 DataIndex = -1;
	qint64			 DataTime  = -1;

	for( int i = 0 ; i < mData.size() ; i++ )
	{
		const DataCast	&DC = mData[ i ];

		if( DC.mTime > pTime )
		{
			continue;
		}

		if( DC.mUuid != pUuid )
		{
			continue;
		}

		if( DataTime >= 0 && DC.mTime < DataTime )
		{
			continue;
		}

		DataIndex = i;
		DataTime  = DC.mTime;
	}

	if( DataIndex < 0 )
	{
		return( -1 );
	}

	const DataCast	&DC = mData[ DataIndex ];

	pData = DC.mData;
	pName = DC.mName;
	pType = DC.mType;

	return( DataTime );
}

void Universe::addData( qint64 pTime, const QUuid &pUuid, const QString &pName, const QUuid &pType, const QByteArray &pData )
{
	QByteArray		BA;

	if( true )
	{
		QDataStream		DS( &BA, QIODevice::WriteOnly );

		DS << pTime;
		DS << pUuid;
		DS << pName;
		DS << pType;
		DS << pData;
	}

	if( !BA.isEmpty() )
	{
		addPacket( BA );
	}

	DataCast		DC;

	DC.mTime = pTime;
	DC.mUuid = pUuid;
	DC.mName = pName;
	DC.mType = pType;
	DC.mData = pData;

	fugio::GlobalInterface::UniverseEntry	UE;

	UE.mName = pName;
	UE.mType = pType;
	UE.mUuid = pUuid;

	mMutex.lock();

	mData << DC;

	mEntries.insert( pUuid, UE );

	mMutex.unlock();
}

void Universe::clearData( qint64 pTime )
{
	mMutex.lock();

	for( int i = 0 ; i < mData.size() ; )
	{
		if( mData[ i ].mTime > pTime - 500 )
		{
			i++;

			continue;
		}

		mData.removeAt( i );
	}

	mMutex.unlock();
}

void Universe::updateCasters()
{
	for( QNetworkInterface &I : QNetworkInterface::allInterfaces() )
	{
		if( mCasters.contains( I.index() ) )
		{
			continue;
		}

		if( !( I.flags() & QNetworkInterface::CanBroadcast ) )
		{
			continue;
		}

		for( QNetworkAddressEntry E : I.addressEntries() )
		{
			if( E.broadcast().isNull() )
			{
				continue;
			}

			qDebug() << "Adding Universe Interface:" << I.name() << I.humanReadableName() << E.broadcast().toString() << I.hardwareAddress();

			InterfaceCaster		IC( this );

			IC.mAddress = E.broadcast();
			IC.mPort    = mPort;

			mMutex.lock();

			mCasters.insert( I.index(), IC );

			mMutex.unlock();
		}
	}

	QTimer::singleShot( 60000, this, SLOT(updateCasters()) );
}

void Universe::cast( qint64 pTime )
{
	mMutex.lock();

	for( const QByteArray &BA : mPackets )
	{
		for( InterfaceCaster &IC : mCasters.values() )
		{
			if( IC.mSocket->writeDatagram( BA, IC.mAddress, IC.mPort ) != BA.size() )
			{
//				qWarning() << "FAIL" << pTimeStamp;
			}
			else
			{
		//		qDebug() << logtime() << "SENT" << pTimeStamp;
			}
		}
	}

	mPackets.clear();

	mMutex.unlock();
}

void Universe::readyRead()
{
	QByteArray		DatagramBuffer;

	while( mSocket->hasPendingDatagrams() )
	{
		DatagramBuffer.resize( mSocket->pendingDatagramSize() );

		if( DatagramBuffer.size() != mSocket->pendingDatagramSize() )
		{
			break;
		}

		mSocket->readDatagram( DatagramBuffer.data(), DatagramBuffer.size() );

		QDataStream			DS( DatagramBuffer );
		DataCast			DC;

		DS >> DC.mTime;
		DS >> DC.mUuid;
		DS >> DC.mName;
		DS >> DC.mType;
		DS >> DC.mData;

//		qDebug() << DC.mName;

		if( !DC.isValid() )
		{
			continue;
		}

		fugio::GlobalInterface::UniverseEntry	UE;

		UE.mName = DC.mName;
		UE.mType = DC.mType;
		UE.mUuid = DC.mUuid;

		mMutex.lock();

		mData << DC;

		mEntries.insert( DC.mUuid, UE );

		mMutex.unlock();
	}
}

Universe::InterfaceCaster::InterfaceCaster(QObject *pParent)
    : mPort( 45455 )
{
    mSocket = new QUdpSocket( pParent );
}
