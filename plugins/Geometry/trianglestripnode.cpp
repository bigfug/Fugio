#include "trianglestripnode.h"

#include <qmath.h>
#include <QPointF>

#include <fugio/core/uuid.h>

TriangleStripNode::TriangleStripNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_COUNT, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_OUTPUT_POINTS, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );
	FUGID( PIN_OUTPUT_UV, "45c45a72-3ed7-4c40-9261-38b5fd22c478" );

	mPinInputCount = pinInput( tr( "Count" ), PIN_INPUT_COUNT );

	mValOutputPoints = pinOutput<fugio::VariantInterface *>( tr( "Points" ), mPinOutputPoints, PID_POINT, PIN_OUTPUT_POINTS );

	mValOutputUV = pinOutput<fugio::VariantInterface *>( tr( "UV" ), mPinOutputUV, PID_POINT, PIN_OUTPUT_UV );

	mPinInputCount->setValue( 4 );
}

void TriangleStripNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	int		Count = variant<int>( mPinInputCount );

	if( Count <= 0 )
	{
		mValOutputPoints->variantClear();

		mValOutputUV->variantClear();

		return;
	}

	if( mValOutputPoints->variantCount() != Count )
	{
		mValOutputPoints->setVariantCount( Count );

		mValOutputUV->setVariantCount( Count );

		for( int i = 0 ; i < Count ; i++ )
		{
			float	X = std::floor( i / 2 );
			float	Y = 1 - ( i % 2 );

			mValOutputPoints->setVariant( i, QPointF( X, Y ) );

			float	S = X / ( float( Count ) * 0.5f );
			float	T = Y;

			mValOutputUV->setVariant( i, QPointF( S, T ) );
		}

		pinUpdated( mPinOutputPoints );
	}
}
