#include "playheadcontrolnode.h"

#include <fugio/context_interface.h>
#include <fugio/context_signals.h>

PlayheadControlNode::PlayheadControlNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mLastTime( 0 )
{
	FUGID( PIN_INPUT_PLAY,		"4aedb550-ad09-4099-ae8d-c8d87f72b7a7" );
	FUGID( PIN_INPUT_STOP,		"28a9899a-7a8e-446a-bcc2-110c0e8f5eca" );
	FUGID( PIN_INPUT_REWIND,	"9e31a9e7-4c8a-4fac-96aa-36f9b0ebe2d0" );

	mPinInputPlay   = pinInput( "Play", PIN_INPUT_PLAY );

	mPinInputStop   = pinInput( "Stop", PIN_INPUT_STOP );

	mPinInputRewind = pinInput( "Rewind", PIN_INPUT_REWIND );
}

bool PlayheadControlNode::initialise( void )
{
	if( !NodeControlBase::initialise() )
	{
		return( false );
	}

	connect( mNode->context()->qobject(), SIGNAL(frameInitialise(qint64)), this, SLOT(contextFrameInitialise(qint64)) );

	return( true );
}

bool PlayheadControlNode::deinitialise( void )
{
	disconnect( mNode->context()->qobject(), SIGNAL(frameInitialise(qint64)), this, SLOT(contextFrameInitialise(qint64)) );

	return( NodeControlBase::deinitialise() );
}

// We're using Context::frameInitialise here instead of inputsUpdated() as
// it's the only place where we should change the playhead position

void PlayheadControlNode::contextFrameInitialise( qint64 pTimeStamp )
{
	if( mLastTime )
	{
		if( mPinInputPlay->isUpdated( mLastTime ) )
		{
			if( !mNode->context()->isPlaying() )
			{
				mNode->context()->play();
			}
		}

		if( mPinInputStop->isUpdated( mLastTime ) )
		{
			if( mNode->context()->isPlaying() )
			{
				mNode->context()->stop();
			}
		}

		if( mPinInputRewind->isUpdated( mLastTime ) )
		{
			bool	isPlaying = mNode->context()->isPlaying();

			if( isPlaying )
			{
				mNode->context()->stop();
			}

			mNode->context()->setPlayheadPosition( 0.0 );

			if( isPlaying )
			{
				mNode->context()->play();
			}
		}
	}

	mLastTime = pTimeStamp;
}

