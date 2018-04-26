#include "joinpointnode.h"

#include <fugio/core/uuid.h>

#include <QPointF>

#include <fugio/pin_variant_iterator.h>

JoinPointNode::JoinPointNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_X, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_INPUT_Y, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );
	FUGID( PIN_OUTPUT_POINT, "261cc653-d7fa-4c34-a08b-3603e8ae71d5" );

	mPinInputX = pinInput( "X", PIN_INPUT_X );
	mPinInputY = pinInput( "Y", PIN_INPUT_Y );

	mValOutputPoint = pinOutput<fugio::VariantInterface *>( tr( "Point" ), mPinOutputPoint, PID_POINT, PIN_OUTPUT_POINT );
}

void JoinPointNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	bool	UpdateOutput = false;

	fugio::PinVariantIterator	X( mPinInputX );
	fugio::PinVariantIterator	Y( mPinInputY );

	QVector<int>	PinCnt = { X.count(), Y.count() };

	auto			MinMax = std::minmax_element( PinCnt.begin(), PinCnt.end() );

	if( !*MinMax.first )
	{
		return;
	}

	const int		OutCnt = *MinMax.second;

	variantSetCount( mValOutputPoint, OutCnt, UpdateOutput );

	for( int i = 0 ; i < OutCnt ; i++ )
	{
		qreal		x = X.index( i ).toReal();
		qreal		y = Y.index( i ).toReal();

		variantSetValue( mValOutputPoint, i, QPointF( x, y ), UpdateOutput );
	}

	if( UpdateOutput )
	{
		pinUpdated( mPinOutputPoint );
	}
}
