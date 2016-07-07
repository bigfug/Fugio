#ifndef AUDIO_SAMPLE_FORMAT_H
#define AUDIO_SAMPLE_FORMAT_H

#include <fugio/global.h>

FUGIO_NAMESPACE_BEGIN

typedef enum AudioSampleFormat
{
	FormatUnknown = -1,

	// Interlevered formats (single buffer)

	Format8UI,		// 8 bit, unsigned
	Format8SI,		// 8 bit, signed

	Format16UI,		// 16 bit, unsigned
	Format16SI,		// 16 bit, signed

	Format32FI,		// 32 bit, floating point
	Format64FI,		// 64 bit, floating point

	// Split formats (one buffer per channel)

	Format8US,		// 8 bit, unsigned
	Format8SS,		// 8 bit, signed

	Format16US,		// 16 bit, unsigned
	Format16SS,		// 16 bit, signed

	Format32FS,		// 32 bit, floating point
	Format64FS,		// 64 bit, floating point

} AudioSampleFormat;

FUGIO_NAMESPACE_END

#endif // AUDIO_SAMPLE_FORMAT_H
