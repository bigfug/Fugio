#include "triggerbooleannode.h"

#include <fugio/core/uuid.h>

#include <fugio/context_interface.h>
#include <fugio/core/variant_interface.h>
#include <fugio/context_signals.h>

TriggerBooleanNode::TriggerBooleanNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mTimeStart( 0 )
{
	mPinInput  = pinInput( "Trigger" );

	mValOutput = pinOutput<fugio::VariantInterface *>( "Boolean", mPinOutput, PID_BOOL );

	mValOutput->setVariant( false );
}

void TriggerBooleanNode::inputsUpdated( qint64 pTimeStamp )
{
	Q_UNUSED( pTimeStamp )

	if( mPinInput->isUpdated( pTimeStamp ) )
	{
		if( !mValOutput->variant().toBool() )
		{
			mValOutput->setVariant( true );

			pinUpdated( mPinOutput );

			connect( mNode->context()->qobject(), SIGNAL(frameStart(qint64)), this, SLOT(contextFrame(qint64)) );
		}

		mTimeStart = pTimeStamp;
	}
}

void TriggerBooleanNode::contextFrame( qint64 pTimeStamp )
{
	if( pTimeStamp - mTimeStart > 250 )
	{
		mValOutput->setVariant( false );

		pinUpdated( mPinOutput );

		disconnect( mNode->context()->qobject(), SIGNAL(frameStart(qint64)), this, SLOT(contextFrame(qint64)) );
	}
}
