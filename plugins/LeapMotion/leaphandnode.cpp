#include "leaphandnode.h"

#include "deviceleap.h"
#include "leapmotionplugin.h"

#include <fugio/core/uuid.h>
#include <fugio/math/uuid.h>
#include <fugio/node_interface.h>
#include <fugio/context_interface.h>
#include <fugio/context_signals.h>
#include <fugio/performance.h>
#include <fugio/leapmotion/hand_interface.h>

LeapHandNode::LeapHandNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	mPinHand = pinInput( "Hand" );

	mOutputStabilizedPalmPosition = pinOutput<fugio::VariantInterface *>( "Stable Palm Position", mOutputStabilizedPalmPositionPin, PID_VECTOR3 );

	mNormal = pinOutput<fugio::VariantInterface *>( "Palm Normal", mPinNormal, PID_VECTOR3 );

	mDirection = pinOutput<fugio::VariantInterface *>( "Palm Direction", mPinDirection, PID_VECTOR3 );

	mPinchStrength = pinOutput<fugio::VariantInterface *>( "Pinch Strength", mPinPinchStrength, PID_FLOAT );

	mGrabStrength = pinOutput<fugio::VariantInterface *>( "Grab Strength", mPinGrabStrength, PID_FLOAT );
}

void LeapHandNode::inputsUpdated( qint64 pTimeStamp )
{
	if( !pTimeStamp )
	{
		return;
	}

#if defined( LEAP_PLUGIN_SUPPORTED )
	fugio::HandInterface		*HandData = input<fugio::HandInterface *>( mPinHand );

	if( !HandData )
	{
		return;
	}

	const Leap::Hand		&Hand = HandData->hand();

	float			F;
	QVector3D		V3;

	if( ( V3 = LeapMotionPlugin::leapToVec3( Hand.stabilizedPalmPosition() ) ) != mOutputStabilizedPalmPosition->variant().value<QVector3D>() )
	{
		mOutputStabilizedPalmPosition->setVariant( V3 );

		pinUpdated( mOutputStabilizedPalmPositionPin );
	}

	if( ( V3 = LeapMotionPlugin::leapToVec3( Hand.palmNormal() ) ) != mNormal->variant().value<QVector3D>() )
	{
		mNormal->setVariant( V3 );

		pinUpdated( mPinNormal );
	}

	if( ( V3 = LeapMotionPlugin::leapToVec3( Hand.direction() ) ) != mDirection->variant().value<QVector3D>() )
	{
		mDirection->setVariant( V3 );

		pinUpdated( mPinDirection );
	}

	if( ( F = Hand.pinchStrength() ) != mPinchStrength->variant().toFloat() )
	{
		mPinchStrength->setVariant( F );

		pinUpdated( mPinPinchStrength );
	}

	if( ( F = Hand.grabStrength() ) != mGrabStrength->variant().toFloat() )
	{
		mGrabStrength->setVariant( F );

		pinUpdated( mPinGrabStrength );
	}
#endif
}
