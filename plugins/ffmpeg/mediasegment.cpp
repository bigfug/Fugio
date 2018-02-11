#include "mediasegment.h"

#include <QProgressDialog>
#include <QImage>
#include <QFileInfo>
#include <QApplication>
#include <QUrl>
#include <QtConcurrent/QtConcurrentRun>

#include <qmath.h>

#include <QDebug>

#include "mediaaudioprocessor.h"

#include <fugio/performance.h>

//#define DEBUG_SEEK
//#define DEBUG_HAVE_FRAMES
//#define DEBUG_VIDEO_STORE
//#define DEBUG_AUDIO_STORE
//#define DEBUG_MIX_AUDIO

#define VIDEO_PTS_PLUS		(10.0)
#define VIDEO_PTS_MINUS		(0.25)
#define AUDIO_PTS_PLUS		(10.0)
#define AUDIO_PTS_MINUS		(2.5)

MediaSegment::MediaSegment( void )
{
	clearSegment();

#if defined( FFMPEG_SUPPORTED )
	if( ( mAudioProcessor = new MediaAudioProcessor() ) != 0 )
	{
	}
#endif
}

MediaSegment::~MediaSegment( void )
{
	unloadMedia();
}

void MediaSegment::clearSegment()
{
#if defined( FFMPEG_SUPPORTED )
	mVideo = VideoStream();
	mAudio = AudioStream();

	mPlayHead = 0;
	mFormatContext = 0;
	mSwrContext = 0;
	mErrorState = false;
	mDuration = 0;
	mDecodeI = true;
	mDecodeB = true;

	mStatusMessage.clear();
#else
	mStatusMessage = tr( "FFMPEG is not supported" );
#endif
}

bool MediaSegment::loadMedia( const QString &pFileName, bool pProcess )
{
	mFileName = pFileName;

	clearSegment();

	mStatusMessage = tr( "FFMPEG loading %1" ).arg( mFileName );

#if defined( FFMPEG_SUPPORTED )
	mFormatContext = avformat_alloc_context();

	mFormatContext->interrupt_callback.opaque   = &mTimeoutHandler;
	mFormatContext->interrupt_callback.callback = &timeout_handler::check_interrupt;

	mTimeoutHandler.reset( 10000 );

	//-------------------------------------------------------------------------

	int		ErrorCode = 0;

	if( ( ErrorCode = avformat_open_input( &mFormatContext, mFileName.toLatin1().constData(), NULL, NULL ) ) != 0 )
	{
		mStatusMessage = av_err( tr( "Couldn't avformat_open_input %1" ).arg( mFileName ), ErrorCode );

		mErrorState = true;

		return( false );
	}

	av_format_inject_global_side_data( mFormatContext );

	if( ( ErrorCode = avformat_find_stream_info( mFormatContext, 0 ) ) != 0 )
	{
		mStatusMessage = av_err( tr( "Couldn't avformat_find_stream_info %1" ).arg( mFileName ), ErrorCode );

		mErrorState = true;

		return( false );
	}

	//-------------------------------------------------------------------------

	mVideo.mStreamId = av_find_best_stream( mFormatContext, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0 );
	mAudio.mStreamId = av_find_best_stream( mFormatContext, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0 );
	mSubtitle.mStreamId = av_find_best_stream( mFormatContext, AVMEDIA_TYPE_SUBTITLE, -1, -1, NULL, 0 );

	// av_find_best_stream might return an error less than -1

	mVideo.mStreamId = qMax( -1, mVideo.mStreamId );
	mAudio.mStreamId = qMax( -1, mAudio.mStreamId );
	mSubtitle.mStreamId = qMax( -1, mSubtitle.mStreamId );

	for( int i = 0 ; i < (int)mFormatContext->nb_streams ; i++ )
	{
		AVCodecParameters		*CodPar = mFormatContext->streams[ i ]->codecpar;

		if( mVideo.mStreamId < 0 && CodPar->codec_type == AVMEDIA_TYPE_VIDEO )
		{
			mVideo.mStreamId = i;
		}

		if( mAudio.mStreamId < 0 && CodPar->codec_type == AVMEDIA_TYPE_AUDIO )
		{
			mAudio.mStreamId = i;
		}

		if( mSubtitle.mStreamId < 0 && CodPar->codec_type == AVMEDIA_TYPE_SUBTITLE )
		{
			mSubtitle.mStreamId = i;
		}
	}

	//-------------------------------------------------------------------------

	AVDictionary		*Opts = 0;

//	av_dict_set( &Opts, "refcounted_frames", "1", 0 );

	//-------------------------------------------------------------------------
	// Open Video Stream

	if( mVideo.mStreamId >= 0 )
	{
		if( !mVideo.open( mFormatContext->streams[ mVideo.mStreamId ], &Opts ) )
		{
			mErrorState = true;

			return( false );
		}

		AVRational		FrameRate = av_guess_frame_rate( mFormatContext, mFormatContext->streams[ mVideo.mStreamId ], NULL );

		qDebug() << "av_guess_frame_rate" << av_q2d( FrameRate );

		mVideo.mFrameRate = FrameRate;
	}

	//-------------------------------------------------------------------------
	// Open Audio Stream

	if( mAudio.mStreamId >= 0 )
	{
		if( !mAudio.open( mFormatContext->streams[ mAudio.mStreamId ], &Opts ) )
		{
			mErrorState = true;

			return( false );
		}
	}

	//-------------------------------------------------------------------------
	// Open Subtitle Stream

	if( mSubtitle.mStreamId >= 0 )
	{
		if( !mSubtitle.open( mFormatContext->streams[ mSubtitle.mStreamId ], &Opts ) )
		{
			mErrorState = true;

			return( false );
		}
	}

	av_dict_free( &Opts );

	//-------------------------------------------------------------------------

	av_init_packet( &mPacket );

	//-------------------------------------------------------------------------

	AVRational	AvTimBas;

	AvTimBas.num = 1;
	AvTimBas.den = AV_TIME_BASE;

	AVRational	TimBas;

	TimBas.num = 1;
	TimBas.den = 1;

	if( !mDuration && mFormatContext->duration != AV_NOPTS_VALUE )
	{
		mDuration = qreal( mFormatContext->duration ) * av_q2d( AvTimBas );
	}

	if( !mDuration )
	{
		mDuration = qMax( mDuration, mVideo.duration() );
	}

	if( !mDuration )
	{
		mDuration = qMax( mDuration, mAudio.duration() );
	}

	if( !mDuration )
	{
		mDuration = qMax( mDuration, mSubtitle.duration() );
	}

	qDebug() << "Duration:" << mDuration << "FPS:" << videoFrameRate();

	emit durationUpdated( mDuration );

	//-------------------------------------------------------------------------

	if( mAudio.mCodecContext )
	{
		AVCodecParameters		*CodPar = mFormatContext->streams[ mAudio.mStreamId ]->codecpar;

		//if( mAudioCodecContext->channel_layout != AV_CH_LAYOUT_STEREO || mAudioCodecContext->sample_rate != 48000 || mAudioCodecContext->sample_fmt != AV_SAMPLE_FMT_S16 )
		{
#if defined( TL_USE_LIB_AV )
#else
			mChannelLayout = CodPar->channel_layout;
			mChannels      = CodPar->channels;
			mSampleRate    = CodPar->sample_rate;
			mSampleFmt     = AVSampleFormat( CodPar->format );

			if( ( mSwrContext = swr_alloc() ) != 0 )
			{
				if( CodPar->channel_layout == 0 )
				{
					switch( CodPar->channels )
					{
						case 1:
							av_opt_set_int( mSwrContext, "in_channel_layout", AV_CH_LAYOUT_MONO, 0 );
							av_opt_set_int( mSwrContext, "out_channel_layout", AV_CH_LAYOUT_MONO,  0);
							break;

						case 2:
							av_opt_set_int( mSwrContext, "in_channel_layout", AV_CH_LAYOUT_STEREO, 0 );
							av_opt_set_int( mSwrContext, "out_channel_layout", AV_CH_LAYOUT_STEREO,  0);
							break;

						default:
							Q_ASSERT( "CHANNELS" );
							break;
					}
				}
				else
				{
					av_opt_set_int( mSwrContext, "in_channel_layout",  CodPar->channel_layout, 0 );
					av_opt_set_int( mSwrContext, "out_channel_layout", CodPar->channel_layout,  0);
				}

				av_opt_set_int( mSwrContext, "in_sample_rate",     CodPar->sample_rate,    0);
				av_opt_set_sample_fmt( mSwrContext, "in_sample_fmt",  AVSampleFormat( CodPar->format ),  0);

				av_opt_set_int( mSwrContext, "out_sample_rate",    48000,                0);
				av_opt_set_sample_fmt( mSwrContext, "out_sample_fmt", AV_SAMPLE_FMT_FLTP, 0);

				int	Error;

				if( ( Error = swr_init( mSwrContext ) ) != 0 )
				{
					swr_free( &mSwrContext );

					return( false );
				}
			}
#endif
		}

		if( mAudioProcessor->loadMedia( mFileName ) )
		{
			if( pProcess )
			{
				QtConcurrent::run( mAudioProcessor, &MediaAudioProcessor::process );
			}
		}
	}

	if( is_realtime( mFormatContext ) )
	{
	}

//	setPlayhead( 0 );

	readNext();

	return( true );
#else
	return( false );
#endif
}

void MediaSegment::unloadMedia( void )
{
#if defined( FFMPEG_SUPPORTED )
	mAudioProcessor->clearAudio();
#endif

	clearAudio();

	clearVideo();
}

#if defined( FFMPEG_SUPPORTED )
void MediaSegment::hapStaticDecodeCallback( HapDecodeWorkFunction function, void *p, unsigned int count, void *info)
{
	reinterpret_cast<MediaSegment *>( info )->hapDecodeCallback( function, p, count );
}

void MediaSegment::hapDecodeCallback( HapDecodeWorkFunction function, void *p, unsigned int count )
{
	for( unsigned int i = 0 ; i < count ; i++ )
	{
		// Invoke your multithreading mechanism to cause this function to be called
		// on a suitable number of threads.

		function( p, i );
	}
}
#endif

bool MediaSegment::isVideoPicture( void ) const
{
#if defined( FFMPEG_SUPPORTED )
	return( mVideo.mStreamId != -1 && ( mFormatContext->streams[ mVideo.mStreamId ]->disposition & AV_DISPOSITION_ATTACHED_PIC ) != 0 );
#else
	return( false );
#endif
}


bool MediaSegment::ptsInRange( qreal pPTS, qreal pTarget, qreal Min, qreal Max ) const
{
#if defined( FFMPEG_SUPPORTED )
	const qreal		L = pTarget - Min;
	const qreal		H = pTarget + Max;

	return( ( pPTS >= L && pPTS <= H ) || ( pPTS >= ( L - mDuration ) && pPTS <= ( H - mDuration ) ) || ( pPTS >= ( L + mDuration ) && pPTS <= ( H + mDuration ) ) );
#else
	return( false );
#endif
}

void MediaSegment::audioRange( qint64 &pMinPTS, qint64 &pMaxPTS ) const
{
	pMinPTS = pMaxPTS = -1;

#if defined( FFMPEG_SUPPORTED )
	QMutexLocker	L( &mAudDatLck );

	for( const AudioBuffer &AB : mAudDat )
	{
		if( pMinPTS == -1 || AB.mAudPts < pMinPTS )
		{
			pMinPTS = AB.mAudPts;
		}

		if( pMaxPTS == -1 || AB.mAudPts > pMaxPTS )
		{
			pMaxPTS = AB.mAudPts;
		}
	}
#endif
}

void MediaSegment::videoRange(qreal &pMinPTS, qreal &pMaxPTS) const
{
	pMinPTS = pMaxPTS = -1;

#if defined( FFMPEG_SUPPORTED )
	for( const VidDat &VD : mVidDat )
	{
		if( pMinPTS == -1 || VD.mPTS < pMinPTS )
		{
			pMinPTS = VD.mPTS;
		}

		if( pMaxPTS == -1 || VD.mPTS > pMaxPTS )
		{
			pMaxPTS = VD.mPTS;
		}
	}
#endif
}

void MediaSegment::processVideoFrame( qreal TargetPTS, qreal PacketTS, bool pForce )
{
#if defined( FFMPEG_SUPPORTED )
	double			TimeBase = av_q2d( mFormatContext->streams[ mVideo.mStreamId ]->time_base );
	VidDat			VD;

	memset( &VD, 0, sizeof( VD ) );

	if( isVideoPicture() )
	{
		pForce = true;
	}
	else
	{
		mVideo.mPTS = av_frame_get_best_effort_timestamp( mVideo.mFrame );

		if( mVideo.mPTS != AV_NOPTS_VALUE )
		{
			mVideo.mPTS = mVideo.mPTS * TimeBase;
		}
		else if( PacketTS != AV_NOPTS_VALUE )
		{
//			mVideo.mPTS = mVideo.mPTS;
		}
		else
		{
			mVideo.mPTS = qMax( TargetPTS, 0.0 );
		}

		VD.mPTS = mVideo.mPTS;

		if( av_frame_get_pkt_duration( mVideo.mFrame ) )
		{
			/* update the video clock */
			VD.mDuration = av_frame_get_pkt_duration( mVideo.mFrame ) * TimeBase;

			/* if we are repeating a frame, adjust clock accordingly */
			VD.mDuration += qreal( mVideo.mFrame->repeat_pict ) * ( VD.mDuration * 0.5 );
		}
		else
		{
	//		if( mFrameReadDuration != -1 && mFrameReadPTS >= 0 && VD.mPTS > mFrameReadPTS )
	//		{
	//			//mFrameDurationAverage += ( mFrameDurationAverage - ( VD.mPTS - mFrameReadPTS ) ) / 10.0;
	//		}

	//		VD.mDuration = mFrameDurationAverage;
		}
	}

	if( VD.mPTS > mVideo.mMaxPTS )
	{
		mVideo.mMaxPTS = VD.mPTS;
		mVideo.mMaxDur = VD.mDuration;
	}

#if defined( QT_DEBUG ) && defined( DEBUG_VIDEO_STORE )
	qDebug() << "Video Added:" << VD.mPTS;
#endif

	VD.mForced = pForce;

	if( hasVideoFrame( VD.mPTS ) )
	{
		return;
	}

	if( av_image_alloc( VD.mData, VD.mLineSizes, mVideo.mFrame->width, mVideo.mFrame->height, AVPixelFormat( mVideo.mFrame->format ), 32 ) < 0 )
	{
		return;
	}

	av_image_copy( VD.mData, VD.mLineSizes, (const uint8_t **)( mVideo.mFrame->data ), mVideo.mFrame->linesize, AVPixelFormat( mVideo.mFrame->format ), mVideo.mFrame->width, mVideo.mFrame->height );

	mVidDat.append( VD );

	if( mVideo.mCodecContext->refcounted_frames )
	{
		av_frame_unref( mVideo.mFrame );
	}
#endif
}

void MediaSegment::processAudioFrame( qreal TargetPTS, qreal PacketTS, bool pForce )
{
#if defined( FFMPEG_SUPPORTED )
	AVRational tb = AVRational{ 1, mAudio.mFrame->sample_rate };
	//double		TimeBase = av_q2d( mFormatContext->streams[ mAudio.mStreamId ]->time_base );
	double		TimeBase = av_q2d( tb );
	qreal		FrameTS = mAudio.mFrame->pts; //av_frame_get_best_effort_timestamp( mAudio.mFrame );

	if( FrameTS != AV_NOPTS_VALUE )
	{
		mAudio.mPTS = double( FrameTS ) * TimeBase;
	}
	else if( PacketTS != AV_NOPTS_VALUE )
	{
		mAudio.mPTS = double( PacketTS ) * TimeBase;
	}
	else
	{
		mAudio.mPTS = qMax( TargetPTS, 0.0 );
	}

	if( mAudio.mSamplePTS == -1 )
	{
		mAudio.mSamplePTS = qCeil( mAudio.mPTS * 48000 );
	}

	//qDebug() << "mAudioPTS:" << mAudioPTS << qCeil( mAudioPTS * 48000 ) << mAudioSamplePTS;

#if defined( TL_USE_LIB_AV )
#else
	int num_samples = av_rescale_rnd( swr_get_delay( mSwrContext, mAudio.mCodecContext->sample_rate ) + mAudio.mFrame->nb_samples, 48000, mAudio.mCodecContext->sample_rate, AV_ROUND_UP );

	AudioBuffer		AD;

	AD.mAudDat.resize( mChannels );

	AD.mForce = pForce;

	int		linsze;
	int		bufsze;

	if( ( bufsze = av_samples_alloc( AD.mAudDat.data(), &linsze, mChannels, num_samples, AV_SAMPLE_FMT_FLTP, 1 ) ) >= 0 )
	{
		if( mChannelLayout == mAudio.mCodecContext->channel_layout && mChannels == mAudio.mCodecContext->channels && mSampleRate == mAudio.mCodecContext->sample_rate && mSampleFmt == mAudio.mCodecContext->sample_fmt )
		{
			if( ( AD.mAudSmp = swr_convert( mSwrContext, AD.mAudDat.data(), num_samples, (const uint8_t **)mAudio.mFrame->data, mAudio.mFrame->nb_samples ) ) > 0 )
			{
				AD.mAudPts = mAudio.mSamplePTS;
				AD.mAudLen = av_samples_get_buffer_size( &linsze, mChannels, AD.mAudSmp, AV_SAMPLE_FMT_FLTP, 1 );

				if( AD.mAudPts / 48000.0 > mAudio.mMaxPTS )
				{
					mAudio.mMaxPTS = AD.mAudPts / 48000.0;
					mAudio.mMaxDur = AD.mAudLen / 48000.0;
				}

				QMutexLocker	MutLck( &mAudDatLck );

				mAudio.mSamplePTS += AD.mAudSmp;

				for( int i = 0 ; i < mAudDat.size() ; i++ )
				{
					AudioBuffer		&CurDat = mAudDat[ i ];

					if( CurDat.mAudPts == AD.mAudPts )
					{
#if defined( QT_DEBUG ) && defined( DEBUG_AUDIO_STORE )
						qDebug() << "Audio Duplicate:" << AD.mAudPts;
#endif
						return;
					}
				}

#if defined( QT_DEBUG ) && defined( DEBUG_AUDIO_STORE )
				qDebug() << "Audio Added:" << AD.mAudPts << AD.mAudSmp;
#endif

				mAudDat.append( AD );

				AD.clear();
			}
		}
	}
#endif
#endif // FFMPEG_SUPPORTED
}

void MediaSegment::processSubtitleFrame( qreal TargetPTS, qreal PacketTS, bool pForce )
{
	Q_UNUSED( TargetPTS )
	Q_UNUSED( PacketTS )
	Q_UNUSED( pForce )

#if defined( FFMPEG_SUPPORTED )
	if( mSubtitle.mCodecContext->refcounted_frames )
	{
		av_frame_unref( mVideo.mFrame );
	}
#endif
}

//-----------------------------------------------------------------------------
// Remove old video frames unless they are marked as forced, which happens
// when processing EOF

void MediaSegment::removeVideoFrames( void )
{
#if defined( FFMPEG_SUPPORTED )
	int		DelCnt = 0;

	for( int i = 0 ; i < mVidDat.size() ; )
	{
		VidDat		VD = mVidDat[ i ];

		if( VD.mForced )
		{
			i++; continue;
		}

		if( ptsInRange( VD.mPTS, mPlayHead, VIDEO_PTS_MINUS, VIDEO_PTS_PLUS ) )
		{
			i++; continue;
		}

		av_freep( &VD.mData[ 0 ] );

		mVidDat.removeAt( i );

		DelCnt++;
	}

	if( DelCnt && !is_realtime( mFormatContext ) )
	{
		updateVideoFrames( mPlayHead );
	}
#endif
}

void MediaSegment::removeAudioFrames()
{
#if defined( FFMPEG_SUPPORTED )
	QMutexLocker	MutLck( &mAudDatLck );

	for( int i = 0 ; i < mAudDat.size() ; )
	{
		AudioBuffer		&CurDat = mAudDat[ i ];

		if( !CurDat.mForce && !ptsInRange( CurDat.mAudPts / 48000.0, mAudio.mSamplePTS / 48000.0, AUDIO_PTS_MINUS, AUDIO_PTS_PLUS ) )
		{
#if defined( QT_DEBUG ) && defined( DEBUG_AUDIO_STORE )
			qDebug() << "Audio Removed:" << CurDat.mAudPts << CurDat.mAudSmp;
#endif

			mAudDat.removeAt( i );
		}
		else
		{
			i++;
		}
	}
#endif
}

QString MediaSegment::av_err( const QString &pHeader, int pErrorCode )
{
#if defined( FFMPEG_SUPPORTED )
	char	errbuf[ AV_ERROR_MAX_STRING_SIZE ];

	av_make_error_string( errbuf, AV_ERROR_MAX_STRING_SIZE, pErrorCode );

	return( QString( "%1: %2" ).arg( pHeader ).arg( QString::fromLatin1( errbuf ) ) );
#else
	return( "" );
#endif
}

bool MediaSegment::hasVideoFrame( qreal pPTS ) const
{
#if defined( FFMPEG_SUPPORTED )
	for( int i = 0 ; i < mVidDat.size() ; i++ )
	{
		if( mVidDat.at( i ).mPTS == pPTS )
		{
			return( true );
		}
	}
#endif
	return( false );
}

bool MediaSegment::hapProcess( qreal TargetPTS, qreal PacketTS, bool pForce )
{
#if defined( FFMPEG_SUPPORTED )
	if( mVideo.mCodec->id != AV_CODEC_ID_HAP )
	{
		return( false );
	}

	if( !mPacket.data || mPacket.size <= 0 )
	{
		return( false );
	}

	unsigned long	DataUsed   = 0;
	unsigned int	DataFormat = 0;

	if( HapGetFrameTextureFormat( mPacket.data, mPacket.size, 0, &DataFormat ) != HapResult_No_Error )
	{
		return( false );
	}

	double			TimeBase = av_q2d( mFormatContext->streams[ mVideo.mStreamId ]->time_base );
	VidDat			VD;

	memset( &VD, 0, sizeof( VD ) );

	VD.mPTS = mPacket.pts;

	if( VD.mPTS != AV_NOPTS_VALUE )
	{
		VD.mPTS = VD.mPTS * TimeBase;
	}
	else if( PacketTS != AV_NOPTS_VALUE )
	{
		VD.mPTS = PacketTS;
	}
	else
	{
		VD.mPTS = TargetPTS;
	}

	if( mPacket.duration )
	{
		/* update the video clock */
		VD.mDuration = mPacket.duration * TimeBase;
	}
	else
	{
//		if( mFrameReadDuration != -1 && mFrameReadPTS >= 0 && VD.mPTS > mFrameReadPTS )
//		{
//			//mFrameDurationAverage += ( mFrameDurationAverage - ( VD.mPTS - mFrameReadPTS ) ) / 10.0;
//		}

//		VD.mDuration = mFrameDurationAverage;
	}

//	mFrameReadPTS      = VD.mPTS;
//	mFrameReadDuration = VD.mDuration;

	if( hasVideoFrame( VD.mPTS ) )
	{
		return( true );
	}

	//qDebug() << "mFrameReadPTS: " << mFrameReadPTS << "mFrameReadDuration:" << mFrameReadDuration << ( mFrameReadPTS + mFrameReadDuration );

	VD.mForced = pForce;

	// TODO: Calculate this correctly

	int BufferSize = mVideo.mCodecContext->width * mVideo.mCodecContext->height;

	switch( DataFormat )
	{
		case HapTextureFormat_RGB_DXT1:
			BufferSize = BufferSize / 2;
			break;

		case HapTextureFormat_RGBA_DXT5:
		case HapTextureFormat_YCoCg_DXT5:
			BufferSize = BufferSize * 4;
			break;

		default:
			return( false );
	}

	if( av_reallocp( &VD.mData[ 0 ], BufferSize ) )
	{
		return( false );
	}

	if( HapDecode( mPacket.data, mPacket.size, 0, &MediaSegment::hapStaticDecodeCallback, this, VD.mData[ 0 ], BufferSize, &DataUsed, &DataFormat ) != HapResult_No_Error )
	{
		return( false );
	}

	VD.mLineSizes[ 0 ] = DataUsed / mVideo.mCodecContext->height;

	mVidDat.append( VD );

	mVideo.mFrame->height        = mVideo.mCodecContext->height;
	mVideo.mFrame->width         = mVideo.mCodecContext->width;
	mVideo.mFrame->repeat_pict   = 0;

	mVideo.mFrame->format        = DataFormat;
	mVideo.mFrame->linesize[ 0 ] = DataUsed / mVideo.mCodecContext->height;
	mVideo.mFrame->key_frame     = 1;

	return( true );
#else
	return( false );
#endif
}

bool MediaSegment::readVideoFrame( qreal TargetPTS, bool pForce )
{
#if defined( FFMPEG_SUPPORTED )
	while( true )
	{
		if( hapProcess( TargetPTS, TargetPTS, pForce ) )
		{
			return( true );
		}

		int	RcvErr = 0;

		while( ( RcvErr = avcodec_receive_frame( mVideo.mCodecContext, mVideo.mFrame ) ) == 0 )
		{
			processVideoFrame( TargetPTS, TargetPTS, pForce );
		}

		if( !is_realtime( mFormatContext ) )
		{
			updateVideoFrames( mPlayHead );
		}

		if( !RcvErr )
		{
			return( true );
		}

		if( RcvErr == AVERROR_EOF )
		{
			break;
		}

		if( RcvErr != AVERROR(EAGAIN) )
		{
			mErrorState = true;
		}

		break;
	}
#endif
	return( false );
}

bool MediaSegment::readAudioFrame( qreal TargetPTS, bool pForce )
{
#if defined( FFMPEG_SUPPORTED )
	int	RcvErr;

	while( ( RcvErr = avcodec_receive_frame( mAudio.mCodecContext, mAudio.mFrame ) ) == 0 )
	{
		processAudioFrame( TargetPTS, TargetPTS, pForce );
	}

	if( RcvErr == AVERROR_EOF )
	{
		return( false );
	}

	if( RcvErr && RcvErr != AVERROR(EAGAIN) )
	{
		mErrorState = true;

		return( false );
	}

	return( true );
#else
	return( false );
#endif
}

bool MediaSegment::readSubtitleFrame( qreal TargetPTS, bool pForce )
{
#if defined( FFMPEG_SUPPORTED )
	int	RcvErr;

	while( ( RcvErr = avcodec_receive_frame( mSubtitle.mCodecContext, mSubtitle.mFrame ) ) == 0 )
	{
		processSubtitleFrame( TargetPTS, TargetPTS, pForce );
	}

	if( RcvErr == AVERROR_EOF )
	{
		return( false );
	}

	if( RcvErr && RcvErr != AVERROR(EAGAIN) )
	{
		mErrorState = true;

		return( false );
	}

	return( true );
#else
	return( false );
#endif
}

int MediaSegment::findFrameIndex( qreal pTimeStamp ) const
{
#if defined( FFMPEG_SUPPORTED )
	int			CurIdx = -1;
	qreal		CurDif = 10000.0;

	for( int i = 0 ; i < mVidDat.size() ; i++ )
	{
		const VidDat &CVD = mVidDat[ i ];

		if( CVD.mPTS > pTimeStamp )
		{
			if( CVD.mPTS - pTimeStamp < 0.005 )
			{
				return( i );
			}

			continue;
		}

		qreal	NewDif = pTimeStamp - CVD.mPTS;

		if( CurIdx < 0 || NewDif < CurDif )
		{
			CurIdx = i;
			CurDif = NewDif;
		}
	}

	return( CurDif < 0.05 ? CurIdx : -1 );
#else
	return( -1 );
#endif
}

#include <cmath>

void MediaSegment::updateVideoFrames( qreal pPTS )
{
#if defined( FFMPEG_SUPPORTED )
	// Reset the current indexes

	mVideo.mPrevIdx = -1;
	mVideo.mCurrIdx = -1;
	mVideo.mNxt1Idx = -1;
	mVideo.mNxt2Idx = -1;

	if( mVidDat.isEmpty() )
	{
		return;
	}

	// Media files can have a single image, such as an album cover embedded in an MP3
	// Detect this case here

	if( isVideoPicture() )
	{
		if( !mVidDat.isEmpty() )
		{
			mVideo.mPrevIdx = 0;
			mVideo.mCurrIdx = 0;
			mVideo.mNxt1Idx = 0;
			mVideo.mNxt2Idx = 0;

			mVideo.mPTS = 0;
		}

		return;
	}

	// We're in a media file with multiple frames.

	const double	StreamFrameRate = av_q2d( mVideo.mFrameRate );

	if( StreamFrameRate <= 0.0 )
	{
		qWarning() << "Can't detect media frame rate";

		return;
	}

	const float	FrameRate = 1.0 / StreamFrameRate;
	const float	FrameDiv  = float( pPTS ) / FrameRate;

	int			CurrFrm = std::floor( FrameDiv );

	double		PrevPTS = double( CurrFrm - 1 ) * FrameRate;
	double		CurrPTS = double( CurrFrm + 0 ) * FrameRate;
	double		Nxt1PTS = double( CurrFrm + 1 ) * FrameRate;
	double		Nxt2PTS = double( CurrFrm + 2 ) * FrameRate;

	mVideo.mPrevIdx = findFrameIndex( PrevPTS );
	mVideo.mCurrIdx = findFrameIndex( CurrPTS );
	mVideo.mNxt1Idx = findFrameIndex( Nxt1PTS );
	mVideo.mNxt2Idx = findFrameIndex( Nxt2PTS );

	if( mVideo.mPrevIdx == -1 && mVideo.mCurrIdx != -1 )
	{
		mVideo.mPrevIdx = mVideo.mCurrIdx;
	}

	if( mVideo.mPrevIdx != -1 && mVideo.mCurrIdx == -1 )
	{
		mVideo.mCurrIdx = mVideo.mPrevIdx;
	}

	if( mDuration > 0 )
	{
		if( Nxt1PTS >= mDuration )
		{
			mVideo.mNxt1Idx = mVideo.mCurrIdx;
		}

		if( Nxt2PTS >= mDuration )
		{
			mVideo.mNxt2Idx = ( mVideo.mNxt1Idx != -1 ? mVideo.mNxt1Idx : mVideo.mCurrIdx );
		}
	}

	if( mVideo.mNxt1Idx == -1 && mVideo.mNxt2Idx != -1 )
	{
		mVideo.mNxt1Idx = mVideo.mNxt2Idx;
	}

	if( pPTS <= mVideo.mMaxDur && mVideo.mPrevIdx == -1 && mVideo.mCurrIdx == -1 && mVideo.mNxt1Idx != -1 )
	{
		mVideo.mPrevIdx = mVideo.mCurrIdx = mVideo.mNxt1Idx;
	}
#endif
}

bool MediaSegment::haveVideoFrames( void ) const
{
#if defined( FFMPEG_SUPPORTED )
	return( mVideo.mPrevIdx != -1 && mVideo.mCurrIdx != -1 && mVideo.mNxt1Idx != -1 && mVideo.mNxt2Idx != -1 );
#else
	return( false );
#endif
}

void MediaSegment::setPlayhead( qreal pTimeStamp )
{
#if defined( FFMPEG_SUPPORTED )
	AVStream		*VideoStream   = ( mVideo.mStreamId != -1 ? mFormatContext->streams[ mVideo.mStreamId ] : 0 );
	AVStream		*AudioStream   = ( mAudio.mStreamId != -1 ? mFormatContext->streams[ mAudio.mStreamId ] : 0 );

	if( mErrorState || ( !VideoStream && !AudioStream ) )
	{
		return;
	}

	const bool VideoIsPicture = isVideoPicture();

	if( ( !VideoStream || VideoIsPicture ) && AudioStream && mAudioProcessor->preloaded() )
	{
		return;
	}

	if( is_realtime( mFormatContext ) )
	{
		readNext();

		return;
	}

	mPlayHead = qBound( 0.0, pTimeStamp, mDuration );

	// Check whether we need to seek to a new position in the media

	if( mDuration > 0.25 )
	{
		const bool VideoNeedsSeek = ( mVideo.mStreamId == -1 || ( mVideo.mPTS != -1 && ( mVideo.mPTS + 0.50 < mPlayHead || mVideo.mPTS - 0.5 > mPlayHead ) && !VideoIsPicture ) || VideoIsPicture );
		const bool AudioNeedsSeek = ( mAudio.mStreamId == -1 || ( mAudio.mPTS != -1 && ( mAudio.mPTS + 0.25 < mPlayHead || mAudio.mPTS - 1.0 > mPlayHead ) ) || mAudioProcessor->preloaded() );

		if( VideoNeedsSeek && AudioNeedsSeek )
		{
			int64_t			StreamTarget = mPlayHead * AV_TIME_BASE;

#if defined( QT_DEBUG ) && defined( DEBUG_SEEK )
			qDebug() << "Seek =" << mPlayHead << StreamTarget << mVideo.mPTS << mAudio.mPTS;
#endif

			int				SeekError = avformat_seek_file( mFormatContext, -1, INT64_MIN, StreamTarget, INT64_MAX, 0 );

			if( SeekError < 0 )
			{
#if defined( QT_DEBUG ) && defined( DEBUG_SEEK )
				qDebug() << "Seek =" << mPlayHead << StreamTarget << av_err( "SeekError", SeekError );
#endif

				SeekError = avformat_seek_file( mFormatContext, -1, INT64_MIN, StreamTarget, INT64_MAX, AVSEEK_FLAG_ANY );
			}

			mAudio.mPTS          = -1;
			mVideo.mPTS          = -1;
			mAudio.mSamplePTS    = -1;

			if( mVideo.mCodecContext )
			{
				avcodec_flush_buffers( mVideo.mCodecContext );
			}

			if( mAudio.mCodecContext )
			{
				avcodec_flush_buffers( mAudio.mCodecContext );
			}

			if( SeekError < 0 )
			{
				if( SeekError != AVERROR_EOF )
				{
					qWarning() << av_err( "avformat_seek_file", SeekError );

					mErrorState = true;
				}

				return;
			}
		}
	}

	removeVideoFrames();
	removeAudioFrames();

	if( VideoStream )
	{
		updateVideoFrames( mPlayHead );
	}

	// Read and process packets from the media file

	while( true )
	{
		if( AudioStream && !mAudioProcessor->preloaded() )
		{
			if( mAudio.mPTS >= mPlayHead && ( !VideoStream || mVideo.mPTS >= mPlayHead ) )
			{
				if( VideoStream )
				{
					updateVideoFrames( mPlayHead );
				}

				break;
			}
		}
		else
		{
			if( VideoIsPicture )
			{
				break;
			}

			if( haveVideoFrames() )
			{
				break;
			}
		}

		av_packet_unref( &mPacket );

		// Read frames until we hit EOF

		mTimeoutHandler.reset( 100000 );

		int			 ReadError;

		if( ( ReadError = av_read_frame( mFormatContext, &mPacket ) ) < 0 )
		{
			if( ReadError != AVERROR_EOF )
			{
				qWarning() << av_err( "av_read_frame", ReadError );

				mErrorState = true;

				return;
			}

			double		MaxPTS = -1;
			double		MaxDur = 0;

			if( mVideo.mStreamId != -1 )
			{
				while( ( mVideoSendResult = avcodec_send_packet( mVideo.mCodecContext, NULL ) ) == 0 )
				{
					readVideoFrame( mPlayHead, true );
				}

				if( mVideo.mMaxPTS > MaxPTS )
				{
					MaxPTS = mVideo.mMaxPTS;
					MaxDur = mVideo.mMaxDur;
				}
			}

			if( mAudio.mStreamId != -1 && !mAudioProcessor->preloaded() )
			{
				while( ( mAudioSendResult = avcodec_send_packet( mAudio.mCodecContext, NULL ) ) == 0 )
				{
					readAudioFrame( mPlayHead, true );
				}

				if( mAudio.mMaxPTS > MaxPTS )
				{
					MaxPTS = mAudio.mMaxPTS;
					MaxDur = mAudio.mMaxDur;
				}
			}

			if( MaxPTS + MaxDur < mDuration )
			{
				qInfo() << "Duration updated from" << mDuration << "to" << ( MaxPTS + MaxDur );

				mDuration = MaxPTS + MaxDur;
			}

			break;
		}

		// We have a new packet

		int64_t		PacketTS = AV_NOPTS_VALUE;

		if( mPacket.pts != AV_NOPTS_VALUE )
		{
			PacketTS = mPacket.pts;
		}
		else if( mPacket.dts != AV_NOPTS_VALUE )
		{
			PacketTS = mPacket.dts;
		}

		if( mPacket.stream_index == mVideo.mStreamId )
		{
			do
			{
				mVideoSendResult = avcodec_send_packet( mVideo.mCodecContext, &mPacket );

				if( !mVideoSendResult || mVideoSendResult == AVERROR( EAGAIN ) )
				{
					readVideoFrame( mPlayHead, false );
				}
			}
			while( mVideoSendResult == AVERROR( EAGAIN ) );
		}
		else if( mPacket.stream_index == mAudio.mStreamId && !mAudioProcessor->preloaded() )
		{
			do
			{
				mAudioSendResult = avcodec_send_packet( mAudio.mCodecContext, &mPacket );

				if( !mAudioSendResult || mAudioSendResult == AVERROR( EAGAIN ) )
				{
					while( ( mAudioRecvResult = avcodec_receive_frame( mAudio.mCodecContext, mAudio.mFrame ) ) == 0 )
					{
						processAudioFrame( mPlayHead, mPlayHead, false );
					}
				}
			}
			while( mAudioSendResult == AVERROR( EAGAIN ) );
		}
		else if( mPacket.stream_index == mSubtitle.mStreamId )
		{
			// untested
#if 0
			int	r, g;
			AVSubtitle S;

			memset( &S, 0, sizeof( S ) );

			r = avcodec_decode_subtitle2( mSubtitle.mCodecContext, &S, &g, &mPacket );

			if( g )
			{
				avsubtitle_free( &S );
			}
#endif
		}
	}

#if defined( QT_DEBUG ) && defined( DEBUG_HAVE_FRAMES )
	bool	HaveFrames = haveVideoFrames();

	if( !HaveFrames )
	{
		qint64	AudMin, AudMax;
		qreal	VidMin, VidMax;

		audioRange( AudMin, AudMax );

		videoRange( VidMin, VidMax );

		qDebug() << "!haveFramesFor()" << mPlayHead << mVideo.mPrevIdx << mVideo.mCurrIdx << mVideo.mNxt1Idx << mVideo.mNxt2Idx << "(" << VidMin << VidMax << ")" << "(" << ( qreal( AudMin ) / 48000.0 ) << ( qreal( AudMax ) / 48000.0 ) << ")";
	}
#endif

#endif // FFMPEG_SUPPORTED
}

void MediaSegment::mixAudio( qint64 pSamplePosition, qint64 pSampleCount, int pChannelOffset, int pChannelCount, void **pBuffers, float pVol ) const
{
#if defined( FFMPEG_SUPPORTED )
	if( mAudio.mStreamId == -1 )
	{
		return;
	}

	if( mAudioProcessor->preloaded() )
	{
		mAudioProcessor->mixAudio( pSamplePosition, pSampleCount, pChannelOffset, pChannelCount, pBuffers, pVol );

		return;
	}

	QMutexLocker		Lock( &mAudDatLck );

	//qDebug() << "addData" << pSamplePosition << pSampleCount;

	qreal				SmpCpy = 0;

	for( const AudioBuffer &AD : mAudDat )
	{
		if( pSamplePosition >= AD.mAudPts + AD.mAudSmp )
		{
			continue;
		}

		if( pSamplePosition + pSampleCount <= AD.mAudPts )
		{
			continue;
		}

		qint64		AudPos = qMax( pSamplePosition, AD.mAudPts );
		int			AudLen = qMin( ( AD.mAudPts + AD.mAudSmp ) - AudPos, ( pSamplePosition + pSampleCount ) - AudPos );

		qint64		SrcPos = AudPos - AD.mAudPts;
		qint64		DstPos = AudPos - pSamplePosition;

#if defined( QT_DEBUG )
		Q_ASSERT( SrcPos >= 0 );
		Q_ASSERT( DstPos >= 0 );
		Q_ASSERT( SrcPos < AD.mAudSmp );
		Q_ASSERT( DstPos < pSampleCount );
		Q_ASSERT( SrcPos + AudLen <= AD.mAudSmp );
		Q_ASSERT( DstPos + AudLen <= pSampleCount );
#endif

//		qDebug() << SrcPos << QString( "(%1)" ).arg( AD.mAudSmp ) << "->" << DstPos << AudLen;

		for( int i = 0 ; i < pChannelCount ; i++ )
		{
			if( AD.mAudDat.size() <= i )
			{
				continue;
			}

			const float	*Src1 = reinterpret_cast<const float *>( AD.mAudDat[ i ] ) + SrcPos;
			float		*Src2 = reinterpret_cast<float *>( pBuffers[ i ] ) + DstPos;

			for( int j = 0 ; j < AudLen ; j++ )
			{
				const int		Idx = j;

				Src2[ Idx ] += Src1[ Idx ] * pVol;
			}
		}

		SmpCpy += AudLen;

		if( SmpCpy == pSampleCount )
		{
			break;
		}
	}

#if defined( QT_DEBUG ) && defined( DEBUG_MIX_AUDIO )
	if( SmpCpy != pSampleCount )
	{
		qDebug() << "SegmentVideo::mixAudio:" << pSamplePosition << "-" << SmpCpy << "!=" << pSampleCount;
	}
#endif

#endif // FFMPEG_SUPPORTED
}

void MediaSegment::clearAudio()
{
#if defined( FFMPEG_SUPPORTED )
	QMutexLocker		Lock( &mAudDatLck );

	mAudDat.clear();
#endif
}

void MediaSegment::clearVideo()
{
#if defined( FFMPEG_SUPPORTED )
	for( int i = 0 ; i < mVidDat.size() ; i++ )
	{
		av_freep( &mVidDat[ i ].mData[ 0 ] );
	}

	mVidDat.clear();

	mVideo.mPrevIdx = mVideo.mCurrIdx = mVideo.mNxt1Idx = mVideo.mNxt2Idx = -1;
#endif
}

void MediaSegment::decodeIFrames( bool pDecode )
{
#if defined( FFMPEG_SUPPORTED )
	mDecodeI = pDecode;
#endif
}

void MediaSegment::decodeBFrames( bool pDecode )
{
#if defined( FFMPEG_SUPPORTED )
	mDecodeB = pDecode;
#endif
}

qreal MediaSegment::wavL( qreal pTimeStamp ) const
{
#if defined( FFMPEG_SUPPORTED )
	return( mAudioProcessor->wavL( pTimeStamp ) );
#else
	return( 0 );
#endif
}

qreal MediaSegment::wavR( qreal pTimeStamp ) const
{
#if defined( FFMPEG_SUPPORTED )
	return( mAudioProcessor->wavR( pTimeStamp ) );
#else
	return( 0 );
#endif
}

QSize MediaSegment::imageSize() const
{
#if defined( FFMPEG_SUPPORTED )
	if( !hasVideo() )
	{
		return( QSize() );
	}

	AVCodecParameters		*CodPar = mFormatContext->streams[ mVideo.mStreamId ]->codecpar;

	return( QSize( CodPar->width, CodPar->height ) );
#else
	return( QSize() );
#endif
}

void MediaSegment::setPreload( bool pPreload )
{
#if defined( FFMPEG_SUPPORTED )
	if( pPreload )
	{
		if( !mAudioProcessor->options().testFlag( MediaAudioProcessor::Preload ) )
		{
			mAudioProcessor->setOptions( MediaAudioProcessor::Preload );

			if( !mFileName.isEmpty() )
			{
				QtConcurrent::run( mAudioProcessor, &MediaAudioProcessor::process );
			}
		}
	}
	else
	{
		mAudioProcessor->setOptions( MediaAudioProcessor::NoOptions );

		mAudioProcessor->clearAudio();
	}
#endif
}

bool MediaSegment::preload() const
{
#if defined( FFMPEG_SUPPORTED )
	return( mAudioProcessor->options().testFlag( MediaAudioProcessor::Preload ) );
#else
	return( false );
#endif
}

const fugio::SegmentInterface::VidDat *MediaSegment::viddat() const
{
#if defined( FFMPEG_SUPPORTED )
	if( !hasVideo() )
	{
		return( nullptr );
	}

	if( mVidDat.isEmpty() )
	{
		return( nullptr );
	}

	if( mVideo.mCurrIdx < 0 )
	{
		return( nullptr );
	}

	return( &mVidDat[ mVideo.mCurrIdx ] );
#else
	return( nullptr );
#endif
}

int MediaSegment::imageFormat() const
{
#if defined( FFMPEG_SUPPORTED )
	if( !hasVideo() )
	{
		return( AV_PIX_FMT_NONE );
	}

	AVCodecParameters		*CodPar = mFormatContext->streams[ mVideo.mStreamId ]->codecpar;

	return( CodPar->format );
#else
	return( -1 );
#endif
}

bool MediaSegment::imageIsHap() const
{
#if defined( FFMPEG_SUPPORTED )
	return( mVideo.mCodec && mVideo.mCodec->id == AV_CODEC_ID_HAP );
#else
	return( false );
#endif
}

qreal MediaSegment::videoFrameRate() const
{
#if defined( FFMPEG_SUPPORTED )
	return( mVideo.mCodecContext && mVideo.mFrameRate.den ? av_q2d( mVideo.mFrameRate ) : 0.0 );
#else
	return( 0.0 );
#endif
}

void MediaSegment::readNext()
{
#if defined( FFMPEG_SUPPORTED )
	AVStream		*VideoStream   = ( mVideo.mStreamId != -1 ? mFormatContext->streams[ mVideo.mStreamId ] : 0 );
	AVStream		*AudioStream   = ( mAudio.mStreamId != -1 ? mFormatContext->streams[ mAudio.mStreamId ] : 0 );

	if( mErrorState || ( !VideoStream && !AudioStream ) )
	{
		return;
	}

	if( VideoStream )
	{
		if( mVideo.mCurrIdx != -1 )
		{
			qreal	OldPlayHead = mPlayHead;
			qreal	CurDuration = 0;

			mPlayHead = mVidDat[ mVideo.mCurrIdx ].mPTS;

			for( const VidDat &VD : mVidDat )
			{
				CurDuration = VD.mDuration;

				if( VD.mPTS <= mPlayHead )
				{
					continue;
				}

				mPlayHead = VD.mPTS;

				break;
			}

			if( OldPlayHead == mPlayHead )
			{
				mPlayHead += CurDuration;
			}
		}
		else
		{
			mPlayHead = 0;
		}
	}
	else if( AudioStream )
	{
	}

	while( true )
	{
		const bool	HaveAudio = !AudioStream || mAudioProcessor->preloaded() || mAudio.mPTS >= mPlayHead;
		const bool	HaveVideo = !VideoStream || mVideo.mPTS >= mPlayHead;

		if( HaveAudio && HaveVideo )
		{
			break;
		}

		av_packet_unref( &mPacket );

		int			 ReadError;

		mTimeoutHandler.reset( 1000 );

		if( ( ReadError = av_read_frame( mFormatContext, &mPacket ) ) < 0 )
		{
			if( ReadError == AVERROR(EAGAIN) || ReadError == AVERROR_EXIT )
			{
				break;
			}

			if( ReadError != AVERROR_EOF )
			{
				qWarning() << av_err( "av_read_frame", ReadError );

				mErrorState = true;

				return;
			}

			if( VideoStream )
			{
				while( ( mVideoSendResult = avcodec_send_packet( mVideo.mCodecContext, NULL ) ) == 0 )
				{
					readVideoFrame( mPlayHead, true );
				}
			}

			if( AudioStream )
			{
				while( ( mAudioSendResult = avcodec_send_packet( mAudio.mCodecContext, NULL ) ) == 0 )
				{
					readAudioFrame( mPlayHead, true );
				}
			}

			break;
		}

		int64_t		PacketTS = AV_NOPTS_VALUE;

		if( mPacket.pts != AV_NOPTS_VALUE )
		{
			PacketTS = mPacket.pts;
		}
		else if( mPacket.dts != AV_NOPTS_VALUE )
		{
			PacketTS = mPacket.dts;
		}

		if( mPacket.stream_index == mVideo.mStreamId )
		{
			if( ( mVideoSendResult = avcodec_send_packet( mVideo.mCodecContext, &mPacket ) ) == 0 )
			{
				readVideoFrame( mPlayHead, false );
			}
		}
		else if( mPacket.stream_index == mAudio.mStreamId )
		{
			if( ( mAudioSendResult = avcodec_send_packet( mAudio.mCodecContext, &mPacket ) ) == 0 )
			{
				while( ( mAudioRecvResult = avcodec_receive_frame( mAudio.mCodecContext, mAudio.mFrame ) ) == 0 )
				{
					processAudioFrame( mPlayHead, mPlayHead, false );
				}
			}
		}
		else if( mPacket.stream_index == mSubtitle.mStreamId )
		{

		}
	}

	if( VideoStream )
	{
		removeVideoFrames();

		updateVideoFrames( mPlayHead );
	}

	if( AudioStream )
	{
		if( !VideoStream && !mAudDat.isEmpty() )
		{
			mPlayHead = qreal( mAudDat.last().mAudPts ) / 48000.0;
		}

		removeAudioFrames();
	}
#endif
}
