#include "udpreceiverawnode.h"

#include <QtEndian>

#include <fugio/core/uuid.h>
#include <fugio/context_signals.h>
#include <fugio/performance.h>
#include <fugio/serialise_interface.h>

#include <fugio/context_interface.h>
#include <fugio/core/array_interface.h>
#include <fugio/core/variant_interface.h>

#include "networkplugin.h"

UDPReceiveRawNode::UDPReceiveRawNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mStream( nullptr )
{
	static const QUuid	ID_DATA = QUuid( "{70E5469F-A27B-4A54-BDE0-C2547A8EA269}" );

	mPinPort = pinInput( "Port" );

	mPinPort->registerPinInputType( PID_INTEGER );

	mPinPort->setValue( 7878 );

	mValOutputBuffer = pinOutput<fugio::VariantInterface *>( "Data", mPinOutputBuffer, PID_BYTEARRAY_LIST, ID_DATA );
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
	if( mSocket.state() != QUdpSocket::BoundState )
	{
		return;
	}

	QVariantList	DatagramList;

	while( mSocket.hasPendingDatagrams() )
	{
		fugio::Performance	Perf( mNode, "frameStart", pTimeStamp );

		QByteArray		Datagram;

		Datagram.resize( mSocket.pendingDatagramSize() );

		mSocket.readDatagram( Datagram.data(), Datagram.size() );

		DatagramList.append( Datagram );
	}

	mValOutputBuffer->setVariant( DatagramList );

	if( !DatagramList.isEmpty() )
	{
		pinUpdated( mPinOutputBuffer );
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

	if( mSocket.state() == QUdpSocket::BoundState )
	{
		mNode->setStatus( NodeInterface::Initialised );

		mNode->setStatusMessage( tr( "Bound" ) );
	}
}

