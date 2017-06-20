#ifndef TIMESYNC_H
#define TIMESYNC_H

#include <QUdpSocket>

typedef struct TimeDatagram
{
	qint64		mServerTimestamp;
	qint64		mClientTimestamp;
} TimeDatagram;

class TimeSync : public QObject
{
	Q_OBJECT

public:
	TimeSync( QObject *pParent = nullptr );

private slots:
      void processPendingDatagrams( void );

	  void responseReady( void );

private:
	QUdpSocket		*mSocket;
	QUdpSocket		*mResponseSocket;
	qint64			 mServerTimestamp;
	qint64			 mClientTimestamp;
	qint64			 mRTT;
};

#endif // TIMESYNC_H
