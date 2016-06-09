#include "pinode.h"

#include <fugio/core/uuid.h>

#include <qmath.h>

PiNode::PiNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	mValOutput = pinOutput<fugio::VariantInterface *>( "PI", mPinOutput, PID_FLOAT );

	mValOutput->setVariant( double( M_PI ) );
}

