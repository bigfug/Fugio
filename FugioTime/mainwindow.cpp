#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtEndian>
#include <QNetworkDatagram>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow), mPort( 45454 )
{
	ui->setupUi(this);

	mUniverseTimer.start();

	groupAddress = QHostAddress( "239.255.43.21" );

	timer = new QTimer( this );

	udpSocket = new QUdpSocket( this );

	connect( timer, SIGNAL(timeout()), this, SLOT(sendTime()) );

	connect( udpSocket, SIGNAL(readyRead()), this, SLOT(responseReady()) );
	connect( udpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(sendError(QAbstractSocket::SocketError)) );
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::on_mButton_clicked( bool checked )
{
    if( checked )
	{
		timer->start( 1000 );
	}
	else
	{
		timer->stop();
	}
}

void MainWindow::sendTime()
{
	TimeDatagram		TDG;

	TDG.mServerTimestamp = mUniverseTimer.elapsed();

//	qDebug() << TDG.mServerTimestamp;

	TDG.mServerTimestamp = qToBigEndian<qint64>( TDG.mServerTimestamp );
	TDG.mClientTimestamp = 0;

	udpSocket->writeDatagram( reinterpret_cast<const char *>( &TDG ), sizeof( TDG ), groupAddress, mPort );
}

void MainWindow::responseReady()
{
	TimeDatagram		TDG;

	while( udpSocket->hasPendingDatagrams() )
	{
		QNetworkDatagram	DG = udpSocket->receiveDatagram();

		qDebug() << "RECV" << DG.senderAddress() << DG.senderPort() << DG.data().size();

		if( !DG.isValid() || DG.data().size() != sizeof( TimeDatagram ) )
		{
			continue;
		}

		memcpy( &TDG, DG.data(), sizeof( TDG ) );

		qDebug() << "PING" << DG.senderAddress() << DG.senderPort() << qFromBigEndian<qint64>( TDG.mClientTimestamp );

		TDG.mServerTimestamp = qToBigEndian<qint64>( mUniverseTimer.elapsed() );

		if( udpSocket->writeDatagram( (const char *)&TDG, sizeof( TDG ), DG.senderAddress(), DG.senderPort() ) != sizeof( TimeDatagram ) )
		{
			qWarning() << "Couldn't write packet";
		}
	}
}

void MainWindow::sendError( QAbstractSocket::SocketError pError )
{
	qWarning() << pError;
}
