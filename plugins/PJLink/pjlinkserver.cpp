#include "pjlinkserver.h"

#include <QCryptographicHash>

PJLinkServer::PJLinkServer( QObject *pParent )
	: QObject( pParent )
{
	mSocketIP4 = new QUdpSocket( this );

	connect( mSocketIP4, &QUdpSocket::readyRead, this, &PJLinkServer::readReadyIP4 );

	if( !mSocketIP4->bind( QHostAddress( QHostAddress::AnyIPv4 ), 4352, QUdpSocket::ShareAddress ) )
	{
		qWarning() << tr( "PJLink: Can't bind to port 4352 on IP4" );
	}
	else
	{
		QTimer::singleShot( 1000, this, &PJLinkServer::searchTimeout );
	}

	mSocketIP6 = new QUdpSocket( this );

	connect( mSocketIP6, &QUdpSocket::readyRead, this, &PJLinkServer::readReadyIP6 );

	if( !mSocketIP6->bind( QHostAddress( QHostAddress::AnyIPv6 ), 4352, QUdpSocket::ShareAddress ) )
	{
		qWarning() << tr( "PJLink: Can't bind to port 4352 on IP6" );
	}

	mClientQueryTimer = new QTimer( this );

	connect( mClientQueryTimer, &QTimer::timeout, [=]( void )
	{
		emit clientQueryStatus();
	} );

	mClientQueryTimer->start( 5000 );
}

PJLinkServer::~PJLinkServer()
{
	mClientQueryTimer->stop();
}

QVector<QHostAddress> PJLinkServer::clientAddresses() const
{
	QVector<QHostAddress>		ClientAddresses;

	for( PJLinkClient *C : mClientList )
	{
		ClientAddresses << C->address();
	}

	return( ClientAddresses );
}

void PJLinkServer::searchTimeout( void )
{
	QByteArray		Datagram( "%2SRCH\r" );

	if( mSocketIP4->isValid() )
	{
		mSocketIP4->writeDatagram( Datagram, QHostAddress::Broadcast, 4352 );
	}

	QTimer::singleShot( 30000, this, &PJLinkServer::searchTimeout );
}

void PJLinkServer::readReadyIP6()
{
	readSocket( mSocketIP6 );
}

void PJLinkServer::readReadyIP4()
{
	readSocket( mSocketIP4 );
}

void PJLinkServer::readSocket( QUdpSocket *pSocket )
{
	QByteArray		Datagram;
	QHostAddress	Host;
	quint16			Port;

	while( pSocket->hasPendingDatagrams() )
	{
		int			DatagramSize = int( pSocket->pendingDatagramSize() );

		Datagram.resize( DatagramSize );

		if( !Datagram.size() || Datagram.size() != DatagramSize )
		{
			continue;
		}

		if( pSocket->readDatagram( Datagram.data(), Datagram.size(), &Host, &Port ) != Datagram.size() )
		{
			continue;
		}

#if defined( QT_DEBUG )
		qDebug() << "PJLINK:" << QString::fromLatin1( Datagram );
#endif

		PJLinkReponse	Response( Datagram );

		if( Response.resposeCode() != PJLinkReponse::OK )
		{
			continue;
		}

		PJLinkClient	*Client = clientAlloc( Host );

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
			Client->setStatus( Response.response() );
		}
		else if( Response.command() == "POWR" )
		{
			Client->setPower( PJLinkClient::powerFromChar( Response.response().at( 0 ) ) );
		}
		else if( Response.command() == "INPT" )
		{
			Client->setInput( Response.response() );
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

	return( Q_NULLPTR );
}

PJLinkClient *PJLinkServer::clientAlloc( QHostAddress pAddress )
{
	PJLinkClient	*C = client( pAddress );

	if( !C )
	{
		C = new PJLinkClient( pAddress, this );

		if( C )
		{
			connect( this, &PJLinkServer::clientQueryStatus, C, &PJLinkClient::updateStatus );

			connect( C, &PJLinkClient::clientConnected, [=]( void )
			{
				emit clientConnected( C );
			} );

			connect( C, &PJLinkClient::clientUpdated, [=]( void )
			{
				emit clientUpdated( C );
			} );

			connect( C, &PJLinkClient::authenticationError, [=]( void )
			{
				emit clientAuthenticationError( C );
			} );

			qInfo() << tr( "PJLink added projector at" ) << pAddress.toString();

			mClientList << C;

			emit clientListChanged();
		}
	}

	return( C );
}

PJLinkClient::PJLinkClient( QHostAddress pAddress, QObject *pParent )
	: QObject( pParent ), mAddress( pAddress ), mAuthenticated( false ),
	  mReady( false ), mPassword( "JBMIAProjectorLink" ), mInputTerminalNameIndex( 0 )
{
	connect( &mSocket, &QTcpSocket::readyRead, this, &PJLinkClient::readyRead );

#if QT_VERSION >= QT_VERSION_CHECK( 5, 7, 0 )
	connect( &mSocket, QOverload<QAbstractSocket::SocketError>::of( &QAbstractSocket::error ), this, &PJLinkClient::socketError );
#else
	connect( &mSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketError(QAbstractSocket::SocketError)) );
#endif
}

void PJLinkClient::powerOn()
{
	sendCommand( QByteArray( "%1POWR 1\r" ) );
}

void PJLinkClient::powerOff()
{
	sendCommand( QByteArray( "%1POWR 0\r" ) );
}

void PJLinkClient::freeze( bool pFreeze )
{
	sendCommand( QString( "%%2FREZ %1\r" ).arg( pFreeze ? '1' : '0' ).toLocal8Bit() );
}

void PJLinkClient::switchInput( PJLinkClient::Input pInput, char pValue )
{
	sendCommand( QString( "%%2INPT %1%2\r" ).arg( pInput ).arg( pValue ).toLocal8Bit() );
}

void PJLinkClient::updateStatus()
{
	if( mSocket.state() == QAbstractSocket::UnconnectedState )
	{
		connectToClient();
	}

	if( mReady && mCommands.isEmpty() )
	{
		queryStatus();
	}
}

void PJLinkClient::sendCommand( QByteArray pCommand )
{
	if( mSocket.state() == QAbstractSocket::UnconnectedState )
	{
		QTimer::singleShot( 1, this, &PJLinkClient::connectToClient );
	}
	else if( mSocket.state() == QAbstractSocket::ConnectedState )
	{
		mCommands << pCommand;

		sendCommand();
	}
}

void PJLinkClient::sendCommand()
{
	if( mReady && !mCommands.isEmpty() )
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

		//qDebug() << mCommands.first();

		mReady = false;
	}
}

void PJLinkClient::socketError( QAbstractSocket::SocketError pSocketError )
{
#if defined( QT_DEBUG )
	qDebug() << pSocketError;
#endif

	switch( pSocketError )
	{
		case QAbstractSocket::RemoteHostClosedError:
			mAuthenticated = mReady = false;

			mCommands.clear();
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
#if defined( QT_DEBUG )
		qDebug() << "PJLINK CLIENT:" << QString::fromLatin1( Data );
#endif

		if( Data.startsWith( "PJLINK" ) )
		{
			QByteArrayList	Command = Data.split( ' ' );

			if( Command.size() == 2 )
			{
				if( Command[ 1 ] == "0" )
				{
					mReady = true;
				}
				else if( Command[ 1 ] == "ERRA\r" )
				{
					emit authenticationError();

					mSocket.disconnectFromHost();
				}
			}
			else if( Command.size() == 3 && Command[ 1 ] == "1" )
			{
				QByteArray	Salt = Command[ 2 ];
				QByteArray	HashData;

				Salt.chop( 1 );

				HashData.append( Salt );
				HashData.append( mPassword );

				mDigest = QCryptographicHash::hash( HashData, QCryptographicHash::Md5 ).toHex();

				mReady = true;
			}
			else
			{
				qWarning() << tr( "PJLink unrecognised command" ) << Command;
			}

			if( mReady )
			{
				mCommands.clear();

				sendCommand( QByteArray( "%1CLSS ?\r" ) );
			}
		}
		else if( Data.startsWith( '%' ) )
		{
			if( !mAuthenticated )
			{
				mAuthenticated = true;

				emit clientConnected();
			}

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
			if( mPower.update() )
			{
				emit clientUpdated();
			}
		}
		else if( Response.resposeCode() == PJLinkReponse::OK )
		{
			setPower( powerFromChar( Response.response().at( 0 ) ) );
		}
	}
	else if( Response.command() == "NAME" )
	{
		if( Response.resposeCode() == PJLinkReponse::OK )
		{
			setName( Response.response() );
		}
	}
	else if( Response.command() == "INPT" )
	{
		if( Response.response().toUpper() == "OK" )
		{
			if( mInputType.update() )
			{
				emit clientUpdated();
			}

			if( mInput.update() )
			{
				emit clientUpdated();
			}
		}
		else
		{
			setInput( Response.response() );
		}
	}
	else if( Response.command() == "AVMT" )
	{
		if( Response.response().toUpper() == "OK" )
		{
			if( mVideoMute.update() )
			{
				emit clientUpdated();
			}

			if( mAudioMute.update() )
			{
				emit clientUpdated();
			}
		}
		else
		{
			char	Type		= Response.response().at( 0 ) - '0';		// 1-3
			char	Value		= Response.response().at( 1 ) - '0';		// 0-1

			if( Type == 1 || Type == 3 )
			{
				if( mVideoMute.set( Value == '1' ) )
				{
					emit clientUpdated();
				}
			}

			if( Type == 2 || Type == 3 )
			{
				if( mAudioMute.set( Value == '1' ) )
				{
					emit clientUpdated();
				}
			}
		}
	}
	else if( Response.command() == "ERST" )
	{
		if( Response.response().toUpper() == "OK" )
		{
			bool b1 = false;

			b1 |= mFanStatus.update();
			b1 |= mLampStatus.update();
			b1 |= mTemperatureStatus.update();
			b1 |= mCoverOpenStatus.update();
			b1 |= mFilterStatus.update();
			b1 |= mOtherStatus.update();

			if( b1 )
			{
				emit clientUpdated();
			}
		}
		else if( Response.resposeCode() == PJLinkReponse::OK )
		{
			setStatus( Response.response() );
		}
	}
	else if( Response.command() == "LAMP" )
	{
		if( Response.resposeCode() == PJLinkReponse::OK )
		{
			QByteArrayList	LampList = Response.response().split( ' ' );

			mLamps.resize( LampList.size() / 2 );

			for( int i = 0 ; i < mLamps.size() ; i++ )
			{
				PJLinkLamp		Lamp;

				Lamp.mUsageTime = LampList.takeFirst().toInt();
				Lamp.mLampOn    = LampList.takeFirst().toInt();

				mLamps[ i ] = Lamp;
			}
		}
		else if( Response.resposeCode() == PJLinkReponse::ERR1 )
		{
			mLamps.clear();
		}
	}
	else if( Response.command() == "INST" )
	{
		mInputTerminalNameIndex = 0;

		if( Response.resposeCode() == PJLinkReponse::OK )
		{
			QByteArrayList	InstList = Response.response().split( ' ' );

			mInputs.resize( InstList.size() );

			for( int i = 0 ; i < mInputs.size() ; i++ )
			{
				QByteArray		InputData = InstList.takeFirst();

				PJLinkInput		Input;

				Input.mInput = inputFromChar( InputData.at( 0 ) );
				Input.mSource = InputData.at( 1 );

				mInputs[ i ] = Input;

				if( mClass.current() >= 2 )
				{
					QByteArray		QueryCommand( "%2INNM ?" );

					QueryCommand.append( InputData );
					QueryCommand.append( "\r" );

					sendCommand( QueryCommand );
				}
			}
		}
	}
	else if( Response.command() == "INF1" )
	{
		if( Response.resposeCode() == PJLinkReponse::OK )
		{
			if( mManufacturer.set( Response.response() ) )
			{
				emit clientUpdated();
			}
		}
	}
	else if( Response.command() == "INF2" )
	{
		if( Response.resposeCode() == PJLinkReponse::OK )
		{
			if( mProductName.set( Response.response() ) )
			{
				emit clientUpdated();
			}
		}
	}
	else if( Response.command() == "INFO" )
	{
		if( Response.resposeCode() == PJLinkReponse::OK )
		{
			if( mOtherInformation.set( Response.response() ) )
			{
				emit clientUpdated();
			}
		}
	}
	else if( Response.command() == "CLSS" )
	{
		if( Response.resposeCode() == PJLinkReponse::OK )
		{
			if( mClass.set( Response.response().toInt() ) )
			{
				emit clientUpdated();
			}

			queryStatus();
		}
	}
	else if( Response.command() == "SNUM" )
	{
		if( Response.resposeCode() == PJLinkReponse::OK )
		{
			if( mSerialNumber.set( Response.response() ) )
			{
				emit clientUpdated();
			}
		}
	}
	else if( Response.command() == "SVER" )
	{
		if( Response.resposeCode() == PJLinkReponse::OK )
		{
			if( mSoftwareVersion.set( Response.response() ) )
			{
				emit clientUpdated();
			}
		}
	}
	else if( Response.command() == "INNM" )
	{
		if( Response.resposeCode() == PJLinkReponse::OK )
		{
			if( mInputTerminalNameIndex < mInputs.size() )
			{
				mInputs[ mInputTerminalNameIndex++ ].mName = QString::fromUtf8( Response.response() );
			}
		}
	}
	else if( Response.command() == "IRES" )
	{
		if( Response.resposeCode() == PJLinkReponse::OK )
		{
			if( mInputResolution.set( sizeFromResponse( Response.response() ) ) )
			{
				emit clientUpdated();
			}
		}
	}
	else if( Response.command() == "RRES" )
	{
		if( Response.resposeCode() == PJLinkReponse::OK )
		{
			if( mRecommendedResolution.set( sizeFromResponse( Response.response() ) ) )
			{
				emit clientUpdated();
			}
		}
	}
	else if( Response.command() == "FILT" )
	{
		if( Response.resposeCode() == PJLinkReponse::OK )
		{
			if( mFilterUsageTime.set( Response.response().toInt() ) )
			{
				emit clientUpdated();
			}
		}
	}
	else if( Response.command() == "RLMP" )
	{
		if( Response.resposeCode() == PJLinkReponse::OK )
		{
			if( mLampModelNumber.set( Response.response().split( ' ' ) ) )
			{
				emit clientUpdated();
			}
		}
	}
	else if( Response.command() == "RFIL" )
	{
		if( Response.resposeCode() == PJLinkReponse::OK )
		{
			if( mFilterModelNumber.set( Response.response().split( ' ' ) ) )
			{
				emit clientUpdated();
			}
		}
	}
	else if( Response.command() == "SVOL" )
	{
		if( Response.response().toUpper() == "OK" )
		{

		}
		else
		{
		}
	}
	else if( Response.command() == "MVOL" )
	{
		if( Response.response().toUpper() == "OK" )
		{

		}
		else
		{
		}
	}
	else if( Response.command() == "FREZ" )
	{
		if( Response.response().toUpper() == "OK" )
		{
			if( mFreeze.update() )
			{
				emit clientUpdated();
			}
		}
		else
		{
			if( mFreeze.set( Response.response().toInt() ) )
			{
				emit clientUpdated();
			}
		}
	}
}

void PJLinkClient::queryStatus()
{
	if( mClass.current() >= 1 )
	{
		sendCommand( QByteArray( "%1NAME ?\r" ) );
		sendCommand( QByteArray( "%1INF1 ?\r" ) );
		sendCommand( QByteArray( "%1INF2 ?\r" ) );
		sendCommand( QByteArray( "%1INFO ?\r" ) );
		sendCommand( QByteArray( "%1POWR ?\r" ) );
		sendCommand( QByteArray( "%1AVMT ?\r" ) );
		sendCommand( QByteArray( "%1ERST ?\r" ) );
		sendCommand( QByteArray( "%1LAMP ?\r" ) );
	}

	if( mClass.current() == 1 )
	{
		sendCommand( QByteArray( "%1INST ?\r" ) );
		sendCommand( QByteArray( "%1INPT ?\r" ) );
	}

	if( mClass.current() >= 2 )
	{
		sendCommand( QByteArray( "%2INST ?\r" ) );
		sendCommand( QByteArray( "%2SNUM ?\r" ) );
		sendCommand( QByteArray( "%2SVER ?\r" ) );
		sendCommand( QByteArray( "%2RLMP ?\r" ) );
		sendCommand( QByteArray( "%2RFIL ?\r" ) );
		sendCommand( QByteArray( "%2INPT ?\r" ) );
		sendCommand( QByteArray( "%2IRES ?\r" ) );
		sendCommand( QByteArray( "%2RRES ?\r" ) );
		sendCommand( QByteArray( "%2FILT ?\r" ) );
		sendCommand( QByteArray( "%2FREZ ?\r" ) );
	}
}
