#ifndef MEDIAPROXY_H
#define MEDIAPROXY_H

#include "mediapresetmp4.h"

class MediaProxy : public MediaPresetMp4
{
public:
	virtual ~MediaProxy( void ) {}

	static void registerPreset( MediaPresetManager &pManager )
	{
		pManager.registerPreset( "media-proxy", &MediaProxy::instance );
	}

	static MediaPresetInterface *instance( void )
	{
		return( new MediaProxy() );
	}

	virtual qreal videoFramesPerSecond( void ) const
	{
		return( 25 );
	}

	virtual void fillVideoCodecContext( AVCodecContext *pContext ) const
	{
		MediaPresetMp4::fillVideoCodecContext( pContext );

		pContext->pix_fmt	= AV_PIX_FMT_YUV420P;
		pContext->width		= 240;
		pContext->height	= 180;
		pContext->bit_rate  = 1000 * 1000;
		pContext->bit_rate_tolerance = 0;

		av_opt_set( pContext->priv_data, "preset", "ultrafast", AV_OPT_SEARCH_CHILDREN );
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
		pContext->sample_fmt     = AV_SAMPLE_FMT_S16;
		pContext->bit_rate       = 128 * 1000;
		pContext->bit_rate_tolerance = 0;
	}
};

#endif // MEDIAPROXY_H
