#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtEndian>
#include <QNetworkDatagram>
#include <QHostInfo>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	mNetworkStatusLabel = new QLabel( "Initialising" );

	ui->mStatusBar->addPermanentWidget( mNetworkStatusLabel );

	connect( &mNAM, &QNetworkAccessManager::networkAccessibleChanged, this, &MainWindow::networkAccessibility );

	connect( &mTimeServer, &TimeServer::clientResponse, this, &MainWindow::clientUpdate );

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
	qint64				TS = mTimeServer.timestamp();

	if( mNAM.networkAccessible() != QNetworkAccessManager::Accessible )
	{
		QPalette Palette;

		Palette.setColor( QPalette::Window,     Qt::red );
		Palette.setColor( QPalette::WindowText, Qt::white );

		mNetworkStatusLabel->setPalette( Palette );
		mNetworkStatusLabel->setAutoFillBackground( true );
		mNetworkStatusLabel->setText( "No Network" );
	}
	else if( ui->mButton->isChecked() )
	{
		mNetworkStatusLabel->setText( QString( "%1" ).arg( TS ) );

		mTimeCast.sendTime( TS );

		QPalette Palette;

		Palette.setColor( QPalette::Window,     Qt::darkGreen );
		Palette.setColor( QPalette::WindowText, Qt::white );

		mNetworkStatusLabel->setPalette( Palette );
		mNetworkStatusLabel->setAutoFillBackground( true );
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

	for( SocketEntry &SE : mSocketEntries )
	{
		if( SE.mListItem && SE.mListItem->text() != SE.mName )
		{
			SE.mListItem->setText( SE.mName );
		}

		if( TS - SE.mTimestamp >= 10000 )
		{
			if( SE.mListItem )
			{
				ui->mClientList->removeItemWidget( SE.mListItem );

				delete SE.mListItem;

				SE.mListItem = 0;
			}
		}
		else if( !SE.mListItem )
		{
			SE.mListItem = new QListWidgetItem( SE.mName );

			ui->mClientList->addItem( SE.mListItem );
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

	qint64	t = QDateTime::currentMSecsSinceEpoch();

	QTimer::singleShot( qMax( 500LL, 1000LL - ( t % 1000LL ) ), this, SLOT(sendTime()) );
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

void MainWindow::clientUpdate( const QHostAddress &pAddr, int pPort, qint64 pTimestamp )
{
	// Record the client entry

	pTimestamp = mTimeServer.timestamp();

	bool			SocketEntryFound = false;

	for( SocketEntry &SE : mSocketEntries )
	{
		if( SE.mAddress == pAddr && SE.mPort == pPort )
		{
			SE.mTimestamp = pTimestamp;

			SocketEntryFound = true;

			break;
		}
	}

	if( !SocketEntryFound )
	{
		SocketEntry		SE;

		SE.mAddress   = pAddr;
		SE.mPort      = pPort;
		SE.mTimestamp = pTimestamp;
		SE.mListItem  = 0;
		SE.mName      = QString( "%1:%2" ).arg( SE.mAddress.toString() ).arg( SE.mPort );
		SE.mLookupId  = QHostInfo::lookupHost( pAddr.toString(), this, SLOT(hostLookup(QHostInfo)) );

		mSocketEntries << SE;
	}
}
