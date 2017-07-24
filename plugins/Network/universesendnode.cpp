#include "universesendnode.h"

#include <QDataStream>

#include <fugio/global_interface.h>
#include <fugio/serialise_interface.h>

#include "networkplugin.h"

UniverseSendNode::UniverseSendNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{

}

void UniverseSendNode::inputsUpdated( qint64 pTimeStamp )
{
	fugio::GlobalInterface	*Global = NetworkPlugin::instance()->app();

	for( QSharedPointer<fugio::PinInterface> P : node()->enumInputPins() )
	{
		if( !P->isUpdated( pTimeStamp ) )
		{
			continue;
		}

		fugio::SerialiseInterface		*SI = input<fugio::SerialiseInterface *>( P );

		if( !SI )
		{
			continue;
		}

		QByteArray						 BA;

		if( true )
		{
			QDataStream					 DS( &BA, QIODevice::WriteOnly );

			SI->serialise( DS );
		}

		if( BA.isEmpty() )
		{
			continue;
		}

		Global->sendToUniverse( Global->universalTimestamp() + 500, P->globalId(), P->name(), P->connectedPin()->controlUuid(), BA );
	}
}


bool UniverseSendNode::canAcceptPin( PinInterface *pPin ) const
{
	return( pPin->direction() == PIN_OUTPUT );
}
