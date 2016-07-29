#include "counternode.h"

#include <fugio/core/uuid.h>

#include <fugio/context_interface.h>

CounterNode::CounterNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( ID_RESET,	"7AC449C1-0CC8-4DEA-A404-BB439BDD976E" );
	FUGID( ID_COUNT,	"7A49997F-F720-4EBA-81D3-347F00C55CB9" );

	mPinInputTrigger = pinInput( "Trigger", PID_FUGIO_NODE_TRIGGER );

	mPinInputReset   = pinInput( "Reset", ID_RESET );

	mOutputCount = pinOutput<fugio::VariantInterface *>( "Count", mPinOutputCount, PID_INTEGER, ID_COUNT );
}

void CounterNode::inputsUpdated( qint64 pTimeStamp )
{
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
