#include "audiogeneratorpin.h"

#include <fugio/node_interface.h>
#include <fugio/node_control_interface.h>

AudioGeneratorPin::AudioGeneratorPin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin )
{

}

fugio::AudioGeneratorInterface *AudioGeneratorPin::generator()
{
	if( !mGenerator && mPin->node() && mPin->node()->hasControl() )
	{
		mGenerator = qobject_cast<fugio::AudioGeneratorInterface *>( mPin->node()->control()->qobject() );
	}

	return( mGenerator );
}

const fugio::AudioGeneratorInterface *AudioGeneratorPin::generator() const
{
	return( mGenerator || !mPin->node() || !mPin->node()->hasControl() ? mGenerator : qobject_cast<fugio::AudioGeneratorInterface *>( mPin->node()->control()->qobject() ) );
}

int AudioGeneratorPin::audioChannels() const
{
	return( generator() ? generator()->audioChannels() : 0 );
}

qreal AudioGeneratorPin::audioSampleRate() const
{
	return( generator() ? generator()->audioSampleRate() : 0 );
}

fugio::AudioSampleFormat AudioGeneratorPin::audioSampleFormat() const
{
	return( generator() ? generator()->audioSampleFormat() : fugio::AudioSampleFormat::FormatUnknown );
}

bool AudioGeneratorPin::audioLock( qint64 pSamplePosition, qint64 pSampleCount, const void **pBuffers, qint64 &pReturnedPosition, qint64 &pReturnedCount )
{
	return( generator() ? generator()->audioLock( pSamplePosition, pSampleCount, pBuffers, pReturnedPosition, pReturnedCount ) : false );
}

void AudioGeneratorPin::audioUnlock( qint64 pSamplePosition, qint64 pSampleCount )
{
	if( generator() )
	{
		generator()->audioUnlock( pSamplePosition, pSampleCount );
	}
}
