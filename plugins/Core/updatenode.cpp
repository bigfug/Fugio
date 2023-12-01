#include "updatenode.h"

#include <fugio/core/uuid.h>

#include <fugio/pin_variant_iterator.h>

UpdateNode::UpdateNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_UPDATE, "c6b773cb-791e-431e-a265-7ec3cdd6cc46" );
	FUGID( PIN_INPUT_FIRST,  "d34d5a72-c513-4fce-a27a-65dbe831feee" );
	FUGID( PIN_OUTPUT_FIRST, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );

	QSharedPointer<fugio::PinInterface>			 PinInp;
	QSharedPointer<fugio::PinInterface>			 PinOut;

	mPinInputTrigger = pinInput( tr( "Trigger" ), PID_FUGIO_NODE_TRIGGER );

	mPinInputAlwaysUpdate = pinInput( tr( "Always" ), PIN_INPUT_UPDATE );

	mPinInputAlwaysUpdate->setValue( true );

	PinInp = pinInput( tr( "Input" ), PIN_INPUT_FIRST );

	PinOut = pinOutput( tr( "Output" ), PIN_OUTPUT_FIRST );

	PinInp->setAutoRename( true );

	mNode->pairPins( PinInp, PinOut );

	connect( mNode->qobject(), SIGNAL(pinLinked(QSharedPointer<fugio::PinInterface>,QSharedPointer<fugio::PinInterface>)), this, SLOT(pinLinked(QSharedPointer<fugio::PinInterface>,QSharedPointer<fugio::PinInterface>)) );

	connect( mNode->qobject(), SIGNAL(pinUnlinked(QSharedPointer<fugio::PinInterface>,QSharedPointer<fugio::PinInterface>)), this, SLOT(pinUnlinked(QSharedPointer<fugio::PinInterface>,QSharedPointer<fugio::PinInterface>)) );
}

void UpdateNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	const bool UpdateAll    = mPinInputTrigger->isUpdated( pTimeStamp );
	const bool UpdateAlways = variant<bool>( mPinInputAlwaysUpdate );

	for( fugio::NodeInterface::UuidPair UP : mNode->pairedPins() )
	{
		QSharedPointer<fugio::PinInterface>  SrcPin = mNode->findPinByLocalId( UP.first );
		QSharedPointer<fugio::PinInterface>  DstPin = mNode->findPinByLocalId( UP.second );

		if( !DstPin )
		{
			continue;
		}

		if( SrcPin->isUpdated( pTimeStamp ) || ( UpdateAll && UpdateAlways ) )
		{
			pinUpdated( DstPin );
		}
	}
}

bool UpdateNode::canAcceptPin( fugio::PinInterface *pPin ) const
{
	return( pPin->direction() == PIN_OUTPUT );
}

QList<QUuid> UpdateNode::pinAddTypesInput() const
{
	return( QList<QUuid>() );
}

QUuid UpdateNode::pinAddControlUuid( fugio::PinInterface *pPin ) const
{
	Q_UNUSED( pPin )

	return( QUuid() );
}

bool UpdateNode::pinShouldAutoRename( fugio::PinInterface *pPin ) const
{
	return( pPin->direction() == PIN_INPUT );
}

void UpdateNode::pinLinked( QSharedPointer<fugio::PinInterface> pPinSrc, QSharedPointer<fugio::PinInterface> pPinDst )
{
	if( pPinDst->direction() != PIN_OUTPUT )
	{
		return;
	}

	if( pPinSrc->node() != mNode || pPinSrc->direction() != PIN_INPUT )
	{
		return;
	}

	QSharedPointer<fugio::PinInterface>		PinLnk = mNode->findPinByLocalId( pPinSrc->pairedUuid() );

	if( PinLnk )
	{
		PinLnk->setControl( pPinDst->control() );
	}
}

void UpdateNode::pinUnlinked( QSharedPointer<fugio::PinInterface> pPinSrc, QSharedPointer<fugio::PinInterface> pPinDst )
{
	if( pPinDst->direction() != PIN_OUTPUT )
	{
		return;
	}

	if( pPinSrc->node() != mNode || pPinSrc->direction() != PIN_INPUT )
	{
		return;
	}

	QSharedPointer<fugio::PinInterface>		PinLnk = mNode->findPinByLocalId( pPinSrc->pairedUuid() );

	if( PinLnk )
	{
		PinLnk->setControl( QSharedPointer<fugio::PinControlInterface>());
	}
}
