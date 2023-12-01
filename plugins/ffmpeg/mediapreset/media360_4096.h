#ifndef MEDIA360_4096_H
#define MEDIA360_4096_H

#include "mediapresetmp4.h"

class Media360_4096 : public MediaPresetMp4
{
public:
	virtual ~Media360_4096( void ) {}

	static void registerPreset( MediaPresetManager &pManager )
	{
		pManager.registerPreset( "Video: 360 4096x2048", &Media360_4096::instance );
	}

	static MediaPresetInterface *instance( void )
	{
		return( new Media360_4096() );
	}

	virtual QSize videoFrameSize( void ) const
	{
		return( QSize( 4096, 2048 ) );
	}

	virtual qreal videoFramesPerSecond( void ) const
	{
		return( 30 );
	}

	virtual void fillVideoCodecContext( AVCodecContext *pContext ) const
	{
		MediaPresetMp4::fillVideoCodecContext( pContext );

		pContext->pix_fmt	= AV_PIX_FMT_YUV420P;
		pContext->width		= 4096;
		pContext->height	= 2048;
		pContext->bit_rate  = 25000 * 1000;
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

#endif // MEDIA360_4096_H
