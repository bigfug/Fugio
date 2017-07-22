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

namespace Ui {
class MainWindow;
}

typedef struct TimeDatagram
{
	qint64		mServerTimestamp;
	qint64		mClientTimestamp;
} TimeDatagram;

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

	void responseReady( void );

	void sendError( QAbstractSocket::SocketError pError );

	void networkAccessibility( QNetworkAccessManager::NetworkAccessibility pNA );

	void hostLookup( const QHostInfo &pHost );

private:
	Ui::MainWindow			*ui;
	int						 mPort;
	QUdpSocket				*udpSocket;
	QTimer					*timer;
	QHostAddress			 groupAddress;
	QElapsedTimer			 mUniverseTimer;
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
};

#endif // MAINWINDOW_H
