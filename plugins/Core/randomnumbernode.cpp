#include "randomnumbernode.h"

#include <fugio/global_interface.h>
#include <fugio/context_interface.h>
#include <fugio/core/uuid.h>

RandomNumberNode::RandomNumberNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mDistribution( 0.0, 1.0 )
{
	mPinTrigger = pinInput( "Trigger", PID_FUGIO_NODE_TRIGGER );

	QSharedPointer<fugio::PinInterface>	PinTmp;

	pinOutput<fugio::VariantInterface *>( tr( "Random" ), PinTmp, PID_FLOAT );
}

void RandomNumberNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	for( QSharedPointer<fugio::PinInterface> P : mNode->enumOutputPins() )
	{
		if( fugio::VariantInterface *V = qobject_cast<fugio::VariantInterface *>( P->control()->qobject() ) )
		{
			V->setVariant( mDistribution( mGenerator ) );

			pinUpdated( P );
		}
	}
}

QList<QUuid> RandomNumberNode::pinAddTypesOutput() const
{
	static QList<QUuid> PinLst;

	if( PinLst.isEmpty() )
	{
		PinLst << PID_FLOAT;
	}

	return( PinLst );
}

bool RandomNumberNode::canAcceptPin(fugio::PinInterface *pPin) const
{
	if( pPin->direction() != PIN_OUTPUT )
	{
		return( false );
	}

	return( true );
}
