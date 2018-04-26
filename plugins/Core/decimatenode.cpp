#include "decimatenode.h"

#include <fugio/core/uuid.h>

#include <fugio/pin_variant_iterator.h>

DecimateNode::DecimateNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_COUNT, "c6b773cb-791e-431e-a265-7ec3cdd6cc46" );
	FUGID( PIN_INPUT_RESET, "CB7333A0-4D5F-441C-80FA-D6220132C6BC" );
	FUGID( PIN_INPUT_VALUE, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_OUTPUT_VALUE, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );

	mPinInputCount = pinInput( tr( "Count" ), PIN_INPUT_COUNT );

	mPinInputReset = pinInput( tr( "Reset" ), PIN_INPUT_RESET );

	mPinInput = pinInput( tr( "Input" ), PIN_INPUT_VALUE );

	mPinOutput = pinOutput( "Output", PIN_OUTPUT_VALUE );

	mPinInput->setAutoRename( true );

	mNode->pairPins( mPinInput, mPinOutput );

	connect( mNode->qobject(), SIGNAL(pinLinked(QSharedPointer<fugio::PinInterface>,QSharedPointer<fugio::PinInterface>)), this, SLOT(pinLinked(QSharedPointer<fugio::PinInterface>,QSharedPointer<fugio::PinInterface>)) );

	connect( mNode->qobject(), SIGNAL(pinUnlinked(QSharedPointer<fugio::PinInterface>,QSharedPointer<fugio::PinInterface>)), this, SLOT(pinUnlinked(QSharedPointer<fugio::PinInterface>,QSharedPointer<fugio::PinInterface>)) );
}

void DecimateNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	const int	Count = variant<int>( mPinInputCount );

	if( mPinInputReset->isUpdated( pTimeStamp ) )
	{
		for( QMap<QUuid,int>::iterator it = mCounterData.begin() ; it != mCounterData.end() ; it++ )
		{
			it.value() = Count;
		}
	}

	for( fugio::NodeInterface::UuidPair UP : mNode->pairedPins() )
	{
		QSharedPointer<fugio::PinInterface>  SrcPin = mNode->findPinByLocalId( UP.first );
		QSharedPointer<fugio::PinInterface>  DstPin = mNode->findPinByLocalId( UP.second );

		if( SrcPin->isUpdated( pTimeStamp ) )
		{
			int		C = mCounterData.value( SrcPin->globalId(), Count );

			if( C >= Count - 1 )
			{
				pinUpdated( DstPin );

				C = 0;
			}
			else
			{
				C++;
			}

			mCounterData.insert( SrcPin->globalId(), C );
		}
	}
}

QUuid DecimateNode::pairedPinControlUuid( QSharedPointer<fugio::PinInterface> pPin ) const
{
	Q_UNUSED( pPin )

	return( QUuid() );
}

void DecimateNode::pinLinked( QSharedPointer<fugio::PinInterface> P1, QSharedPointer<fugio::PinInterface> P2 )
{
	if( P1->direction() != PIN_OUTPUT )
	{
		std::swap( P1, P2 );
	}

	if( P1->node() == mNode )
	{
		return;
	}

//	qDebug() << "link" << P1->node()->name() << ":" << P1->name() << "->" << P2->node()->name() << ":" << P2->name();

	QSharedPointer<fugio::PinControlInterface>	PCI = P1->control();

	for( fugio::NodeInterface::UuidPair UP : mNode->pairedPins() )
	{
		if( UP.first != P2->localId() )
		{
			continue;
		}

		QSharedPointer<fugio::PinInterface>  DstPin = mNode->findPinByLocalId( UP.second );

		if( !DstPin )
		{
			continue;
		}

		DstPin->setControl( PCI );
	}
}

void DecimateNode::pinUnlinked(QSharedPointer<fugio::PinInterface> P1, QSharedPointer<fugio::PinInterface> P2)
{
	if( P1->direction() != PIN_OUTPUT )
	{
		std::swap( P1, P2 );
	}

	if( P1->node() == mNode )
	{
		return;
	}

//	qDebug() << "unlink" << P1->node()->name() << ":" << P1->name() << "->" << P2->node()->name() << ":" << P2->name();

	for( fugio::NodeInterface::UuidPair UP : mNode->pairedPins() )
	{
		if( UP.first != P2->localId() )
		{
			continue;
		}

		QSharedPointer<fugio::PinInterface>  DstPin = mNode->findPinByLocalId( UP.second );

		if( !DstPin )
		{
			continue;
		}

		DstPin->setControl( QSharedPointer<fugio::PinControlInterface>() );
	}
}

bool DecimateNode::canAcceptPin( fugio::PinInterface *pPin ) const
{
	return( pPin->direction() == PIN_OUTPUT );
}
