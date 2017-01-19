#ifndef MEDIAWAV44_H
#define MEDIAWAV44_H

#include "interfacemediapreset.h"
#include "mediapresetmanager.h"

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
	#include <libavutil/imgutils.h>
}

class MediaWav44 : public InterfaceMediaPreset
{
public:
	virtual ~MediaWav44( void ) {}

	static void registerPreset( MediaPresetManager &pManager )
	{
		pManager.registerPreset( "Audio: WAV 44khz", &MediaWav44::instance );
	}

	static InterfaceMediaPreset *instance( void )
	{
		return( new MediaWav44() );
	}

	virtual QString fileExt( void ) const
	{
		return( "wav" );
	}

	virtual QString filter( void ) const
	{
		return( QObject::tr( "WAV Audio (*.wav)" ) );
	}

	virtual qreal videoFramesPerSecond( void ) const
	{
		return( 0 );
	}

	virtual void fillVideoCodecContext( AVCodecContext *pContext ) const
	{
		Q_UNUSED( pContext )
	}

	virtual qreal audioSampleRate( void ) const
	{
		return( 44100 );
	}

	virtual void fillAudioCodecContext( AVCodecContext *pContext ) const
	{
		pContext->sample_rate    = audioSampleRate();
		pContext->channel_layout = AV_CH_LAYOUT_STEREO;
		pContext->channels       = av_get_channel_layout_nb_channels( pContext->channel_layout );
		pContext->sample_fmt     = AV_SAMPLE_FMT_S16;
	}

	virtual bool hasVideo( void ) const
	{
		return( false );
	}

	virtual bool hasAudio( void ) const
	{
		return( true );
	}

	virtual void setQuality( AVCodecContext *pContext, AVStream *pVideoStream, qreal pQuality ) const
	{
		Q_UNUSED( pContext )
		Q_UNUSED( pVideoStream )
		Q_UNUSED( pQuality );
	}

	virtual void setSpeed( AVCodecContext *pContext, AVStream *pVideoStream, qreal pSpeed ) const
	{
		Q_UNUSED( pContext )
		Q_UNUSED( pVideoStream )
		Q_UNUSED( pSpeed );
	}
};

#endif // MEDIAWAV44_H
