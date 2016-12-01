#include "mediaaudioprocessor.h"
#include <QDebug>
#include <QFileInfo>
#include <QDateTime>

static const qreal		AUD_INC = 250.0; //44100.0 / 4.41;

MediaAudioProcessor::MediaAudioProcessor( void )
{
	mOptions = Calculate;

	mFormatContext = 0;
	mAudioCodecContext = 0;
	mAudioCodec = 0;
	mFrameSrc = 0;
	mPacketSize = 0;
	mSwrContext = 0;
	mErrorState = false;
	SrcDat = 0;
	SrcLen = 0;
	mDuration = 0;
	mFrameReadPTS      = -1;
	mFrameReadDuration = 0;
	mFrameCurrPTS      = -1;
	mFrameCurrDuration = 0;
	AudioParserContext = 0;
	ParserData = 0;
	ParserSize = 0;
	mAudioPTS = -1;
	mAudioSamplePTS = -1;
	mFrameDst = 0;
	mFrameFinished = 0;
	mPreloaded = false;
	mCalculated = false;
	mDecodeI = true;
	mDecodeB = true;
	mFrameDurationAverage = 1.0 / 25.0;

	mAudPts = 0;
	mAudNxt = AUD_INC;
}

MediaAudioProcessor::~MediaAudioProcessor()
{

}

void MediaAudioProcessor::processAudioFrame()
{
	QMutexLocker		Lock( &mAudDatLck );

#if defined( TL_USE_LIB_AV )
#else
	int linesize    = 0;
	int out_samples = av_rescale_rnd( swr_get_delay( mSwrContext, mAudioCodecContext->sample_rate ) + mFrameSrc->nb_samples, 48000, mAudioCodecContext->sample_rate, AV_ROUND_UP);
	int dst_bufsize = av_samples_get_buffer_size( &linesize, mChannels, out_samples, AV_SAMPLE_FMT_FLTP, 1 );
	int	cur_bufsize = ( mOptions.testFlag( Preload ) && !mPreloaded ? mAudBuf.first().size() : 0 );

	QVector<uint8_t *>	OutPtr;

	OutPtr.resize( mChannels );

	for( int i = 0 ; i < mChannels ; i++ )
	{
		mAudBuf[ i ].resize( cur_bufsize + out_samples );

		OutPtr[ i ] = (uint8_t *)&mAudBuf[ i ][ mAD.mAudSmp ];
	}

	if( mChannelLayout == mAudioCodecContext->channel_layout && mChannels == mAudioCodecContext->channels && mSampleRate == mAudioCodecContext->sample_rate && mSampleFmt == mAudioCodecContext->sample_fmt )
	{
		out_samples = swr_convert( mSwrContext, &OutPtr[ 0 ], out_samples, (const uint8_t **)mFrameSrc->data, mFrameSrc->nb_samples );
		dst_bufsize = av_samples_get_buffer_size( &linesize, mChannels, out_samples, AV_SAMPLE_FMT_FLTP, 1 );

		if( !mCalculated )
		{
			for( int c = 0 ; c < mChannels ; c++ )
			{
				AudPrv	&AP     = mAudPrv[ c ];
				float	*SrcPtr = (float *)OutPtr[ c ];

				for( int i = 0 ; i < out_samples ; i++ )
				{
					float		l = SrcPtr[ i ];

					AP.mAudSm = qMax( AP.mAudSm, powf( l * l, 1./8. ) );

					mAudPts += 1.0;

					if( mAudPts >= mAudNxt )
					{
						AP.mAudMax = qMax( AP.mAudSm, AP.mAudMax );

						AP.mAudWav.append( AP.mAudSm );

						AP.mAudSm  = 0;

						mAudNxt += AUD_INC;
					}
				}
			}
		}

		if( mOptions.testFlag( Preload ) && !mPreloaded )
		{
			mAD.mAudSmp += out_samples;
			mAD.mAudLen += linesize;

			if( mAD.mAudSmp > 48000 )
			{
				int		bufsze;
				int		linsze;

				if( ( bufsze = av_samples_alloc( mAD.mAudDat.data(), &linsze, mChannels, mAD.mAudSmp, AV_SAMPLE_FMT_FLTP, 0 ) ) >= 0 )
				{
					for( int i = 0 ; i < mChannels ; i++ )
					{
						memcpy( mAD.mAudDat[ i ], mAudBuf[ i ].data(), mAD.mAudLen );

						//mAD.mAudDat = mAudBuf.
					}

					mAudDat.append( mAD );
				}

				for( int i = 0 ; i < mChannels ; i++ )
				{
					mAudBuf[ i ].clear();

					mAD.mAudDat[ i ] = 0;
				}

				mAD.mAudPts += mAD.mAudSmp;

				mAD.mAudLen = 0;
				mAD.mAudSmp = 0;
			}
		}
	}
#endif

}

QString MediaAudioProcessor::av_err(const QString &pHeader, int pErrorCode)
{
	char	errbuf[ AV_ERROR_MAX_STRING_SIZE ];

	av_make_error_string( errbuf, AV_ERROR_MAX_STRING_SIZE, pErrorCode );

	return( QString( "%1: %2" ).arg( pHeader ).arg( QString::fromLatin1( errbuf ) ) );
}

void MediaAudioProcessor::clearAudio()
{
	QMutexLocker		Lock( &mAudDatLck );

	mAudDat.clear();

	mPreloaded = false;
}

bool MediaAudioProcessor::loadMedia( const QString &pFileName )
{
	mFileName = pFileName;

	if( avformat_open_input( &mFormatContext, mFileName.toLatin1().constData(), NULL, NULL ) != 0 )
	{
		qWarning() << "Couldn't avformat_open_input" << mFileName;

		mErrorState = true;

		return( false );
	}

	if( avformat_find_stream_info( mFormatContext, 0 ) < 0 )
	{
		qWarning() << "Couldn't avformat_find_stream_info" << mFileName;

		mErrorState = true;

		return( false );
	}

	mAudioStream = -1;

	for( int i = 0 ; i < (int)mFormatContext->nb_streams ; i++ )
	{
		if( mAudioStream < 0 && mFormatContext->streams[ i ]->codec->codec_type == AVMEDIA_TYPE_AUDIO )
		{
			mAudioStream = i;
		}
	}

	if( mAudioStream == -1 )
	{
		return( false );
	}

	if( ( mFrameSrc = av_frame_alloc() ) == 0 )
	{
		//mErrorState = true;

		return( false );
	}

	//-------------------------------------------------------------------------

	mAudioCodecContext = ( mAudioStream < 0 ? 0 : mFormatContext->streams[ mAudioStream ]->codec );

	//-------------------------------------------------------------------------
	// Open Audio Stream

	if( mAudioStream >= 0 && ( mAudioCodec = avcodec_find_decoder( mAudioCodecContext->codec_id ) ) == 0 )
	{
		qWarning() << "Couldn't avcodec_find_decoder for audio" << mFileName;

		mAudioStream = -1;

		mAudioCodecContext = 0;
	}

	if( mAudioCodec != 0 && avcodec_open2( mAudioCodecContext, mAudioCodec, 0 ) < 0 )
	{
		qWarning() << "Couldn't avcodec_open2 for audio" << mFileName;

		mErrorState = true;

		return( false );
	}

	//-------------------------------------------------------------------------

	av_init_packet( &mPacket );
	av_init_packet( &mParserPacket );

	//-------------------------------------------------------------------------

	AVRational	AvTimBas;

	AvTimBas.num = 1;
	AvTimBas.den = AV_TIME_BASE;

	AVRational	TimBas;

	TimBas.num = 1;
	TimBas.den = 1;

	if( mDuration == 0 && mFormatContext->duration != AV_NOPTS_VALUE )
	{
		mDuration = qreal( mFormatContext->duration ) * av_q2d( AvTimBas );
	}

	if( mAudioStream != -1 )
	{
		AVStream		*AudioStream   = mFormatContext->streams[ mAudioStream ];
		AVRational		 AudioTimeBase = AudioStream->time_base;

		if( mDuration == 0 && AudioStream->duration != AV_NOPTS_VALUE )
		{
			mDuration = qreal( AudioStream->duration ) * av_q2d( AudioTimeBase );
		}
	}

	if( mAudioCodecContext != 0 )
	{
		//if( mAudioCodecContext->channel_layout != AV_CH_LAYOUT_STEREO || mAudioCodecContext->sample_rate != 48000 || mAudioCodecContext->sample_fmt != AV_SAMPLE_FMT_S16 )
		{
#if defined( TL_USE_LIB_AV )
#else
			mChannelLayout = mAudioCodecContext->channel_layout;
			mChannels      = mAudioCodecContext->channels;
			mSampleRate    = mAudioCodecContext->sample_rate;
			mSampleFmt     = mAudioCodecContext->sample_fmt;

			if( ( mSwrContext = swr_alloc() ) != 0 )
			{
				if( mAudioCodecContext->channel_layout == 0 )
				{
					switch( mAudioCodecContext->channels )
					{
						case 1:
							av_opt_set_int( mSwrContext, "in_channel_layout", AV_CH_LAYOUT_MONO, 0 );
							av_opt_set_int( mSwrContext, "out_channel_layout", AV_CH_LAYOUT_MONO, 0);
							break;

						case 2:
							av_opt_set_int( mSwrContext, "in_channel_layout", AV_CH_LAYOUT_STEREO, 0 );
							av_opt_set_int( mSwrContext, "out_channel_layout", AV_CH_LAYOUT_STEREO, 0);
							break;

						default:
							Q_ASSERT( "CHANNELS" );
							break;
					}
				}
				else
				{
					av_opt_set_int( mSwrContext, "in_channel_layout",  mAudioCodecContext->channel_layout, 0 );
					av_opt_set_int( mSwrContext, "out_channel_layout", mAudioCodecContext->channel_layout, 0 );
				}

				av_opt_set_int( mSwrContext, "in_sample_rate",     mAudioCodecContext->sample_rate,    0);
				av_opt_set_sample_fmt( mSwrContext, "in_sample_fmt",  mAudioCodecContext->sample_fmt,  0);

				av_opt_set_int( mSwrContext, "out_sample_rate",    48000,                 0);
				av_opt_set_sample_fmt( mSwrContext, "out_sample_fmt", AV_SAMPLE_FMT_FLTP, 0);

				mAudPrv.resize( mChannels );

				mAD.mAudDat.resize( mChannels );

				mAudBuf.resize( mChannels );

				int	Error;

				if( ( Error = swr_init( mSwrContext ) ) != 0 )
				{
					swr_free( &mSwrContext );

					return( false );
				}
			}
#endif
		}
	}

	return( true );
}

qreal MediaAudioProcessor::timeToWaveformIdx( qreal pTimeStamp ) const
{
	return( ( pTimeStamp * 48000.0 ) / AUD_INC );
}

qreal MediaAudioProcessor::wavL( qreal pTimeStamp ) const
{
	Q_ASSERT( pTimeStamp >= 0 );

	QMutexLocker		Lock( &mAudDatLck );

//	if( mAudPreloaded )
//	{
//		return( 0.0 );
//	}

	if( mAudPrv.isEmpty() )
	{
		return( 0.0 );
	}

	const AudPrv	&AP = mAudPrv[ 0 ];

	if( AP.mAudWav.isEmpty() )
	{
		return( 0.0 );
	}

	qint64		WavIdx = qMin( qreal( AP.mAudWav.size() - 1 ), timeToWaveformIdx( pTimeStamp ) );

	Q_ASSERT( WavIdx >= 0 );

	if( AP.mAudWav.size() <= WavIdx )
	{
		return( 0 );
	}

	return( AP.mAudWav.at( WavIdx ) );
}

qreal MediaAudioProcessor::wavR( qreal pTimeStamp ) const
{
	Q_ASSERT( pTimeStamp >= 0 );

	QMutexLocker		Lock( &mAudDatLck );

#if 0
	qint64		WavIdx;

	if( mAudPreloaded )
	{
		WavIdx = pTimeStamp * 48000;

		for( QList<AudDat>::const_iterator it = mAudDat.begin() ; it != mAudDat.end() ; it++ )
		{
			const AudDat		&AD = *it;

			if( WavIdx < AD.mAudPts )
			{
				continue;
			}

			if( WavIdx >= AD.mAudPts + AD.mAudSmp )
			{
				continue;
			}

			WavIdx = WavIdx - AD.mAudPts;
			WavIdx = WavIdx * 2;

			return( qreal( ((qint16 *)AD.mAudDat)[ WavIdx + 1 ] ) / 32768.0 );
		}

		return( 0.0 );
	}
#endif

	if( mAudPrv.size() < 2 )
	{
		return( 0.0 );
	}

	const AudPrv	&AP = mAudPrv[ 1 ];

	if( AP.mAudWav.isEmpty() )
	{
		return( 0.0 );
	}

	qint64		WavIdx = qMin( qreal( AP.mAudWav.size() - 1 ), timeToWaveformIdx( pTimeStamp ) );

	Q_ASSERT( WavIdx >= 0 );

	if( AP.mAudWav.size() <= WavIdx )
	{
		return( 0 );
	}

	return( AP.mAudWav.at( WavIdx ) );
}

void MediaAudioProcessor::mixAudio( qint64 pSamplePosition, qint64 pSampleCount, int pChannelOffset, int pChannelCount, void **pBuffers, float pVol ) const
{
	if( !mAudDatLck.tryLock() )
	{
		return;
	}

//	QMutexLocker		Lock( &mAudDatLck );

	//qDebug() << "addData" << pSamplePosition << pSampleCount;

	qint64				SmpCpy = 0;

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
		qint64		AudLen = qMin( ( AD.mAudPts + AD.mAudSmp ) - AudPos, ( pSamplePosition + pSampleCount ) - AudPos );

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

		//qDebug() << SrcPos << QString( "(%1)" ).arg( AD.mAudSmp ) << "->" << DstPos << AudLen;

		for( int i = 0 ; i < pChannelCount ; i++ )
		{
			if( AD.mAudDat.size() <= pChannelOffset + i )
			{
				continue;
			}

			const float	*Src1 = reinterpret_cast<const float *>( AD.mAudDat[ pChannelOffset + i ] ) + SrcPos;
			float		*Src2 = reinterpret_cast<float *>( pBuffers[ i ] ) + DstPos;

			for( int j = 0 ; j < AudLen ; j++ )
			{
				Src2[ j ] += Src1[ j ] * pVol;
			}
		}

		SmpCpy += AudLen;

		if( SmpCpy == pSampleCount )
		{
			break;
		}
	}

//	if( SmpCpy != pSampleCount )
//	{
//		qDebug() << "addData16:" << pSamplePosition << "-" << SmpCpy << "!=" << pSampleCount;
//	}

	mAudDatLck.unlock();
}

bool MediaAudioProcessor::preloaded() const
{
	return( mPreloaded );
}

void MediaAudioProcessor::process( void )
{
	mProcessAborted = false;

	qreal	FileSize = QFileInfo( mFileName ).size();

	if( FileSize <= 0 )
	{
		return;
	}

	qDebug() << "Processing:" << mFileName;

	qreal	FilePos  = 0;

	mAudBuf.reserve( 48000 * sizeof( float ) );

	avformat_seek_file( mFormatContext, -1, 0, 0, 0, 0 );

	if( mAudioCodecContext != 0 )
	{
		avcodec_flush_buffers( mAudioCodecContext );
	}

	av_packet_unref( &mPacket );

	mPacketSize = 0;

	//mAD.mAudDat = 0;
	mAD.mAudPts = 0;
	mAD.mAudSmp = 0;
	mAD.mAudLen = 0;

	int			BytesRead;

	//qint64		TimeS = QDateTime::currentMSecsSinceEpoch();

	while( !mErrorState && !mProcessAborted )
	{
		if( mPacketSize == 0 )
		{
			av_packet_unref( &mPacket );

			int			 ReadError;

			if( ( ReadError = av_read_frame( mFormatContext, &mPacket ) ) < 0 )
			{
				if( ReadError != AVERROR_EOF )
				{
					qWarning() << av_err( "av_read_frame", ReadError );

					mErrorState = true;

					//return;
				}

				break;
			}

			mPacketSize = mPacket.size;

			FilePos += mPacketSize;
		}

		mFrameFinished = 0;

		while( !mErrorState && !mProcessAborted && mPacketSize > 0 )
		{
			if( mPacket.stream_index == mAudioStream )
			{
				if( ( BytesRead = avcodec_decode_audio4( mAudioCodecContext, mFrameSrc, &mFrameFinished, &mPacket ) ) < 0 )
				{
					qWarning() << av_err( "avcodec_decode_audio4", BytesRead );

					mErrorState = true;

					//return;
				}
				else if( mFrameFinished != 0 )
				{
					processAudioFrame();
				}
			}
			else
			{
				BytesRead = mPacketSize;
			}

			mPacketSize = qMax( 0, mPacketSize - BytesRead );
		}
	}

	av_packet_unref( &mPacket );

	mPacketSize = 0;

	if( !mProcessAborted )
	{
		while( ( BytesRead = avcodec_decode_audio4( mAudioCodecContext, mFrameSrc, &mFrameFinished, &mPacket ) ) > 0 )
		{
			if( mFrameFinished != 0 )
			{
				processAudioFrame();
			}
		}
	}

	if( mOptions.testFlag( Preload ) && !mPreloaded )
	{
		if( mAD.mAudSmp > 0 )
		{
			int		bufsze;
			int		linsze;

			if( ( bufsze = av_samples_alloc( mAD.mAudDat.data(), &linsze, mChannels, mAD.mAudSmp, AV_SAMPLE_FMT_FLTP, 0 ) ) >= 0 )
			{
				for( int i = 0 ; i < mChannels ; i++ )
				{
					memcpy( mAD.mAudDat[ i ], mAudBuf[ i ].data(), mAD.mAudLen );
				}

				mAudDat.append( mAD );
			}

			for( int i = 0 ; i < mChannels ; i++ )
			{
				mAudBuf[ i ].clear();

				mAD.mAudDat[ i ] = 0;
			}

			mAD.mAudPts += mAD.mAudSmp;

			mAD.mAudLen = 0;
			mAD.mAudSmp = 0;
		}
	}

	for( int i = 0 ; i < mChannels ; i++ )
	{
		mAudBuf[ i ].clear();
	}

	mPreloaded = mOptions.testFlag( Preload );

	for( int c = 0 ; c < mChannels ; c++ )
	{
		AudPrv	&AP = mAudPrv[ c ];

		if( AP.mAudMax > 0.0f && AP.mAudMax < 1.0f )
		{
			for( int i = 0 ; i < AP.mAudWav.size() ; i++ )
			{
				AP.mAudWav[ i ] /= AP.mAudMax;
			}
		}
	}

	mCalculated = true;

	//qint64		TimeE = QDateTime::currentMSecsSinceEpoch();

	//qDebug() << "Audio Process Time:" << TimeE - TimeS;

	qDebug() << "Finished Processing:" << mFileName;
}

void MediaAudioProcessor::abort()
{
	mProcessAborted = true;
}

/*

void SegmentVideo::calculateWaveform()
{
	if( mAudioCodecContext->channel_layout == 0 )
	{
		switch( mAudioCodecContext->channels )
		{
			case 1:
				av_opt_set_int( SWR, "in_channel_layout", AV_CH_LAYOUT_MONO, 0 );
				break;

			case 2:
				av_opt_set_int( SWR, "in_channel_layout", AV_CH_LAYOUT_STEREO, 0 );
				break;

			default:
				Q_ASSERT( "CHANNELS" );
				break;
		}
	}
	else
	{
		av_opt_set_int( SWR, "in_channel_layout",  mAudioCodecContext->channel_layout, 0 );
	}

	av_opt_set_int( SWR, "in_sample_rate",     mAudioCodecContext->sample_rate,    0);
	av_opt_set_sample_fmt( SWR, "in_sample_fmt",  mAudioCodecContext->sample_fmt,  0);

	av_opt_set_int( SWR, "out_channel_layout", AV_CH_LAYOUT_STEREO,  0);
	av_opt_set_int( SWR, "out_sample_rate",    48000,                0);
	av_opt_set_sample_fmt( SWR, "out_sample_fmt", AV_SAMPLE_FMT_FLTP, 0);

	if( swr_init( SWR ) != 0 )
	{
		swr_free( &SWR );

		return;
	}
#endif

#if defined( TL_USE_LIB_AV )
#else
						if( mChannelLayout == mAudioCodecContext->channel_layout && mChannels == mAudioCodecContext->channels && mSampleRate == mAudioCodecContext->sample_rate && mSampleFmt == mAudioCodecContext->sample_fmt )
						{
							int out_samples = av_rescale_rnd( swr_get_delay( SWR, mAudioCodecContext->sample_rate ) + mFrameSrc->nb_samples, 48000, mAudioCodecContext->sample_rate, AV_ROUND_UP);
							int dst_bufsize = av_samples_get_buffer_size( 0, 2, out_samples, AV_SAMPLE_FMT_FLTP, 1 );

							AudBuf.resize( qMax( out_samples * 2, AudBuf.size() ) );

							uint8_t		*OutPtr = (uint8_t *)AudBuf.data();

							out_samples = swr_convert( SWR, &OutPtr, out_samples, (const uint8_t **)mFrameSrc->data, mFrameSrc->nb_samples );
							dst_bufsize = av_samples_get_buffer_size( 0, 2, out_samples, AV_SAMPLE_FMT_FLTP, 1 );

							for( int i = 0 ; i < out_samples ; i++ )
							{
								float		l = AudBuf.at( ( i * 2 ) + 0 );
								float		r = AudBuf.at( ( i * 2 ) + 1 );

								AudSmL = qMax( AudSmL, l * l );
								AudSmR = qMax( AudSmR, r * r );

								AudPts += 1.0;

								if( AudPts >= AudNxt )
								{
									AudMaxL = qMax( AudSmL, AudMaxL );
									AudMaxR = qMax( AudSmR, AudMaxR );

									mAudWavL.append( AudSmL );
									mAudWavR.append( AudSmR );

									AudSmL  = 0;
									AudSmR  = 0;
									AudNxt += AUD_INC;
								}
							}
						}
#endif
					}
				}
				else
				{
					BytesRead = mPacketSize;
				}

				mPacketSize = qMax( 0, mPacketSize - BytesRead );

				QApplication::processEvents();
			}
		}

		av_packet_unref( &mPacket );

		mPacketSize = 0;

		while( ( BytesRead = avcodec_decode_audio4( mAudioCodecContext, mFrameSrc, &mFrameFinished, &mPacket ) ) > 0 )
		{
			if( mFrameFinished != 0 )
			{
				int out_samples = av_rescale_rnd( swr_get_delay( SWR, mAudioCodecContext->sample_rate ) + mFrameSrc->nb_samples, 48000, mAudioCodecContext->sample_rate, AV_ROUND_UP);
				int dst_bufsize = av_samples_get_buffer_size( 0, 2, out_samples, AV_SAMPLE_FMT_S16, 1 );

				AudBuf.resize( qMax( out_samples * 2, AudBuf.size() ) );

				uint8_t		*OutPtr = (uint8_t *)AudBuf.data();

				out_samples = swr_convert( SWR, &OutPtr, out_samples, (const uint8_t **)mFrameSrc->data, mFrameSrc->nb_samples );
				dst_bufsize = av_samples_get_buffer_size( 0, 2, out_samples, AV_SAMPLE_FMT_FLTP, 1 );

				for( int i = 0 ; i < out_samples ; i++ )
				{
					float		l = AudBuf.at( ( i * 2 ) + 0 );
					float		r = AudBuf.at( ( i * 2 ) + 1 );

					AudSmL = qMax( AudSmL, l * l );
					AudSmR = qMax( AudSmR, r * r );

					AudPts += 1.0;

					if( AudPts >= AudNxt )
					{
						AudMaxL = qMax( AudSmL, AudMaxL );
						AudMaxR = qMax( AudSmR, AudMaxR );

						mAudWavL.append( AudSmL );
						mAudWavR.append( AudSmR );

						AudSmL  = 0;
						AudSmR  = 0;
						AudNxt += AUD_INC;
					}
				}
			}
		}

		ProgressDialog.setValue( FileSize / 1024 );
	}

	if( AudMaxL > 0.0f && AudMaxL < 1.0f )
	{
		for( int i = 0 ; i < mAudWavL.size() ; i++ )
		{
			mAudWavL[ i ] /= AudMaxL;
		}
	}

	if( AudMaxR > 0.0f && AudMaxR < 1.0f )
	{
		for( int i = 0 ; i < mAudWavR.size() ; i++ )
		{
			mAudWavR[ i ] /= AudMaxR;
		}
	}
}

*/
