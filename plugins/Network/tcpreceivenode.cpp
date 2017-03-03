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

	connect( &mServer, SIGNAL(newConnection()), this, SLOT(serverNewConnection()) );

	mNode->setStatus( fugio::NodeInterface::Warning );

	return( true );
}

bool TCPReceiveNode::deinitialise()
{
	return( NodeControlBase::deinitialise() );
}

void TCPReceiveNode::serverNewConnection()
{
	QTcpSocket		*S = mServer.nextPendingConnection();

	if( S )
	{
		connect( S, &QTcpSocket::readyRead, this, &TCPReceiveNode::serverRead );
		connect( S, &QTcpSocket::disconnected, this, &TCPReceiveNode::serverDisconnected );

		mStreams << new QDataStream( S );
	}
}

void TCPReceiveNode::serverDisconnected()
{
	QTcpSocket		*X = qobject_cast<QTcpSocket *>( sender() );

	for( int i = 0 ; i < mStreams.size() ; )
	{
		QDataStream		*D = mStreams.at( i );
		QTcpSocket		*S = qobject_cast<QTcpSocket *>( D->device() );

		if( X == S )
		{
			mStreams.removeAt( i );

			X->deleteLater();

			delete D;

			break;
		}
	}
}

void TCPReceiveNode::serverRead()
{
	QTcpSocket		*X = qobject_cast<QTcpSocket *>( sender() );

	for( int i = 0 ; i < mStreams.size() ; )
	{
		QDataStream		*D = mStreams.at( i );
		QTcpSocket		*S = qobject_cast<QTcpSocket *>( D->device() );

		if( X == S )
		{
			QUuid		ControlId, LocalId;
			QString		Name;

			D->startTransaction();

			*D >> LocalId >> ControlId >> Name;

			if( !D->commitTransaction() )
			{
				return;
			}

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

			mNode->setStatus( fugio::NodeInterface::Initialised );

			return;
		}
	}
}

void TCPReceiveNode::inputsUpdated( qint64 pTimeStamp )
{
	if( mPinPort->isUpdated( pTimeStamp ) )
	{
		mServer.close();

		mNode->setStatus( fugio::NodeInterface::Warning );

		for( int i = 0 ; i < mStreams.size() ; )
		{
			QDataStream		*D = mStreams.at( i );
			QTcpSocket		*S = qobject_cast<QTcpSocket *>( D->device() );

			S->deleteLater();

			delete D;
		}

		mStreams.clear();

		mServer.listen( QHostAddress::Any, variant( mPinPort ).toInt() );

		return;
	}
}

