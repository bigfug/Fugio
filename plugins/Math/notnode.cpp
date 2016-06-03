#include "notnode.h"

#include <fugio/core/uuid.h>

#include <fugio/context_interface.h>

NotNode::NotNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	static const QUuid	PII_NUMBER1( "{c13a41c6-544b-46bb-a9f2-19dd156d236c}" );

	mPinInput = pinInput( "Boolean", PII_NUMBER1 );

	mValOutput = pinOutput<fugio::VariantInterface *>( "Boolean", mPinOutput, PID_BOOL );
}

void NotNode::inputsUpdated( qint64 pTimeStamp )
{
	Q_UNUSED( pTimeStamp )

	bool		OutVal = variant( mPinInput ).toBool();

	OutVal = OutVal ? false : true;

	if( OutVal != mValOutput->variant().toBool() )
	{
		mValOutput->setVariant( OutVal );

		pinUpdated( mPinOutput );
	}
}
