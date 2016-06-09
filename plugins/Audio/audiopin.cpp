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
