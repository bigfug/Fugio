#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtEndian>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	mUniverseTimer.start();

	groupAddress = QHostAddress( "239.255.43.21" );

	timer = new QTimer(this);

	udpSocket = new QUdpSocket(this);

	connect( timer, SIGNAL(timeout()), this, SLOT(sendTime()) );
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
	const qint64 t = qToBigEndian<qint64>( mUniverseTimer.elapsed() );

	udpSocket->writeDatagram( reinterpret_cast<const char *>( &t ), sizeof( t ), groupAddress, 45454 );
}
