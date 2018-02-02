#include "splitrectnode.h"

#include <fugio/core/uuid.h>
#include <fugio/context_interface.h>

#include <QSizeF>
#include <QPointF>
#include <QRectF>

SplitRectNode::SplitRectNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_RECT, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_OUTPUT_POSITION, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );
	FUGID( PIN_OUTPUT_SIZE, "261cc653-d7fa-4c34-a08b-3603e8ae71d5" );

	mPinInputRect = pinInput( tr( "Rect" ), PIN_INPUT_RECT );

	mPinInputRect->registerPinInputType( PID_RECT );

	mValOutputPosition = pinOutput<fugio::VariantInterface *>( tr( "Position" ), mPinOutputPosition, PID_POINT, PIN_OUTPUT_POSITION );

	mValOutputSize = pinOutput<fugio::VariantInterface *>( tr( "Size" ), mPinOutputSize, PID_SIZE, PIN_OUTPUT_SIZE );
}

void SplitRectNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	QRectF		R = variant<QRectF>( mPinInputRect );
	QPointF		P = R.topLeft();
	QSizeF		S = R.size();

	if( mValOutputPosition->variant().value<QPointF>() != P )
	{
		mValOutputPosition->setVariant( P );

		pinUpdated( mPinOutputPosition );
	}

	if( mValOutputSize->variant().value<QSizeF>() != S )
	{
		mValOutputSize->setVariant( S );

		pinUpdated( mPinOutputSize );
	}
}
