#include "cosinedegreesnode.h"

#include <fugio/core/uuid.h>

#include <fugio/context_interface.h>

#include <qmath.h>

#include <fugio/pin_variant_iterator.h>

CosineDegreeNode::CosineDegreeNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	mPinInput = pinInput( "Degrees" );

	mValOutput = pinOutput<fugio::VariantInterface *>( "Cos", mPinOutput, PID_FLOAT );
}

void CosineDegreeNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	fugio::PinVariantIterator	Input( mPinInput );

	bool	OutputUpdated = mValOutput->variantCount() != Input.size();

	mValOutput->setVariantCount( Input.size() );

	for( int i = 0 ; i < Input.size() ; i++ )
	{
		double		NewVal = std::cos( ( Input.index( i ).toDouble() / 180.0 ) * M_PI );

		if( NewVal != mValOutput->variant( i ).toDouble() )
		{
			mValOutput->setVariant( i, NewVal );

			OutputUpdated = true;
		}
	}

	if( OutputUpdated )
	{
		pinUpdated( mPinOutput );
	}
}
