#ifndef MEDIADVDPAL_H
#define MEDIADVDPAL_H

#include "mediapresetmp4.h"

class MediaDvdPal : public MediaPresetMp4
{
public:
	virtual ~MediaDvdPal( void ) {}

	static void registerPreset( MediaPresetManager &pManager )
	{
		pManager.registerPreset( "Video: DVD PAL", &MediaDvdPal::instance );
	}

	static InterfaceMediaPreset *instance( void )
	{
		return( new MediaDvdPal() );
	}

	virtual qreal videoFramesPerSecond( void ) const
	{
		return( 25 );
	}

	virtual void fillVideoCodecContext( AVCodecContext *pContext ) const
	{
		MediaPresetMp4::fillVideoCodecContext( pContext );

		pContext->pix_fmt	= AV_PIX_FMT_YUV420P;
		pContext->width		= 720;
		pContext->height	= 576;
		pContext->bit_rate  = 6000000;
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
		pContext->sample_fmt     = AV_SAMPLE_FMT_S16;
		pContext->bit_rate       = 384 * 1000;
		pContext->bit_rate_tolerance = 0;
	}
};

#endif // MEDIADVDPAL_H

