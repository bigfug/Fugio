#ifndef INTERFACE_AUDIO_PRODUCER_H
#define INTERFACE_AUDIO_PRODUCER_H

#include <fugio/global.h>

#include "audio_sample_format.h"

FUGIO_NAMESPACE_BEGIN

class AudioProducerInterface
{
public:
	virtual ~AudioProducerInterface( void ) {}

	virtual int audioChannels( void ) const = 0;

	virtual qreal audioSampleRate( void ) const = 0;

	virtual AudioSampleFormat audioSampleFormat( void ) const = 0;

	virtual qint64 audioLatency( void ) const = 0;

	virtual void *audioAllocInstance( qreal pSampleRate, AudioSampleFormat pSampleFormat, int pChannels ) = 0;

	virtual void audioFreeInstance( void *pInstanceData ) = 0;

	virtual void audio( qint64 pSamplePosition, qint64 pSampleCount, int pChannelOffset, int pChannelCount, void **pBuffers, void *pInstanceData ) const = 0;
};

FUGIO_NAMESPACE_END

Q_DECLARE_INTERFACE( fugio::AudioProducerInterface, "com.bigfug.fugio.audio-producer/1.0" )

#endif // INTERFACE_AUDIO_PRODUCER_H
