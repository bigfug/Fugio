#include "togglenode.h"

#include <fugio/core/uuid.h>

#include <fugio/context_interface.h>
#include <fugio/core/variant_interface.h>

ToggleNode::ToggleNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	mPinInput  = pinInput( "Trigger" );

	mValOutput = pinOutput<fugio::VariantInterface *>( "Boolean", mPinOutput, PID_BOOL );

	mValOutput->setVariant( false );
}

void ToggleNode::inputsUpdated( qint64 pTimeStamp )
{
	Q_UNUSED( pTimeStamp )

	if( mPinInput->isUpdated( pTimeStamp ) )
	{
		if( mValOutput->variant().toBool() )
		{
			mValOutput->setVariant( false );
		}
		else
		{
			mValOutput->setVariant( true );
		}

		mNode->context()->pinUpdated( mPinOutput );
	}
}
