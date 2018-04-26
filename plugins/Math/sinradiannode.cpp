#include "sinradiannode.h"

#include <fugio/core/uuid.h>

#include <fugio/context_interface.h>

#include <qmath.h>

#include <fugio/pin_variant_iterator.h>

SinRadianNode::SinRadianNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	mPinInput = pinInput( "Radians" );

	mValOutput = pinOutput<fugio::VariantInterface *>( "Sin", mPinOutput, PID_FLOAT );
}

void SinRadianNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	fugio::PinVariantIterator	Input( mPinInput );

	bool	OutputUpdated = false;

	variantSetCount( mValOutput, Input.count(), OutputUpdated );

	for( int i = 0 ; i < Input.count() ; i++ )
	{
		variantSetValue( mValOutput, i, std::cos( Input.index( i ).toDouble() ), OutputUpdated );
	}

	if( OutputUpdated )
	{
		pinUpdated( mPinOutput );
	}
}
