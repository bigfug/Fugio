#include "integertofloatnode.h"

#include <fugio/core/uuid.h>
#include <fugio/node_signals.h>
#include <fugio/pin_variant_iterator.h>

IntegerToFloatNode::IntegerToFloatNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_INTEGER,		"8B572C7B-42CE-4EE9-A2C2-9C90D004EE1D" );
	FUGID( PIN_OUTPUT_FLOAT,		"DD67A091-B7C0-4F65-8DE5-DCBD7F66CE03" );

	QSharedPointer<fugio::PinInterface>		PinI, PinO;

	PinI = pinInput( "Integer", PIN_INPUT_INTEGER );
	PinO = pinOutput( "Float", PID_FLOAT, PIN_OUTPUT_FLOAT );

	PinI->setAutoRename( true );
	PinO->setAutoRename( true );

	mNode->pairPins( PinI, PinO );
}

void IntegerToFloatNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	for( fugio::NodeInterface::UuidPair UP : mNode->pairedPins() )
	{
		QSharedPointer<fugio::PinInterface>		 PinI = mNode->findPinByLocalId( UP.first );
		QSharedPointer<fugio::PinInterface>		 PinO = mNode->findPinByLocalId( UP.second );
		fugio::VariantInterface					*VarO = ( PinO && PinO->hasControl() ? qobject_cast<fugio::VariantInterface *>( PinO->control()->qobject() ) : nullptr );

		if( !PinI || !VarO )
		{
			continue;
		}

		fugio::PinVariantIterator	PinItr( PinI );
		bool						OK;
		bool						PinUpd = false;

		VarO->setVariantCount( PinItr.count() );

		for( int i = 0 ; i < PinItr.count() ; i++ )
		{
			float		V = PinItr.index( i ).toInt( &OK );

			if( OK && VarO->variant( i ).toFloat() != V )
			{
				VarO->setVariant( i, V );

				PinUpd = true;
			}
		}

		if( PinUpd )
		{
			pinUpdated( PinO );
		}
	}
}

QList<QUuid> IntegerToFloatNode::pinAddTypesInput() const
{
	static QList<QUuid> PinLst =
	{
		PID_INTEGER
	};

	return( PinLst );
}

bool IntegerToFloatNode::canAcceptPin( fugio::PinInterface *pPin ) const
{
	Q_UNUSED( pPin )

	return( true );
}

bool IntegerToFloatNode::pinShouldAutoRename( fugio::PinInterface *pPin ) const
{
	return( true );
}

QUuid IntegerToFloatNode::pairedPinControlUuid( QSharedPointer<fugio::PinInterface> pPin ) const
{
	return( pPin->direction() == PIN_OUTPUT ? PID_INTEGER : PID_FLOAT );
}
