#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtEndian>
#include <QNetworkDatagram>
#include <QHostInfo>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow), mPort( 45454 )
{
	ui->setupUi(this);

	mNetworkStatusLabel = new QLabel( "Initialising" );

	ui->mStatusBar->addPermanentWidget( mNetworkStatusLabel );

	connect( &mNAM, &QNetworkAccessManager::networkAccessibleChanged, this, &MainWindow::networkAccessibility );

	mUniverseTimer.start();

	groupAddress = QHostAddress( "239.255.43.21" );

	timer = new QTimer( this );

	udpSocket = new QUdpSocket( this );

	connect( timer, SIGNAL(timeout()), this, SLOT(sendTime()) );

	connect( udpSocket, SIGNAL(readyRead()), this, SLOT(responseReady()) );
	connect( udpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(sendError(QAbstractSocket::SocketError)) );

	QTimer::singleShot( 1000, this, SLOT(sendTime()) );
}

MainWindow::~MainWindow()
{
	delete ui;
}

QString MainWindow::logtime()
{
	return( QDateTime::currentDateTimeUtc().toString( "HH:mm:ss.zzz" ) );
}

void MainWindow::sendTime()
{
	if( ui->mButton->isChecked() )
	{
		if( mNAM.networkAccessible() == QNetworkAccessManager::Accessible )
		{
			TimeDatagram		TDG;

			TDG.mServerTimestamp = mUniverseTimer.elapsed();

			mNetworkStatusLabel->setText( QString( "%1" ).arg( TDG.mServerTimestamp ) );

			qDebug() << logtime() << "SEND" << TDG.mServerTimestamp;

			TDG.mServerTimestamp = qToBigEndian<qint64>( TDG.mServerTimestamp );
			TDG.mClientTimestamp = 0;

			udpSocket->writeDatagram( reinterpret_cast<const char *>( &TDG ), sizeof( TDG ), groupAddress, mPort );

			QPalette Palette;

			Palette.setColor( QPalette::Window,     Qt::darkGreen );
			Palette.setColor( QPalette::WindowText, Qt::white );

			mNetworkStatusLabel->setPalette( Palette );
			mNetworkStatusLabel->setAutoFillBackground( true );
		}
		else
		{
			QPalette Palette;

			Palette.setColor( QPalette::Window,     Qt::red );
			Palette.setColor( QPalette::WindowText, Qt::white );

			mNetworkStatusLabel->setPalette( Palette );
			mNetworkStatusLabel->setAutoFillBackground( true );
			mNetworkStatusLabel->setText( "No Network" );
		}
	}
	else
	{
		QPalette Palette;

		Palette.setColor( QPalette::Window,     Qt::gray );
		Palette.setColor( QPalette::WindowText, Qt::black );

		mNetworkStatusLabel->setPalette( Palette );
		mNetworkStatusLabel->setAutoFillBackground( true );
		mNetworkStatusLabel->setText( "Not Enabled" );
	}

	quint64				TS = mUniverseTimer.elapsed();

	for( SocketEntry &SE : mSocketEntries )
	{
		if( SE.mListItem && SE.mListItem->text() != SE.mName )
		{
			SE.mListItem->setText( SE.mName );
		}

		if( SE.mListItem )
		{
			if( TS - SE.mTimestamp >= 10000 )
			{
				ui->mClientList->removeItemWidget( SE.mListItem );

				delete SE.mListItem;

				SE.mListItem = 0;
			}
			else if( TS - SE.mTimestamp >= 5000 )
			{
				SE.mListItem->setForeground( Qt::gray );
			}
			else if( SE.mListItem )
			{
				SE.mListItem->setForeground( Qt::black );
			}
		}
		else
		{
			SE.mListItem = new QListWidgetItem( SE.mName );

			ui->mClientList->addItem( SE.mListItem );
		}
	}

	qint64	t = QDateTime::currentMSecsSinceEpoch();

	QTimer::singleShot( qMax( 500LL, 1000LL - ( t % 1000LL ) ) + 5000, this, SLOT(sendTime()) );
}

void MainWindow::responseReady()
{
	TimeDatagram		TDG;
	quint64				TS = mUniverseTimer.elapsed();

	while( udpSocket->hasPendingDatagrams() )
	{
		QNetworkDatagram	DG = udpSocket->receiveDatagram();

		if( !DG.isValid() || DG.data().size() != sizeof( TimeDatagram ) )
		{
			qDebug() << logtime() << "RECV" << DG.senderAddress() << DG.senderPort() << DG.isValid() << DG.data().size();

			continue;
		}

		memcpy( &TDG, DG.data(), sizeof( TDG ) );

		QString			HostName;

		// Record the client entry

		bool			SocketEntryFound = false;

		for( SocketEntry &SE : mSocketEntries )
		{
			if( SE.mAddress == DG.senderAddress() && SE.mPort == DG.senderPort() )
			{
				SE.mTimestamp = TS;

				HostName = SE.mName;

				SocketEntryFound = true;

				break;
			}
		}

		if( !SocketEntryFound )
		{
			SocketEntry		SE;

			SE.mAddress   = DG.senderAddress();
			SE.mPort      = DG.senderPort();
			SE.mTimestamp = TS;
			SE.mListItem  = 0;
			SE.mName      = QString( "%1:%2" ).arg( SE.mAddress.toString() ).arg( SE.mPort );
			SE.mLookupId  = QHostInfo::lookupHost( DG.senderAddress().toString(), this, SLOT(hostLookup(QHostInfo)) );

			mSocketEntries << SE;

			HostName = SE.mName;
		}

		qDebug() << logtime() << "PING" << HostName << qFromBigEndian<qint64>( TDG.mClientTimestamp );

		// Send the response packet

		TDG.mServerTimestamp = qToBigEndian<qint64>( mUniverseTimer.elapsed() );

		if( udpSocket->writeDatagram( (const char *)&TDG, sizeof( TDG ), DG.senderAddress(), DG.senderPort() ) != sizeof( TimeDatagram ) )
		{
			qWarning() << logtime() << "Couldn't write packet";
		}
	}

	udpSocket->flush();
}

void MainWindow::sendError( QAbstractSocket::SocketError pError )
{
	qWarning() << logtime() << "sendError" << pError;
}

void MainWindow::networkAccessibility( QNetworkAccessManager::NetworkAccessibility pNA )
{
	qDebug() << logtime() << "networkAccessibility" << pNA;
}

void MainWindow::hostLookup( const QHostInfo &pHost )
{
	for( SocketEntry &SE : mSocketEntries )
	{
		if( SE.mLookupId != pHost.lookupId() )
		{
			continue;
		}

		if( pHost.error() != QHostInfo::NoError )
		{
			qDebug() << logtime() << "Couldn't lookup:" << SE.mAddress.toString();

			continue;
		}

		qDebug() << logtime() << "Resolved:" << SE.mAddress.toString() << "to" << pHost.hostName();

		SE.mName = QString( "%1:%2" ).arg( pHost.hostName() ).arg( SE.mPort );

		break;
	}
}
