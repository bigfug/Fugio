#include "triggerbooleannode.h"

#include <fugio/core/uuid.h>

#include <fugio/context_interface.h>
#include <fugio/core/variant_interface.h>
#include <fugio/context_signals.h>

TriggerBooleanNode::TriggerBooleanNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mTimeStart( 0 )
{
	FUGID( PIN_INPUT_DURATION, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );
	FUGID( PIN_OUTPUT_BOOLEAN, "261cc653-d7fa-4c34-a08b-3603e8ae71d5" );

	mPinInput  = pinInput( "Trigger", PID_FUGIO_NODE_TRIGGER );

	mPinInputDuration = pinInput( "Duration", PIN_INPUT_DURATION );

	mPinInputDuration->setValue( 250 );

	mValOutput = pinOutput<fugio::VariantInterface *>( "Boolean", mPinOutput, PID_BOOL, PIN_OUTPUT_BOOLEAN );

	mValOutput->setVariant( false );
}

void TriggerBooleanNode::inputsUpdated( qint64 pTimeStamp )
{
	Q_UNUSED( pTimeStamp )

	if( mPinInput->isUpdated( pTimeStamp ) )
	{
		mValOutput->setVariant( true );

		pinUpdated( mPinOutput );

		connect( mNode->context()->qobject(), SIGNAL(frameStart(qint64)), this, SLOT(contextFrame(qint64)) );

		mTimeStart = pTimeStamp;
	}
}

void TriggerBooleanNode::contextFrame( qint64 pTimeStamp )
{
	int		Duration = variant( mPinInputDuration ).toInt();

	if( pTimeStamp - mTimeStart > Duration )
	{
		mValOutput->setVariant( false );

		pinUpdated( mPinOutput );

		disconnect( mNode->context()->qobject(), SIGNAL(frameStart(qint64)), this, SLOT(contextFrame(qint64)) );
	}
}
