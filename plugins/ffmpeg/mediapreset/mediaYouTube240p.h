#ifndef MEDIAYOUTUBE240P_H
#define MEDIAYOUTUBE240P_H

#include "mediapresetmp4.h"

class MediaYouTube240p : public MediaPresetMp4
{
public:
	virtual ~MediaYouTube240p( void ) {}

	static void registerPreset( MediaPresetManager &pManager )
	{
		pManager.registerPreset( "Video: YouTube 240p 4:3", &MediaYouTube240p::instance );
	}

	static MediaPresetInterface *instance( void )
	{
		return( new MediaYouTube240p() );
	}

	virtual QSize videoFrameSize( void ) const
	{
		return( QSize( 320, 240 ) );
	}

	virtual qreal videoFramesPerSecond( void ) const
	{
		return( 25 );
	}

	virtual void fillVideoCodecContext( AVCodecContext *pContext ) const
	{
		MediaPresetMp4::fillVideoCodecContext( pContext );

		pContext->pix_fmt	= AV_PIX_FMT_YUV420P;
		pContext->width		= 320;
		pContext->height	= 240;
		pContext->bit_rate  = 1000 * 1000;
		pContext->bit_rate_tolerance = 0;
		pContext->sample_aspect_ratio.den = 1;
		pContext->sample_aspect_ratio.num = 1;
//		pContext->gop_size = 10;
//		pContext->max_b_frames = 1;
	}

	virtual qreal audioSampleRate( void ) const
	{
		return( 48000 );
	}

	virtual void fillAudioCodecContext( AVCodecContext *pContext ) const
	{
		MediaPresetMp4::fillAudioCodecContext( pContext );

		pContext->sample_rate    = audioSampleRate();
		pContext->channel_layout = AV_CH_LAYOUT_STEREO;
		pContext->channels       = av_get_channel_layout_nb_channels( pContext->channel_layout );
		pContext->sample_fmt     = AV_SAMPLE_FMT_FLTP;
		pContext->bit_rate       = 128 * 1000;
		pContext->bit_rate_tolerance = 0;
	}
};

#endif // MEDIAYOUTUBE240P_H
