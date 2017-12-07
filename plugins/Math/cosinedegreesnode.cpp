#include "cosinedegreesnode.h"

#include <fugio/core/uuid.h>

#include <fugio/context_interface.h>

#include <qmath.h>

CosineDegreeNode::CosineDegreeNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	mPinInput = pinInput( "Degrees" );

	mValOutput = pinOutput<fugio::VariantInterface *>( "Cos", mPinOutput, PID_FLOAT );
}

void CosineDegreeNode::inputsUpdated( qint64 pTimeStamp )
{
	Q_UNUSED( pTimeStamp )

	bool		b;
	double		v = variant( mPinInput ).toDouble( &b );

	if( !b )
	{
		return;
	}

	v = std::cos( ( v / 180.0 ) * M_PI );

	if( v != mValOutput->variant().toDouble() )
	{
		mValOutput->setVariant( v );

		pinUpdated( mPinOutput );
	}
}
