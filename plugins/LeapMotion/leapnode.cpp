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

	mImage1 = pinOutput<fugio::VariantInterface *>( "Image 1", mPinImage1, PID_IMAGE );

	mDistortionImage = pinOutput<fugio::VariantInterface *>( "Distortion", mPinDistortionImage, PID_IMAGE );
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

			fugio::Image	Image1 = mImage1->variant().value<fugio::Image>();

			if( Image1.size() != QSize( CurrentImage.width(), CurrentImage.height() ) )
			{
				Image1.setFormat( fugio::ImageFormat::GRAY8 );
				Image1.setSize( CurrentImage.width(), CurrentImage.height() );
				Image1.setLineSize( 0, CurrentImage.width() * CurrentImage.bytesPerPixel() );
			}

			if( Image1.isValid() )
			{
				memcpy( Image1.internalBuffer( 0 ), CurrentImage.data(), Image1.bufferSize( 0 ) );

				pinUpdated( mPinImage1 );
			}

			fugio::Image	DstImg = mDistortionImage->variant().value<fugio::Image>();

			if( DstImg.size() != QSize( CurrentImage.distortionWidth() / 2, CurrentImage.distortionHeight() ) )
			{
				DstImg.setFormat( fugio::ImageFormat::RG32 );

				DstImg.setSize( CurrentImage.distortionWidth() / 2, CurrentImage.distortionHeight() );

				DstImg.setLineSize( 0, CurrentImage.distortionWidth() * sizeof( float ) );

				memcpy( DstImg.internalBuffer( 0 ), CurrentImage.distortion(), DstImg.bufferSize( 0 ) );

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
