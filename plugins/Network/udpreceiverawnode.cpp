#include "udpreceiverawnode.h"

#include <QtEndian>

#include <fugio/core/uuid.h>
#include <fugio/context_signals.h>
#include <fugio/performance.h>
#include <fugio/serialise_interface.h>

#include <fugio/context_interface.h>
#include <fugio/core/array_interface.h>

#include "networkplugin.h"

UDPReceiveRawNode::UDPReceiveRawNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mStream( nullptr ), mCurrTime( 0 ), mBytesReceived( 0 ),
	  mLastState( QAbstractSocket::UnconnectedState )
{
	FUGID( PIN_INPUT_PORT,		"9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_OUTPUT_DATA,		"70E5469F-A27B-4A54-BDE0-C2547A8EA269" );
	FUGID( PIN_OUTPUT_COUNT,	"94f9e92b-d531-4dcc-9907-ecba4d56baa4" );

	mPinPort = pinInput( "Port", PIN_INPUT_PORT );

	mPinPort->registerPinInputType( PID_INTEGER );

	mPinPort->setValue( 7878 );

	mValOutputBuffer = pinOutput<fugio::VariantInterface *>( "Data", mPinOutputBuffer, PID_BYTEARRAY, PIN_OUTPUT_DATA );

	mValOutputCount = pinOutput<fugio::VariantInterface *>( "Count", mPinOutputCount, PID_INTEGER, PIN_OUTPUT_COUNT );
}

bool UDPReceiveRawNode::initialise()
{
	if( !NodeControlBase::initialise() )
	{
		return( false );
	}

	connect( mNode->context()->qobject(), SIGNAL(frameStart(qint64)), this, SLOT(frameStart(qint64)) );

	return( true );
}

bool UDPReceiveRawNode::deinitialise()
{
	return( NodeControlBase::deinitialise() );
}

void UDPReceiveRawNode::frameStart( qint64 pTimeStamp )
{
	const QAbstractSocket::SocketState	SocketState = mSocket.state();

	if( SocketState != mLastState )
	{
		switch( SocketState )
		{
			case QAbstractSocket::UnconnectedState:
				mNode->setStatus( fugio::NodeInterface::Initialising );
				mNode->setStatusMessage( "Unconnected" );
				break;

			case QAbstractSocket::HostLookupState:
				mNode->setStatus( fugio::NodeInterface::Initialising );
				mNode->setStatusMessage( "Host Lookup" );
				break;

			case QAbstractSocket::ConnectingState:
				mNode->setStatus( fugio::NodeInterface::Initialising );
				mNode->setStatusMessage( "Connecting" );
				break;

			case QAbstractSocket::ConnectedState:
				mNode->setStatus( fugio::NodeInterface::Initialised );
				mNode->setStatusMessage( "Connected" );
				break;

			case QAbstractSocket::BoundState:
				mNode->setStatus( fugio::NodeInterface::Initialised );
				mNode->setStatusMessage( "Bound" );
				break;

			case QAbstractSocket::ClosingState:
				mNode->setStatus( fugio::NodeInterface::Initialising );
				mNode->setStatusMessage( "Closing" );
				break;

			case QAbstractSocket::ListeningState:
				mNode->setStatus( fugio::NodeInterface::Initialised );
				mNode->setStatusMessage( "Listening" );
				break;
		}

		mLastState = SocketState;
	}

	const qint64 CurrTime = QDateTime::currentMSecsSinceEpoch();
	const qint64 CurrSecs = CurrTime - ( CurrTime % 1000 );

	if( CurrSecs != mCurrTime )
	{
		if( mValOutputCount->variant().toInt() != mBytesReceived )
		{
			mValOutputCount->setVariant( mBytesReceived );

			pinUpdated( mPinOutputCount );
		}

		mBytesReceived = 0;

		mCurrTime = CurrSecs;
	}

	if( SocketState != QUdpSocket::BoundState )
	{
		return;
	}

	if( mSocket.hasPendingDatagrams() )
	{
		fugio::Performance	Perf( mNode, "frameStart", pTimeStamp );

		mValOutputBuffer->variantClear();

		while( mSocket.hasPendingDatagrams() )
		{
			QByteArray		Datagram;

			Datagram.resize( mSocket.pendingDatagramSize() );

			mBytesReceived += mSocket.readDatagram( Datagram.data(), Datagram.size() );

			mValOutputBuffer->variantAppend( Datagram );
		}

		if( mValOutputBuffer->variantCount() )
		{
			pinUpdated( mPinOutputBuffer );
		}
	}
}

void UDPReceiveRawNode::inputsUpdated( qint64 pTimeStamp )
{
	Q_UNUSED( pTimeStamp )

	quint16		PrtNum = variant( mPinPort ).toInt();

	if( !PrtNum )
	{
		mNode->setStatus( NodeInterface::Error );

		mNode->setStatusMessage( tr( "Port number cannot be zero" ) );

		return;
	}

	if( mSocket.localPort() != PrtNum )
	{
		mSocket.close();

		if( !mSocket.bind( QHostAddress::Any, PrtNum ) )
		{
			mNode->setStatus( NodeInterface::Error );

			mNode->setStatusMessage( tr( "Can't bind to port %1" ).arg( PrtNum ) );
		}
	}
}

