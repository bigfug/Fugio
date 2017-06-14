#ifndef TIMESYNC_H
#define TIMESYNC_H

#include <QUdpSocket>

class TimeSync : public QObject
{
	Q_OBJECT

public:
	TimeSync( QObject *pParent = nullptr );

private slots:
      void processPendingDatagrams( void );

private:
	QUdpSocket		*mSocket;
};

#endif // TIMESYNC_H
