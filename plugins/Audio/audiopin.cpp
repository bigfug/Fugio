#include "audiopin.h"

#include <fugio/node_interface.h>
#include <fugio/node_control_interface.h>
#include <fugio/audio/audio_producer_interface.h>
#include <fugio/node_signals.h>

AudioPin::AudioPin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin ), mProducer( nullptr )
{
}

AudioPin::~AudioPin()
{
}

fugio::AudioProducerInterface *AudioPin::producer()
{
	if( !mProducer && mPin->node() && mPin->node()->hasControl() )
	{
		mProducer = qobject_cast<fugio::AudioProducerInterface *>( mPin->node()->control()->qobject() );
	}

	return( mProducer );
}

const fugio::AudioProducerInterface *AudioPin::producer() const
{
	return( mProducer || !mPin->node() || !mPin->node()->hasControl() ? mProducer : qobject_cast<fugio::AudioProducerInterface *>( mPin->node()->control()->qobject() ) );
}


int AudioPin::audioChannels() const
{
	return( producer() ? producer()->audioChannels() : 0 );
}

qreal AudioPin::audioSampleRate() const
{
	return( producer() ? producer()->audioSampleRate() : 0 );
}

fugio::AudioSampleFormat AudioPin::audioSampleFormat() const
{
	return( producer() ? producer()->audioSampleFormat() : fugio::AudioSampleFormat::FormatUnknown );
}

qint64 AudioPin::audioLatency() const
{
	return( producer() ? producer()->audioLatency() : 0 );
}
