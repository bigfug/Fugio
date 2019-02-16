#include "pjlinkserver.h"

#include <QCryptographicHash>

PJLinkServer::PJLinkServer( QObject *pParent )
	: QObject( pParent )
{
	mClientQueryTimer = new QTimer( this );

	connect( mClientQueryTimer, &QTimer::timeout, [=]( void )
	{
		emit clientQueryStatus();
	} );

	mClientQueryTimer->start( 10000 );

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
		C = new PJLinkClient( pAddress );

		connect( this, &PJLinkServer::clientQueryStatus, C, &PJLinkClient::updateStatus );

		mClientList << C;

		emit clientListChanged();
	}

	return( C );
}

PJLinkClient::PJLinkClient( QHostAddress pAddress )
	: mAddress( pAddress ), mAuthenticated( false ), mReady( false ),
	  mInputTerminalNameIndex( 0 )
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

void PJLinkClient::freeze( bool pFreeze )
{
	sendCommand( QString( "%%2FREZ %1\r" ).arg( pFreeze ? '1' : '0' ).toLocal8Bit() );
}

void PJLinkClient::switchInput(PJLinkClient::Input pInput, char pValue)
{
	sendCommand( QString( "%%2INPT %1%2\r" ).arg( pInput ).arg( pValue ).toLocal8Bit() );
}

void PJLinkClient::updateStatus()
{
	queryStatus();
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

			//qDebug() << mCommands.first();

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
				if( mClass.isUnknown() )
				{
					sendCommand( QByteArray( "%1CLSS ?\r" ) );
				}
				else
				{
					queryStatus();
				}
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
			setPower( powerFromChar( Response.response().at( 0 ) ) );
		}
	}
	else if( Response.command() == "NAME" )
	{
		if( Response.resposeCode() == PJLinkReponse::OK )
		{
			mName.set( Response.response() );
		}
	}
	else if( Response.command() == "INPT" )
	{
		if( Response.response().toUpper() == "OK" )
		{
			mInputType.update();
			mInput.update();
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
			mVideoMute.update();
			mAudioMute.update();
		}
		else
		{
			char	Type		= Response.response().at( 0 ) - '0';		// 1-3
			char	Value		= Response.response().at( 1 ) - '0';		// 0-1

			if( Type == 1 || Type == 3 )
			{
				mVideoMute.set( Value == '1' );
			}

			if( Type == 2 || Type == 3 )
			{
				mAudioMute.set( Value == '1' );
			}
		}
	}
	else if( Response.command() == "ERST" )
	{
		if( Response.response().toUpper() == "OK" )
		{
			mFanStatus.update();
			mLampStatus.update();
			mTemperatureStatus.update();
			mCoverOpenStatus.update();
			mFilterStatus.update();
			mOtherStatus.update();
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
			mManufacturer.set( Response.response() );
		}
	}
	else if( Response.command() == "INF2" )
	{
		if( Response.resposeCode() == PJLinkReponse::OK )
		{
			mProductName.set( Response.response() );
		}
	}
	else if( Response.command() == "INFO" )
	{
		if( Response.resposeCode() == PJLinkReponse::OK )
		{
			mOtherInformation.set( Response.response() );
		}
	}
	else if( Response.command() == "CLSS" )
	{
		if( Response.resposeCode() == PJLinkReponse::OK )
		{
			mClass.set( Response.response().toInt() );

			queryInfo();

			queryStatus();
		}
	}
	else if( Response.command() == "SNUM" )
	{
		if( Response.resposeCode() == PJLinkReponse::OK )
		{
			mSerialNumber.set( Response.response() );
		}
	}
	else if( Response.command() == "SVER" )
	{
		if( Response.resposeCode() == PJLinkReponse::OK )
		{
			mSoftwareVersion.set( Response.response() );
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
			mInputResolution.set( sizeFromResponse( Response.response() ) );
		}
	}
	else if( Response.command() == "RRES" )
	{
		if( Response.resposeCode() == PJLinkReponse::OK )
		{
			mRecommendedResolution.set( sizeFromResponse( Response.response() ) );
		}
	}
	else if( Response.command() == "FILT" )
	{
		if( Response.resposeCode() == PJLinkReponse::OK )
		{
			mFilterUsageTime.set( Response.response().toInt() );
		}
	}
	else if( Response.command() == "RLMP" )
	{
		if( Response.resposeCode() == PJLinkReponse::OK )
		{
			mLampModelNumber.set( Response.response().split( ' ' ) );
		}
	}
	else if( Response.command() == "RFIL" )
	{
		if( Response.resposeCode() == PJLinkReponse::OK )
		{
			mFilterModelNumber.set( Response.response().split( ' ' ) );
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
			mFreeze.update();
		}
		else
		{
			mFreeze.set( Response.response().toInt() );
		}
	}
}

void PJLinkClient::queryInfo()
{
	if( mClass.current() == 1 )
	{
		sendCommand( QByteArray( "%1INST ?\r" ) );
	}

	if( mClass.current() >= 1 )
	{
		sendCommand( QByteArray( "%1NAME ?\r" ) );
		sendCommand( QByteArray( "%1INF1 ?\r" ) );
		sendCommand( QByteArray( "%1INF2 ?\r" ) );
		sendCommand( QByteArray( "%1INFO ?\r" ) );
	}

	if( mClass.current() >= 2 )
	{
		sendCommand( QByteArray( "%2INST ?\r" ) );
		sendCommand( QByteArray( "%2SNUM ?\r" ) );
		sendCommand( QByteArray( "%2SVER ?\r" ) );
		sendCommand( QByteArray( "%2RLMP ?\r" ) );
		sendCommand( QByteArray( "%2RFIL ?\r" ) );
	}
}

void PJLinkClient::queryStatus()
{
	if( mClass.current() == 1 )
	{
		sendCommand( QByteArray( "%1INPT ?\r" ) );
	}

	if( mClass.current() >= 1 )
	{
		sendCommand( QByteArray( "%1POWR ?\r" ) );
		sendCommand( QByteArray( "%1AVMT ?\r" ) );
		sendCommand( QByteArray( "%1ERST ?\r" ) );
		sendCommand( QByteArray( "%1LAMP ?\r" ) );
	}

	if( mClass.current() >= 2 )
	{
		sendCommand( QByteArray( "%2INPT ?\r" ) );
		sendCommand( QByteArray( "%2IRES ?\r" ) );
		sendCommand( QByteArray( "%2RRES ?\r" ) );
		sendCommand( QByteArray( "%2FILT ?\r" ) );
		sendCommand( QByteArray( "%2FREZ ?\r" ) );
	}
}
