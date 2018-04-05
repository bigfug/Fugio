#include "audiotoarraynode.h"

#include <fugio/core/uuid.h>
#include <fugio/audio/uuid.h>

#include <fugio/context_interface.h>
#include <fugio/core/variant_interface.h>

#include <fugio/context_signals.h>
#include <fugio/performance.h>
#include <fugio/pin_signals.h>

#include <QFile>
#include <QLabel>
#include <QImage>
#include <QPainter>

AudioToArrayNode::AudioToArrayNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mSampleCount( 0 ), mSamplePosition( 0 ),
	  mProducerInstance( nullptr )
{
	FUGID( PIN_INPUT_AUDIO, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_INPUT_SAMPLES, "261cc653-d7fa-4c34-a08b-3603e8ae71d5" );
	FUGID( PIN_OUTPUT_BUFFER, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );

	mPinInputAudio = pinInput( "Audio", PIN_INPUT_AUDIO );

	mValOutputBuffer = pinOutput<fugio::ArrayInterface *>( "Array", mPinOutputBuffer, PID_ARRAY, PIN_OUTPUT_BUFFER );

	mPinInputSamples = pinInput( tr( "Samples" ), PIN_INPUT_SAMPLES );

	mPinInputSamples->setValue( 2048 );

	mPinInputAudio->setDescription( tr( "An audio input" ) );

	mPinInputSamples->setDescription( tr( "The number of samples to buffer" ) );
}

void AudioToArrayNode::onContextFrame( qint64 pTimeStamp )
{
	fugio::AudioProducerInterface	*API = input<fugio::AudioProducerInterface *>( mPinInputAudio );

	if( mProducerInstance && ( !mProducerInstance->isValid() || ( API && !API->isValid( mProducerInstance ) ) ) )
	{
		delete mProducerInstance;

		mProducerInstance = nullptr;
	}

	if( !API )
	{
		if( mProducerInstance )
		{
			delete mProducerInstance;

			mProducerInstance = nullptr;

			mSamplePosition = 0;
		}

		return;
	}

	if( !mProducerInstance )
	{
		mProducerInstance = API->audioAllocInstance( 48000, fugio::AudioSampleFormat::Format32FS, 1 );
	}

	if( !mProducerInstance )
	{
		return;
	}

	const qint64	CurPos = ( mNode->context()->global()->timestamp() * 48000 ) / 1000;

	if( CurPos - mSamplePosition > 48000 )
	{
		mSamplePosition = CurPos - mSampleCount - API->audioLatency();
	}

	if( CurPos - mSamplePosition < mSampleCount )
	{
		return;
	}

	fugio::Performance	P( mNode, "onContextFrame", pTimeStamp );

	float		*AudPtr = static_cast<float *>( mValOutputBuffer->array() );

	if( AudPtr )
	{
		memset( AudPtr, 0, mValOutputBuffer->byteCount() );

		mProducerInstance->audio( mSamplePosition, mSampleCount, 0, 1, (void **)&AudPtr );

		pinUpdated( mPinOutputBuffer );
	}

	mSamplePosition += mSampleCount;
}

void AudioToArrayNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	if( mProducerInstance )
	{
		delete mProducerInstance;

		mProducerInstance = nullptr;

		mSamplePosition = 0;
	}

	int			SampleCount = variant<int>( mPinInputSamples );

	if( SampleCount > 0 && SampleCount != mSampleCount )
	{
		mSampleCount    = SampleCount;
		mSamplePosition = 0;

		mValOutputBuffer->setCount( mSampleCount );
		mValOutputBuffer->setElementCount( 1 );
		mValOutputBuffer->setStride( sizeof( float ) );
		mValOutputBuffer->setType( QMetaType::Float );
	}

	if( mPinInputAudio->isConnected() )
	{
		connect( mNode->context()->qobject(), SIGNAL(frameProcess(qint64)), this, SLOT(onContextFrame(qint64)) );
	}
	else
	{
		disconnect( mNode->context()->qobject(), SIGNAL(frameProcess(qint64)), this, SLOT(onContextFrame(qint64)) );
	}
}
