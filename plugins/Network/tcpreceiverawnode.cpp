#include "tcpreceiverawnode.h"

#include <QtEndian>

#include <fugio/core/uuid.h>
#include <fugio/context_signals.h>
#include <fugio/performance.h>
#include <fugio/serialise_interface.h>

#include <fugio/context_interface.h>
#include <fugio/core/array_interface.h>
#include <fugio/core/variant_interface.h>

#include "networkplugin.h"

TCPReceiveRawNode::TCPReceiveRawNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mStream( nullptr )
{
	static const QUuid	ID_DATA = QUuid( "{70E5469F-A27B-4A54-BDE0-C2547A8EA269}" );

	mPinPort = pinInput( "Port" );

	mPinPort->registerPinInputType( PID_INTEGER );

	mPinPort->setValue( 7878 );

	mValOutputBuffer = pinOutput<fugio::VariantInterface *>( "Data", mPinOutputBuffer, PID_BYTEARRAY, ID_DATA );
}

bool TCPReceiveRawNode::initialise()
{
	if( !NodeControlBase::initialise() )
	{
		return( false );
	}

	connect( mNode->context()->qobject(), SIGNAL(frameStart(qint64)), this, SLOT(frameStart(qint64)) );

	connect( &mServer, SIGNAL(newConnection()), this, SLOT(serverNewConnection()) );

	return( true );
}

bool TCPReceiveRawNode::deinitialise()
{
	return( NodeControlBase::deinitialise() );
}

void TCPReceiveRawNode::frameStart( qint64 pTimeStamp )
{
	if( !mStream )
	{
		return;
	}

	QTcpSocket		*S = qobject_cast<QTcpSocket *>( mStream->device() );

	if( !S->isOpen() )
	{
		delete mStream;
		delete S;

		mStream = nullptr;

		return;
	}

	if( !S->bytesAvailable() )
	{
		return;
	}

	fugio::Performance	Perf( mNode, "frameStart", pTimeStamp );

	mValOutputBuffer->setVariant( S->readAll() );

	pinUpdated( mPinOutputBuffer );
}

void TCPReceiveRawNode::serverNewConnection()
{
	QTcpSocket		*S = mServer.nextPendingConnection();

	if( S )
	{
		if( mStream )
		{
			delete mStream;

			mStream = nullptr;
		}

		mStream = new QDataStream( S );
	}
}

void TCPReceiveRawNode::inputsUpdated( qint64 pTimeStamp )
{
	if( !pTimeStamp || mPinPort->isUpdated( pTimeStamp ) )
	{
		mServer.close();

		mServer.listen( QHostAddress::Any, variant( mPinPort ).toInt() );

		return;
	}
}

