#include "splitpointnode.h"

#include <fugio/core/uuid.h>
#include <fugio/context_interface.h>

#include <QPointF>

#include <fugio/pin_variant_iterator.h>

SplitPointNode::SplitPointNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_POINT, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_OUTPUT_X, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );
	FUGID( PIN_OUTPUT_Y, "261cc653-d7fa-4c34-a08b-3603e8ae71d5" );

	mPinInputPoint = pinInput( tr( "Point" ), PIN_INPUT_POINT );

	mPinInputPoint->registerPinInputType( PID_POINT );

	mValOutputX  = pinOutput<fugio::VariantInterface *>( tr( "X" ), mPinOutputX, PID_FLOAT, PIN_OUTPUT_X );

	mValOutputY = pinOutput<fugio::VariantInterface *>( tr( "Y" ), mPinOutputY, PID_FLOAT, PIN_OUTPUT_Y );
}

void SplitPointNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	if( !mPinInputPoint->isUpdated( pTimeStamp ) )
	{
		return;
	}

	fugio::PinVariantIterator	P( mPinInputPoint );

	if( !P.count() )
	{
		return;
	}

	const int		OutCnt = P.count();

	bool	UpdateOutputX = false;
	bool	UpdateOutputY = false;

	variantSetCount( mValOutputX, OutCnt, UpdateOutputX );
	variantSetCount( mValOutputY, OutCnt, UpdateOutputY );

	for( int i = 0 ; i < OutCnt ; i++ )
	{
		QPointF		p = P.index( i ).toPointF();

		variantSetValue( mValOutputX, i, p.x(), UpdateOutputX );
		variantSetValue( mValOutputY, i, p.y(), UpdateOutputY );
	}

	if( UpdateOutputX )
	{
		pinUpdated( mPinOutputX );
	}

	if( UpdateOutputX )
	{
		pinUpdated( mPinOutputY );
	}
}
