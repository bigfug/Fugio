#ifndef INTERFACEMEDIAPRESETMP4_H
#define INTERFACEMEDIAPRESETMP4_H

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

#if defined( TL_USE_LIB_AV )
#else
#endif
}

class MediaPresetMp4 : public MediaPresetInterface
{
public:
	virtual ~MediaPresetMp4( void ) {}

	virtual void fillVideoCodecContext( AVCodecContext *pContext ) const
	{
		pContext->codec_id  = AV_CODEC_ID_H264;
		pContext->profile   = FF_PROFILE_H264_BASELINE;
	}

	virtual void fillAudioCodecContext( AVCodecContext *pContext ) const
	{
		pContext->codec_id = AV_CODEC_ID_AAC;
	}

	virtual QString fileExt( void ) const
	{
		return( "mp4" );
	}

	virtual QString filter( void ) const
	{
		return( QObject::tr( "MP4 Video (*.mp4)" ) );
	}

	virtual bool hasVideo( void ) const
	{
		return( true );
	}

	virtual bool hasAudio( void ) const
	{
		return( false );
	}

	virtual void setQuality( AVCodecContext *pContext, AVStream *pVideoStream, qreal pQuality ) const
	{
		Q_UNUSED( pVideoStream )

		int		CRF = int( ( 1.0 - qBound<qreal>( 0.0, pQuality, 1.0 ) ) * 50.0 ) + 1;
		int		ERR;

		if( ( ERR = av_opt_set( pContext->priv_data, "crf", QString::number( CRF ).toLatin1(), 0 ) ) != 0 )
		{
			Q_ASSERT( ERR == 0 );
		}
	}

	virtual void setSpeed( AVCodecContext *pContext, AVStream *pVideoStream, qreal pSpeed ) const
	{
		static const char *const x264_preset_names[] = { "ultrafast", "superfast", "veryfast", "faster", "fast", "medium", "slow", "slower", "veryslow" };

		Q_UNUSED( pVideoStream )

		const char	*OPT = x264_preset_names[ int( ( 1.0 - qBound<qreal>( 0.0, pSpeed, 1.0 ) ) * 7 ) ];
		int			 ERR;

		if( ( ERR = av_opt_set( pContext->priv_data, "preset", OPT, AV_OPT_SEARCH_CHILDREN ) ) != 0 )
		{
			Q_ASSERT( ERR == 0 );
		}
	}
};

#endif // INTERFACEMEDIAPRESETMP4_H
