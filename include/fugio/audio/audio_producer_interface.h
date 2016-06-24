#ifndef INTERFACE_AUDIO_PRODUCER_H
#define INTERFACE_AUDIO_PRODUCER_H

#include <fugio/global.h>

#include "audio_sample_format.h"

FUGIO_NAMESPACE_BEGIN

class AudioProducerInterface
{
public:
	virtual ~AudioProducerInterface( void ) {}

//	virtual int channels( void ) const = 0;

//	virtual qreal sampleRate( void ) const = 0;

//	virtual AudioSampleFormat sampleFormat( void ) const = 0;

	virtual void *allocAudioInstance( qreal pSampleRate, AudioSampleFormat pSampleFormat, int pChannels ) = 0;

	virtual void freeAudioInstance( void *pInstanceData ) = 0;

	virtual void audio( qint64 pSamplePosition, qint64 pSampleCount, int pChannelOffset, int pChannelCount, float **pBuffers, qint64 pLatency, void *pInstanceData ) const = 0;
};

FUGIO_NAMESPACE_END

Q_DECLARE_INTERFACE( fugio::AudioProducerInterface, "com.bigfug.fugio.audio-producer/1.0" )

#endif // INTERFACE_AUDIO_PRODUCER_H
