#include "sindegreenode.h"

#include <fugio/core/uuid.h>

#include <fugio/context_interface.h>

#include <qmath.h>

SinDegreeNode::SinDegreeNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	mPinInput = pinInput( "Degrees" );

	mValOutput = pinOutput<fugio::VariantInterface *>( "Sin", mPinOutput, PID_FLOAT );
}

void SinDegreeNode::inputsUpdated( qint64 pTimeStamp )
{
	Q_UNUSED( pTimeStamp )

	bool		b;
	double		v = variant( mPinInput ).toDouble( &b );

	if( !b )
	{
		return;
	}

	v = std::sin( ( v / 180.0 ) * M_PI );

	if( v != mValOutput->variant().toDouble() )
	{
		mValOutput->setVariant( v );

		pinUpdated( mPinOutput );
	}
}
