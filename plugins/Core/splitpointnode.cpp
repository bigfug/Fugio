#include "splitpointnode.h"

#include <fugio/core/uuid.h>
#include <fugio/context_interface.h>

#include <QPointF>

SplitPointNode::SplitPointNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	mPinInput = pinInput( tr( "Point" ) );

	mPinInput->registerPinInputType( PID_POINT );

	mX  = pinOutput<fugio::VariantInterface *>( tr( "X" ), mPinX, PID_FLOAT );

	mY = pinOutput<fugio::VariantInterface *>( tr( "Y" ), mPinY, PID_FLOAT );
}

void SplitPointNode::inputsUpdated( qint64 pTimeStamp )
{
	if( mPinInput->isUpdated( pTimeStamp ) )
	{
		QPointF				 CurPnt = variant( mPinInput ).toPointF();

		if( mX->variant().toFloat() != CurPnt.x() )
		{
			mX->setVariant( CurPnt.x() );

			pinUpdated( mPinX );
		}

		if( mY->variant().toFloat() != CurPnt.y() )
		{
			mY->setVariant( CurPnt.y() );

			pinUpdated( mPinY );
		}
	}
}
