#ifndef AUDIO_GENERATOR_INTERFACE_H
#define AUDIO_GENERATOR_INTERFACE_H

#include <fugio/global.h>

#include "audio_sample_format.h"

FUGIO_NAMESPACE_BEGIN

class AudioGeneratorInterface
{
public:
	virtual ~AudioGeneratorInterface( void ) {}

	virtual int audioChannels( void ) const = 0;

	virtual qreal audioSampleRate( void ) const = 0;

	virtual AudioSampleFormat audioSampleFormat( void ) const = 0;

	virtual bool audioLock( qint64 pSamplePosition, qint64 pSampleCount, const void **pBuffers, qint64 &pReturnedPosition, qint64 &pReturnedCount ) = 0;

	virtual void audioUnlock( qint64 pSamplePosition, qint64 pSampleCount ) = 0;
};

FUGIO_NAMESPACE_END

Q_DECLARE_INTERFACE( fugio::AudioGeneratorInterface, "com.bigfug.fugio.audio.generator/1.0" )

#endif // AUDIO_GENERATOR_INTERFACE_H
