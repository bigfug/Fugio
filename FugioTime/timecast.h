#ifndef TIMECAST_H
#define TIMECAST_H

#include <QObject>
#include <QUdpSocket>
#include <QHostAddress>
#include <QDateTime>

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

private:
	QUdpSocket			*mSocket;
	QHostAddress		 mGroupAddress;
	int					 mPort;
};

#endif // TIMECAST_H
