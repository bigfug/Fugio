#ifndef MEDIAYOUTUBE360PWIDE_H
#define MEDIAYOUTUBE360PWIDE_H

#include "mediapresetmp4.h"

class MediaYouTube360pWide : public MediaPresetMp4
{
public:
	virtual ~MediaYouTube360pWide( void ) {}

	static void registerPreset( MediaPresetManager &pManager )
	{
		pManager.registerPreset( "Video: YouTube 360p 16:9", &MediaYouTube360pWide::instance );
	}

	static MediaPresetInterface *instance( void )
	{
		return( new MediaYouTube360pWide() );
	}

	virtual QSize videoFrameSize( void ) const
	{
		return( QSize( 640, 360 ) );
	}

	virtual qreal videoFramesPerSecond( void ) const
	{
		return( 25 );
	}

	virtual void fillVideoCodecContext( AVCodecContext *pContext ) const
	{
		MediaPresetMp4::fillVideoCodecContext( pContext );

		pContext->pix_fmt	= AV_PIX_FMT_YUV420P;
		pContext->width		= 640;
		pContext->height	= 360;
		pContext->bit_rate  = 1000 * 1000;
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

#endif // MEDIAYOUTUBE360PWIDE_H
