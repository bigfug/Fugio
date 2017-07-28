#ifndef TIMESERVER_H
#define TIMESERVER_H

#include <QObject>
#include <QUdpSocket>
#include <QElapsedTimer>
#include <QDateTime>

class TimeServer : public QObject
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
	explicit TimeServer( QObject *pParent = nullptr );

	inline qint64 timestamp( void ) const
	{
		return( mUniverseTimer.elapsed() );
	}

signals:
	void clientResponse( const QHostAddress &pAddr, int pPort, qint64 pTimestamp, qint64 pRTT );

private slots:
	void socketError( QAbstractSocket::SocketError pError );

	void responseReady( void );

private:
	QUdpSocket				*mSocket;
	QElapsedTimer			 mUniverseTimer;
};

#endif // TIMESERVER_H
