#include "playheadframesnode.h"

#include <fugio/context_interface.h>
#include <fugio/context_signals.h>

#include <fugio/core/uuid.h>

PlayheadFramesNode::PlayheadFramesNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mLastTime( 0 ), mFrameNumber( 0 )
{
	FUGID( PIN_INPUT_FPS,			"4aedb550-ad09-4099-ae8d-c8d87f72b7a7" );
	FUGID( PIN_INPUT_FRAME_NUMBER,	"9e31a9e7-4c8a-4fac-96aa-36f9b0ebe2d0" );
	FUGID( PIN_INPUT_REWIND,		"0E6FE6B0-F8C5-4C71-BFF7-E8E15059A0E3" );
	FUGID( PIN_OUTPUT_FRAME_NUMBER,	"28a9899a-7a8e-446a-bcc2-110c0e8f5eca" );

	mPinInputTrigger		= pinInput( "Trigger", PID_FUGIO_NODE_TRIGGER );
	mPinInputFPS			= pinInput( "FPS", PIN_INPUT_FPS );
	mPinInputFrameNumber	= pinInput( "Frame Number", PIN_INPUT_FRAME_NUMBER );
	mPinInputRewind			= pinInput( "Rewind", PIN_INPUT_REWIND );

	mValOutputFrameNumber = pinOutput<fugio::VariantInterface *>( "Frame Number", mPinOutputFrameNumber, PID_INTEGER, PIN_OUTPUT_FRAME_NUMBER );

	mPinInputFPS->setValue( 25.0 );
}

// We're using Context::frameInitialise here instead of inputsUpdated() as
// it's the only place where we should change the playhead position

void PlayheadFramesNode::contextFrameInitialise( void )
{
	qreal		CurFPS  = variant( mPinInputFPS ).toReal();

	if( CurFPS > 0 )
	{
		qreal		NewTme = qreal( mFrameNumber ) * ( 1.0 / CurFPS );

		if( mNode->context()->position() != NewTme )
		{
			mNode->context()->setPlayheadPosition( NewTme );
		}
	}

	mValOutputFrameNumber->setVariant( mFrameNumber );

	pinUpdated( mPinOutputFrameNumber );

	disconnect( mNode->context()->qobject(), SIGNAL(frameInitialise()), this, SLOT(contextFrameInitialise()) );
}

void PlayheadFramesNode::inputsUpdated( qint64 pTimeStamp )
{
	qint32		NewFrm = mFrameNumber;

	if( mPinInputFrameNumber->isUpdated( pTimeStamp ) )
	{
		NewFrm = variant( mPinInputFrameNumber ).toInt();
	}

	if( pTimeStamp && mPinInputRewind->isUpdated( pTimeStamp ) )
	{
		NewFrm = 0;
	}

	if( pTimeStamp && mPinInputTrigger->isUpdated( pTimeStamp ) )
	{
		NewFrm++;
	}

	if( NewFrm != mFrameNumber )
	{
		mFrameNumber = NewFrm;

		connect( mNode->context()->qobject(), SIGNAL(frameInitialise()), this, SLOT(contextFrameInitialise()) );
	}
}
