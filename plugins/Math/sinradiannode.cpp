#include "sinradiannode.h"

#include <fugio/core/uuid.h>

#include <fugio/context_interface.h>

#include <qmath.h>

SinRadianNode::SinRadianNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	mPinInput = pinInput( "Radians" );

	mValOutput = pinOutput<fugio::VariantInterface *>( "Sin", mPinOutput, PID_FLOAT );
}

void SinRadianNode::inputsUpdated( qint64 pTimeStamp )
{
	Q_UNUSED( pTimeStamp )

	bool		b;
	double		v = variant( mPinInput ).toDouble( &b );

	if( !b )
	{
		return;
	}

	v = std::sin( v );

	if( v != mValOutput->variant().toDouble() )
	{
		mValOutput->setVariant( v );

		pinUpdated( mPinOutput );
	}
}
