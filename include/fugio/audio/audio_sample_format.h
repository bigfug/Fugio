#ifndef AUDIO_SAMPLE_FORMAT_H
#define AUDIO_SAMPLE_FORMAT_H

#include <fugio/global.h>

FUGIO_NAMESPACE_BEGIN

typedef enum AudioSampleFormat
{
	FMT_UNKNOWN,
	FMT_16BIT,
	FMT_FLT_S
} AudioSampleFormat;

FUGIO_NAMESPACE_END

#endif // AUDIO_SAMPLE_FORMAT_H
