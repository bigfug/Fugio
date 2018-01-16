#include "polygonnode.h"

#include <qmath.h>
#include <QPointF>

#include <fugio/core/uuid.h>

PolygonNode::PolygonNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mRadius( std::sqrt( 2.0f ) )
{
	FUGID( PIN_INPUT_SIDES, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_INPUT_RADIUS, "d77f1053-0f88-4b4a-bfc6-57be3bb1eddd" );
	FUGID( PIN_OUTPUT_POINTS, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );

	mPinInputSides = pinInput( tr( "Sides" ), PIN_INPUT_SIDES );

	mPinInputRadius = pinInput( tr( "Radius" ), PIN_INPUT_RADIUS );

	mValOutputPoints = pinOutput<fugio::VariantInterface *>( tr( "Points" ), mPinOutputPoints, PID_POINT, PIN_OUTPUT_POINTS );

	mPinInputSides->setValue( 3 );
	mPinInputRadius->setValue( mRadius );
}

void PolygonNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	int		Sides = variant( mPinInputSides ).toInt();

	if( Sides < 2 )
	{
		return;
	}

	float	Radius = variant( mPinInputRadius ).toFloat();

	if( Radius <= 0 )
	{
		Radius = std::sqrt( 2.0f );
	}

	if( mValOutputPoints->variantCount() != Sides || mRadius != Radius )
	{
		mValOutputPoints->setVariantCount( Sides );

		const bool		 SidesEven = !( Sides % 2 );
		const float		 Div = ( 2.0f * M_PI ) / float( Sides );
		const float		 Offset = ( SidesEven ? Div * 0.5f : 0.0f );

		for( int i = 0 ; i < Sides ; i++ )
		{
			const float	A = ( float( i ) * Div ) + Offset;

			mValOutputPoints->setVariant( i, QPointF( std::sin( A ) * Radius, std::cos( A ) * Radius ) );
		}

		mRadius = Radius;

		pinUpdated( mPinOutputPoints );
	}
}
