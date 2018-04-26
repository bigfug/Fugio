#include "sindegreenode.h"

#include <fugio/core/uuid.h>

#include <fugio/context_interface.h>

#include <qmath.h>

#include <fugio/pin_variant_iterator.h>

SinDegreeNode::SinDegreeNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	mPinInput = pinInput( "Degrees" );

	mValOutput = pinOutput<fugio::VariantInterface *>( "Sin", mPinOutput, PID_FLOAT );
}

void SinDegreeNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	fugio::PinVariantIterator	Input( mPinInput );

	bool	OutputUpdated = false;

	variantSetCount( mValOutput, Input.count(), OutputUpdated );

	for( int i = 0 ; i < Input.count() ; i++ )
	{
		variantSetValue( mValOutput, i, std::sin( ( Input.index( i ).toDouble() / 180.0 ) * M_PI ), OutputUpdated );
	}

	if( OutputUpdated )
	{
		pinUpdated( mPinOutput );
	}
}
