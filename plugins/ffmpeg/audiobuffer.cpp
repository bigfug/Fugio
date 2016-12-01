#include "audiobuffer.h"

#ifndef INT64_C
#define INT64_C(c) (c ## LL)
#define UINT64_C(c) (c ## ULL)
#endif

extern "C"
{
	#include <libavutil/rational.h>
	#include <libavcodec/avcodec.h>
	#include <libavformat/avformat.h>
	#include <libavutil/opt.h>
	#include <libswscale/swscale.h>
}

AudioBuffer::AudioBuffer()
	: mAudPts( 0 ), mAudSmp( 0 ), mAudLen( 0 ), mForce( false )
{
}

AudioBuffer::~AudioBuffer()
{
	if( !mAudDat.isEmpty() )
	{
		av_freep( &mAudDat[ 0 ] );
	}
}

void AudioBuffer::clear()
{
	mAudDat.clear();
}

