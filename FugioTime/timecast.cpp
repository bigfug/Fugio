#include "timecast.h"

#include <QTimer>
#include <QDebug>
#include <QtEndian>
#include <QNetworkInterface>

TimeCast::TimeCast( void )
{
	QTimer::singleShot( 100, this, SLOT(updateCasters()) );
}

void TimeCast::sendTime( qint64 pTimeStamp )
{
	TimeDatagram		TDG;

	TDG.mServerTimestamp = qToBigEndian<qint64>( pTimeStamp );
	TDG.mClientTimestamp = 0;

	for( InterfaceCaster &IC : mCasters.values() )
	{
		if( IC.mSocket->writeDatagram( reinterpret_cast<const char *>( &TDG ), sizeof( TDG ), IC.mAddress, IC.mPort ) != sizeof( TDG ) )
		{
			qWarning() << "FAIL" << pTimeStamp;
		}
		else
		{
	//		qDebug() << logtime() << "SENT" << pTimeStamp;
		}
	}
}

void TimeCast::updateCasters()
{
	for( QNetworkInterface I : QNetworkInterface::allInterfaces() )
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

			qDebug() << "Adding TimeCast:" << I.name() << I.humanReadableName() << E.broadcast().toString() << I.hardwareAddress();

			InterfaceCaster		IC( this );

			IC.mAddress = E.broadcast();

			mCasters.insert( I.index(), IC );
		}
	}

	QTimer::singleShot( 60000, this, SLOT(updateCasters()) );
}
