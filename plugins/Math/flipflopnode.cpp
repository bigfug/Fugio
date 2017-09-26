#include "flipflopnode.h"

#include <QBitArray>

#include <fugio/core/uuid.h>

#include <fugio/context_interface.h>

FlipFlopNode::FlipFlopNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mValLst( false )
{
	FUGID( PII_NUMBER1, "c13a41c6-544b-46bb-a9f2-19dd156d236c" );
	FUGID( PIN_OUTPUT_BOOL, "0fb3ba87-ff71-41bc-84ee-4f488a18068f" );

	mPinInputTrigger = pinInput( "Trigger", PID_FUGIO_NODE_TRIGGER );

	mPinInputValue = pinInput( "Boolean", PII_NUMBER1 );

	mPinInputValue->setUpdatable( false );

	mValOutputValue = pinOutput<fugio::VariantInterface *>( "Boolean", mPinOutputValue, PID_BOOL, PIN_OUTPUT_BOOL );
}

void FlipFlopNode::inputsUpdated( qint64 pTimeStamp )
{
	if( mPinInputTrigger->isUpdated( pTimeStamp ) )
	{
		bool		NxtVal = variant( mPinInputValue ).toBool();

		if( mValLst != mValOutputValue->variant().toBool() )
		{
			mValOutputValue->setVariant( mValLst );

			pinUpdated( mPinOutputValue );
		}

		mValLst = NxtVal;
	}
}
