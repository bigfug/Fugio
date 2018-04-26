#include "notnode.h"

#include <fugio/core/uuid.h>

#include <fugio/context_interface.h>

#include <fugio/pin_variant_iterator.h>

NotNode::NotNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	static const QUuid	PII_NUMBER1( "{c13a41c6-544b-46bb-a9f2-19dd156d236c}" );

	mPinInput = pinInput( "Boolean", PII_NUMBER1 );

	mValOutput = pinOutput<fugio::VariantInterface *>( "Boolean", mPinOutput, PID_BOOL );
}

void NotNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	bool		Update = mPinOutput->alwaysUpdate();

	fugio::VariantInterface		*SrcVar = input<fugio::VariantInterface *>( mPinInput );

	if( !SrcVar )
	{
		return;
	}

	variantSetCount( mValOutput, SrcVar->variantCount(), Update );

	for( int i = 0 ; i < SrcVar->variantCount() ; i++ )
	{
		variantSetValue( mValOutput, i, !SrcVar->variant( i ).toBool(), Update );
	}

	pinUpdated( mPinOutput, Update );
}
