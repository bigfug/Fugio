#ifndef TIMECAST_H
#define TIMECAST_H

#include <QObject>
#include <QUdpSocket>
#include <QHostAddress>
#include <QDateTime>
#include <QList>

class TimeCast : public QObject
{
	Q_OBJECT

	typedef struct TimeDatagram
	{
		qint64		mServerTimestamp;
		qint64		mClientTimestamp;
	} TimeDatagram;

	static QString logtime( void )
	{
		return( QDateTime::currentDateTimeUtc().toString( "HH:mm:ss.zzz" ) );
	}

public:
	TimeCast( void );

public slots:
	void sendTime( qint64 pTimeStamp );

private slots:
	void updateCasters( void );

private:
	typedef struct InterfaceCaster
	{
		QUdpSocket			*mSocket;
		QHostAddress		 mAddress;
		int					 mPort;

		InterfaceCaster( QObject *pParent )
			: mPort( 45454 )
		{
			mSocket = new QUdpSocket( pParent );
		}

		InterfaceCaster( const InterfaceCaster &pIC )
			: mSocket( pIC.mSocket ), mAddress( pIC.mAddress ), mPort( pIC.mPort )
		{

		}

	} InterfaceCaster;

	QMap<int,InterfaceCaster>		mCasters;
};

#endif // TIMECAST_H
