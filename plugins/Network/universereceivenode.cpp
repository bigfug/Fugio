#include "universereceivenode.h"

#include <fugio/global_interface.h>
#include <fugio/context_signals.h>
#include <fugio/serialise_interface.h>

#include "networkplugin.h"

UniverseReceiveNode::UniverseReceiveNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{

}

bool UniverseReceiveNode::initialise()
{
	if( !NodeControlBase::initialise() )
	{
		return( false );
	}

	connect( node()->context()->qobject(), SIGNAL(frameStart()), this, SLOT(contextFrame()) );

	return( true );
}

bool UniverseReceiveNode::deinitialise()
{
	disconnect( node()->context()->qobject(), SIGNAL(frameStart()), this, SLOT(contextFrame()) );

	return( NodeControlBase::deinitialise() );
}

void UniverseReceiveNode::inputsUpdated( qint64 pTimeStamp )
{
	Q_UNUSED( pTimeStamp )
}

QList<fugio::NodeControlInterface::AvailablePinEntry> UniverseReceiveNode::availableOutputPins() const
{
	QList<fugio::NodeControlInterface::AvailablePinEntry>	PinLst;

	fugio::GlobalInterface	*Global = NetworkPlugin::instance()->app();

	for( fugio::GlobalInterface::UniverseEntry UE : Global->universeEntries() )
	{
		if( mNode->findPinByLocalId( UE.mUuid ) )
		{
			continue;
		}

		PinLst << fugio::NodeControlInterface::AvailablePinEntry( UE.mName, UE.mType, UE.mUuid );
	}

	return( PinLst );
}

void UniverseReceiveNode::contextFrame()
{
	fugio::GlobalInterface	*Global = NetworkPlugin::instance()->app();

	for( QSharedPointer<fugio::PinInterface> P : node()->enumOutputPins() )
	{
		QString		Name;
		QByteArray	Data;
		QUuid		Type;

		if( !Global->universeData( Global->universalTimestamp(), P->localId(), Name, Type, Data ) )
		{
			continue;
		}

		if( P->name() != Name )
		{
			P->setName( Name );
		}

		fugio::SerialiseInterface		*SI = qobject_cast<fugio::SerialiseInterface *>( P->hasControl() ? P->control()->qobject() : nullptr );

		if( !SI )
		{
			continue;
		}

		if( true )
		{
			QDataStream	DS( Data );

			SI->deserialise( DS );
		}

		pinUpdated( P );
	}
}

