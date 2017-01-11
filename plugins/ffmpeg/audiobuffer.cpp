#include "audiobuffer.h"

#ifndef INT64_C
#define INT64_C(c) (c ## LL)
#define UINT64_C(c) (c ## ULL)
#endif

#if defined( FFMPEG_SUPPORTED )
extern "C"
{
	#include <libavutil/rational.h>
	#include <libavcodec/avcodec.h>
	#include <libavformat/avformat.h>
	#include <libavutil/opt.h>
	#include <libswscale/swscale.h>
}
#endif

AudioBuffer::AudioBuffer()
	: mAudPts( 0 ), mAudSmp( 0 ), mAudLen( 0 ), mForce( false )
{
}

AudioBuffer::~AudioBuffer()
{
	if( !mAudDat.isEmpty() )
	{
#if defined( FFMPEG_SUPPORTED )
		av_freep( &mAudDat[ 0 ] );
#endif
	}
}

void AudioBuffer::clear()
{
	mAudDat.clear();
}

