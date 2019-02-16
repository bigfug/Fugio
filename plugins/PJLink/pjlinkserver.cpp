#include "pjlinkserver.h"

#include <QTimer>
#include <QCryptographicHash>

PJLinkServer::PJLinkServer( QObject *pParent )
	: QObject( pParent )
{
	mSocketIP4 = new QUdpSocket( this );

	connect( mSocketIP4, &QUdpSocket::readyRead, this, &PJLinkServer::readReady );

	if( mSocketIP4->bind( QHostAddress( QHostAddress::AnyIPv4 ), 4352, QUdpSocket::ShareAddress ) )
	{
		QTimer::singleShot( 1000, this, &PJLinkServer::searchTimeout );
	}
	else
	{
		qWarning() << "PJLink: Can't bind to port 4352 on IP4";
	}
}

PJLinkServer::~PJLinkServer()
{

}

void PJLinkServer::searchTimeout( void )
{
	QByteArray		Datagram( "%2SRCH\r" );

	mSocketIP4->writeDatagram( Datagram, QHostAddress::Broadcast, 4352 );

	QTimer::singleShot( 30000, this, &PJLinkServer::searchTimeout );
}

void PJLinkServer::readReady()
{
	QByteArray		Datagram;
	QHostAddress	Host;
	quint16			Port;

	while( mSocketIP4->hasPendingDatagrams() )
	{
		int			DatagramSize = int( mSocketIP4->pendingDatagramSize() );

		Datagram.resize( DatagramSize );

		if( !Datagram.size() || Datagram.size() != DatagramSize )
		{
			continue;
		}

		if( mSocketIP4->readDatagram( Datagram.data(), Datagram.size(), &Host, &Port ) != Datagram.size() )
		{
			continue;
		}

		qDebug() << "PJLINK:" << QString::fromLatin1( Datagram );

		PJLinkReponse	Response( Datagram );

		if( Response.resposeCode() != PJLinkReponse::OK )
		{
			continue;
		}

		PJLinkClient	*Client = client( Host );

		if( !Client )
		{
			continue;
		}

		if( Response.command() == "ACKN" )
		{
			Client->connectToClient();
		}
		else if( Response.command() == "LKUP" )
		{
		}
		else if( Response.command() == "ERST" )
		{
			// 0 = no warning, 1 = warning, 2 = error

			int	Fan			= Response.response().at( 0 ) - '0';		// 0-2
			int	Lamp		= Response.response().at( 1 ) - '0';		// 0-2
			int	Temperature	= Response.response().at( 2 ) - '0';		// 0-2
			int	CoverOpen	= Response.response().at( 3 ) - '0';		// 0-2
			int	Filter		= Response.response().at( 4 ) - '0';		// 0-2
			int	Other		= Response.response().at( 5 ) - '0';		// 0-2
		}
		else if( Response.command() == "POWR" )
		{
			// 0 = power off, 1 = power on

			int	Power		= Response.response().at( 0 ) - '0';		// 0-3

			Client->setPower( Power );
		}
		else if( Response.command() == "INPT" )
		{
			int	Input		= Response.response().at( 0 );		// 11～6Z
		}
	}
}

PJLinkClient *PJLinkServer::client( QHostAddress pAddress )
{
	for( PJLinkClient *C : mClientList )
	{
		if( C->address() == pAddress )
		{
			return( C );
		}
	}

	PJLinkClient	*C = new PJLinkClient( pAddress );

	mClientList << C;

	return( C );
}

PJLinkClient::PJLinkClient( QHostAddress pAddress )
	: mAddress( pAddress ), mAuthenticated( false ), mReady( false ), mClass( 2 )
{
	connect( &mSocket, &QTcpSocket::readyRead, this, &PJLinkClient::readyRead );
	connect( &mSocket, QOverload<QAbstractSocket::SocketError>::of( &QAbstractSocket::error ), this, &PJLinkClient::socketError );
}

void PJLinkClient::powerOn()
{
	sendCommand( QByteArray( "%1POWR 1\r" ) );
}

void PJLinkClient::powerOff()
{
	sendCommand( QByteArray( "%1POWR 0\r" ) );
}

void PJLinkClient::sendCommand( QByteArray pCommand )
{
	mCommands << pCommand;

	if( mSocket.state() == QAbstractSocket::UnconnectedState )
	{
		QTimer::singleShot( 1, this, &PJLinkClient::connectToClient );

		return;
	}

	sendCommand();
}

void PJLinkClient::sendCommand()
{
	if( mAuthenticated && mReady )
	{
		if( !mCommands.isEmpty() )
		{
			if( mDigest.isEmpty() )
			{
				mSocket.write( mCommands.first() );
			}
			else
			{
				QByteArray	Command = mDigest;

				Command.append( mCommands.first() );

				mSocket.write( Command );
			}

			qDebug() << mCommands.first();

			mReady = false;
		}
	}
}

void PJLinkClient::socketError( QAbstractSocket::SocketError pSocketError )
{
	switch( pSocketError )
	{
		case QAbstractSocket::RemoteHostClosedError:
			mAuthenticated = mReady = false;
			break;

		default:
			break;
	}
}

void PJLinkClient::readyRead()
{
	QByteArray	Data;

	while( !( Data = mSocket.readLine() ).isEmpty() )
	{
		qDebug() << "PJLINK CLIENT:" << QString::fromLatin1( Data );

		if( Data.startsWith( "PJLINK" ) )
		{
			QByteArrayList	Command = Data.split( ' ' );

			if( Command.size() > 1 && Command[ 1 ] == "0" )
			{
				mAuthenticated = mReady = true;
			}
			else if( Command.size() > 2 && Command[ 1 ] == "1" )
			{
				QByteArray	RandomSequence = Command[ 2 ].left( Command[ 2 ].size() - 1 );
				QByteArray	Password( "JBMIAProjectorLink" );

				QByteArray	HashData;

				HashData.append( RandomSequence );
				HashData.append( Password );

				mDigest = QCryptographicHash::hash( HashData, QCryptographicHash::Md5 ).toHex();

				mAuthenticated = mReady = true;
			}

			if( mAuthenticated && mReady )
			{
				if( mClass == 1 )
				{

				}

				if( mClass >= 1 )
				{
					sendCommand( QByteArray( "%1NAME ?\r" ) );
					sendCommand( QByteArray( "%1AVMT ?\r" ) );
					sendCommand( QByteArray( "%1ERST ?\r" ) );
					sendCommand( QByteArray( "%1LAMP ?\r" ) );
					sendCommand( QByteArray( "%1INF1 ?\r" ) );
					sendCommand( QByteArray( "%1INF2 ?\r" ) );
					sendCommand( QByteArray( "%1INFO ?\r" ) );
					sendCommand( QByteArray( "%1CLSS ?\r" ) );
					sendCommand( QByteArray( "%1POWR ?\r" ) );
				}

				if( mClass >= 2 )
				{
					sendCommand( QByteArray( "%2INPT ?\r" ) );
					sendCommand( QByteArray( "%2SNUM ?\r" ) );
					sendCommand( QByteArray( "%2SVER ?\r" ) );
					sendCommand( QByteArray( "%2INNM ?\r" ) );
					sendCommand( QByteArray( "%2IRES ?\r" ) );
					sendCommand( QByteArray( "%2RRES ?\r" ) );
					sendCommand( QByteArray( "%2FILT ?\r" ) );
					sendCommand( QByteArray( "%2RLMP ?\r" ) );
					sendCommand( QByteArray( "%2RFIL ?\r" ) );
					sendCommand( QByteArray( "%2FREZ ?\r" ) );
					sendCommand( QByteArray( "%2INST ?\r" ) );
				}

				sendCommand();
			}
		}
		else if( Data.startsWith( '%' ) && Data.endsWith( '\r' ) )
		{
			parseResponse( Data );

			if( !mCommands.isEmpty() )
			{
				mCommands.removeFirst();
			}

			mReady = true;

			sendCommand();
		}
	}
}

void PJLinkClient::connectToClient()
{
	if( mSocket.state() == QAbstractSocket::UnconnectedState )
	{
		mSocket.connectToHost( mAddress, 4352 );
	}
}

void PJLinkClient::parseResponse( QByteArray pResponse )
{
	PJLinkReponse	Response( pResponse );

	if( Response.command() == "POWR" )
	{
		if( Response.response().toUpper() == "OK" )
		{
			mPower.update();
		}
		else if( Response.resposeCode() == PJLinkReponse::OK )
		{
			mPower.set( Response.response().toInt() );
		}
	}
	else if( Response.command() == "NAME" )
	{
		if( Response.resposeCode() == PJLinkReponse::OK )
		{
			mName.set( Response.response() );
		}
	}
}
