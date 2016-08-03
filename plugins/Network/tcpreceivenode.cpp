#include "tcpreceivenode.h"

#include <QtEndian>

#include <fugio/core/uuid.h>
#include <fugio/context_signals.h>
#include <fugio/performance.h>
#include <fugio/serialise_interface.h>

#include <fugio/context_interface.h>
#include <fugio/core/array_interface.h>
#include <fugio/core/variant_interface.h>

#include "networkplugin.h"

TCPReceiveNode::TCPReceiveNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	mPinPort = pinInput( "Port" );

	mPinPort->registerPinInputType( PID_INTEGER );

	mPinPort->setValue( 7979 );
}

bool TCPReceiveNode::initialise()
{
	if( !NodeControlBase::initialise() )
	{
		return( false );
	}

	connect( mNode->context()->qobject(), SIGNAL(frameStart(qint64)), this, SLOT(frameStart(qint64)) );

	connect( &mServer, SIGNAL(newConnection()), this, SLOT(serverNewConnection()) );

	return( true );
}

bool TCPReceiveNode::deinitialise()
{
	return( NodeControlBase::deinitialise() );
}

void TCPReceiveNode::frameStart( qint64 pTimeStamp )
{
	fugio::Performance	Perf( mNode, "frameStart", pTimeStamp );

	for( int i = 0 ; i < mStreams.size() ; )
	{
		QDataStream		*D = mStreams.at( i );
		QTcpSocket		*S = qobject_cast<QTcpSocket *>( D->device() );

		if( !S->isOpen() )
		{
			mStreams.removeAt( i );

			delete D;
			delete S;

			continue;
		}

		if( !S->bytesAvailable() )
		{
			i++;

			continue;
		}

		QUuid		ControlId, LocalId;
		QString		Name;

		*D >> LocalId;
		*D >> ControlId;
		*D >> Name;

		QSharedPointer<fugio::PinInterface>		P = mNode->findPinByLocalId( LocalId );

		if( !P )
		{
			P = pinOutput( Name, ControlId, LocalId );
		}

		if( SerialiseInterface *S = qobject_cast<SerialiseInterface *>( P->control()->qobject() ) )
		{
			S->deserialise( *D );

			pinUpdated( P );
		}

		i++;
	}
}

void TCPReceiveNode::serverNewConnection()
{
	QTcpSocket		*S = mServer.nextPendingConnection();

	if( S )
	{
		mStreams << new QDataStream( S );
	}
}

void TCPReceiveNode::inputsUpdated( qint64 pTimeStamp )
{
	if( mPinPort->isUpdated( pTimeStamp ) )
	{
		mServer.close();

		mServer.listen( QHostAddress::Any, variant( mPinPort ).toInt() );

		return;
	}
}

