#ifndef MEDIA360_2048_H
#define MEDIA360_2048_H

#include "mediapresetmp4.h"

class Media360_2048 : public MediaPresetMp4
{
public:
	virtual ~Media360_2048( void ) {}

	static void registerPreset( MediaPresetManager &pManager )
	{
		pManager.registerPreset( "Video: 360 2048x1024", &Media360_2048::instance );
	}

	static MediaPresetInterface *instance( void )
	{
		return( new Media360_2048() );
	}

	virtual QSize videoFrameSize( void ) const
	{
		return( QSize( 2048, 1024 ) );
	}

	virtual qreal videoFramesPerSecond( void ) const
	{
		return( 25 );
	}

	virtual void fillVideoCodecContext( AVCodecContext *pContext ) const
	{
		MediaPresetMp4::fillVideoCodecContext( pContext );

		pContext->pix_fmt	= AV_PIX_FMT_YUV420P;
		pContext->width		= 2048;
		pContext->height	= 1024;
		pContext->bit_rate  = 8000 * 1000;
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
		pContext->bit_rate       = 384 * 1000;
		pContext->bit_rate_tolerance = 0;
	}
};

#endif // MEDIA360_2048_H
