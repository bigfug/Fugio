#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUdpSocket>
#include <QTimer>
#include <QHostAddress>
#include <QElapsedTimer>
#include <QNetworkAccessManager>
#include <QListWidgetItem>
#include <QHostInfo>
#include <QLabel>

#include "timecast.h"
#include "timeserver.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

private:
	static QString logtime( void );

private slots:
	void sendTime( void );

//	void responseReady( void );

	void networkAccessibility( QNetworkAccessManager::NetworkAccessibility pNA );

	void hostLookup( const QHostInfo &pHost );

	void clientUpdate( const QHostAddress &pAddr, int pPort, qint64 pTimestamp );

private:
	Ui::MainWindow			*ui;
	QNetworkAccessManager	 mNAM;
	QLabel					*mNetworkStatusLabel;

	typedef struct SocketEntry
	{
		QHostAddress		 mAddress;
		int					 mPort;
		qint64				 mTimestamp;
		QListWidgetItem		*mListItem;
		QString				 mName;
		int					 mLookupId;
	} SocketEntry;

	QList<SocketEntry>		 mSocketEntries;

	TimeCast				 mTimeCast;
	TimeServer				 mTimeServer;
};

#endif // MAINWINDOW_H
