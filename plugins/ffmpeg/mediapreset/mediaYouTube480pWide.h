#ifndef MEDIAYOUTUBE480PWIDE_H
#define MEDIAYOUTUBE480PWIDE_H

#include "mediapresetmp4.h"

class MediaYouTube480pWide : public MediaPresetMp4
{
public:
	virtual ~MediaYouTube480pWide( void ) {}

	static void registerPreset( MediaPresetManager &pManager )
	{
		pManager.registerPreset( "Video: YouTube 480p 16:9", &MediaYouTube480pWide::instance );
	}

	static MediaPresetInterface *instance( void )
	{
		return( new MediaYouTube480pWide() );
	}

	virtual QSize videoFrameSize( void ) const
	{
		return( QSize( 854, 480 ) );
	}

	virtual qreal videoFramesPerSecond( void ) const
	{
		return( 25 );
	}

	virtual void fillVideoCodecContext( AVCodecContext *pContext ) const
	{
		MediaPresetMp4::fillVideoCodecContext( pContext );

		pContext->pix_fmt	= AV_PIX_FMT_YUV420P;
		pContext->width		= 854;
		pContext->height	= 480;
		pContext->bit_rate  = 2500 * 1000;
		pContext->bit_rate_tolerance = 0;
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

#endif // MEDIAYOUTUBE480PWIDE_H
