#include "leapnode.h"

#include "deviceleap.h"

#include <fugio/core/uuid.h>
#include <fugio/math/uuid.h>
#include <fugio/image/uuid.h>

#include <fugio/node_interface.h>
#include <fugio/context_interface.h>
#include <fugio/context_signals.h>
#include <fugio/performance.h>

LeapNode::LeapNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mDevice( nullptr ), mLastFrameId( 0 )
{
	mHandLeft  = pinOutput<LeapHandPin *>( tr( "Left Hand" ), mPinHandLeft, PID_LEAP_HAND );

	mHandRight = pinOutput<LeapHandPin *>( tr( "Right Hand" ), mPinHandRight, PID_LEAP_HAND );

	mImage1 = pinOutput<fugio::ImageInterface *>( "Image 1", mPinImage1, PID_IMAGE );

	mDistortionImage = pinOutput<fugio::ImageInterface *>( "Distortion", mPinDistortionImage, PID_IMAGE );
}

bool LeapNode::initialise( void )
{
	if( !NodeControlBase::initialise() )
	{
		return( false );
	}

	if( ( mDevice = DeviceLeap::newDevice() ) == 0 )
	{
		return( false );
	}

	connect( mNode->context()->qobject(), SIGNAL(frameStart(qint64)), this, SLOT(onContextFrameStart(qint64)) );

	return( true );
}

bool LeapNode::deinitialise( void )
{
	if( mDevice != 0 )
	{
		DeviceLeap::delDevice( mDevice );

		mDevice = 0;
	}

	return( NodeControlBase::deinitialise() );
}

void LeapNode::onContextFrameStart( qint64 pTimeStamp )
{
	fugio::Performance	Perf( mNode, "frameStart", pTimeStamp );

#if defined( LEAP_PLUGIN_SUPPORTED )
    if( !mDevice || !mDevice->controller().isConnected() || mDevice->frameId() == mLastFrameId )
	{
		return;
	}

	Leap::Frame			 CurrentFrame = mDevice->frame();

	//qDebug() << mDevice->controller().frame().id();

	if( mPinImage1->isConnectedToActiveNode() )
	{
		Leap::ImageList		 CurrentImages = CurrentFrame.images();

		if( CurrentImages.count() > 0 )
		{
			Image CurrentImage = CurrentImages[ 0 ];

			if( mImage1->size() != QSize( CurrentImage.width(), CurrentImage.height() ) )
			{
				mImage1->setFormat( fugio::ImageInterface::FORMAT_GRAY8 );
				mImage1->setSize( CurrentImage.width(), CurrentImage.height() );
				mImage1->setLineSize( 0, CurrentImage.width() * CurrentImage.bytesPerPixel() );
			}

			if( !mImage1->size().isEmpty() )
			{
				memcpy( mImage1->internalBuffer( 0 ), CurrentImage.data(), mImage1->bufferSize( 0 ) );

				pinUpdated( mPinImage1 );
			}

			if( mDistortionImage->size() != QSize( CurrentImage.distortionWidth() / 2, CurrentImage.distortionHeight() ) )
			{
				mDistortionImage->setFormat( fugio::ImageInterface::FORMAT_RG32 );

				mDistortionImage->setSize( CurrentImage.distortionWidth() / 2, CurrentImage.distortionHeight() );

				mDistortionImage->setLineSize( 0, CurrentImage.distortionWidth() * sizeof( float ) );

				memcpy( mDistortionImage->internalBuffer( 0 ), CurrentImage.distortion(), mDistortionImage->bufferSize( 0 ) );

				pinUpdated( mPinDistortionImage );
			}
		}
	}

	if( mHandLeft )
	{
		mHandLeft->setHand( mDevice->handLeft() );

		pinUpdated( mPinHandLeft );
	}

	if( mHandRight )
	{
		mHandRight->setHand( mDevice->handRight() );

		pinUpdated( mPinHandRight );
	}

	mLastFrameId = CurrentFrame.id();
#endif
}
