#include "counternode.h"

#include <fugio/core/uuid.h>

#include <fugio/context_interface.h>

CounterNode::CounterNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	static const QUuid		ID_RESET = QUuid( "{7AC449C1-0CC8-4DEA-A404-BB439BDD976E}" );
	static const QUuid		ID_COUNT = QUuid( "{7A49997F-F720-4EBA-81D3-347F00C55CB9}" );

	mPinInputTrigger = pinInput( "Trigger", PID_FUGIO_NODE_TRIGGER );

	mPinInputReset   = pinInput( "Reset", ID_RESET );

	mOutputCount = pinOutput<fugio::VariantInterface *>( "Count", mPinOutputCount, PID_INTEGER, ID_COUNT );
}

void CounterNode::inputsUpdated( qint64 pTimeStamp )
{
	if( !pTimeStamp )
	{
		mOutputCount->setVariant( 0 );

		pinUpdated( mPinOutputCount );

		return;
	}

	if( mPinInputReset->isUpdated( pTimeStamp ) )
	{
		if( mOutputCount->variant().toInt() != 0 )
		{
			mOutputCount->setVariant( 0 );

			pinUpdated( mPinOutputCount );
		}
	}

	if( mPinInputTrigger->isUpdated( pTimeStamp ) )
	{
		int		i = mOutputCount->variant().toInt() + 1;

		mOutputCount->setVariant( i );

		pinUpdated( mPinOutputCount );
	}
}
