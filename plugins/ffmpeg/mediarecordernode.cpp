#include "mediarecordernode.h"

#include <QSettings>
#include <QPushButton>
#include <QDockWidget>
#include <QMainWindow>
#include <QDebug>
#include <QPainter>
#include <QFile>
#include <QFileInfo>

#include <fugio/global_interface.h>
#include <fugio/context_interface.h>
#include <fugio/image/image.h>
#include <fugio/audio/audio_producer_interface.h>
#include <fugio/context_signals.h>
#include <fugio/audio/audio_instance_base.h>
#include <fugio/core/uuid.h>
#include <fugio/file/uuid.h>
#include <fugio/editor_interface.h>

#include "mediapreset/mediapresetinterface.h"

#include "mediarecorderform.h"

#include "ffmpegplugin.h"

#include <fugio/utils.h>

//#define RECORD_RAW_OUTPUT

MediaRecorderNode::MediaRecorderNode( QSharedPointer<fugio::NodeInterface> pNode ) :
	NodeControlBase( pNode ), mDockWidget( 0 ), mDockForm( 0 ), mDockArea( Qt::RightDockWidgetArea )
{
	FUGID( PIN_INPUT_FILENAME, "c997473a-2016-466b-9128-beacb99870a2" );
	FUGID( PIN_INPUT_IMAGE, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_INPUT_AUDIO, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );
	FUGID( PIN_INPUT_START, "261cc653-d7fa-4c34-a08b-3603e8ae71d5" );
	FUGID( PIN_INPUT_DURATION, "249f2932-f483-422f-b811-ab679f006381" );
	FUGID( PIN_INPUT_RECORD, "51297977-7b4b-4e08-9dea-89a8add4abe0" );

	FUGID( PIN_OUTPUT_FILENAME, "5064e449-8b0b-4447-9009-c81997f754ef" );
	FUGID( PIN_OUTPUT_IMAGE_SIZE, "ce8d578e-c5a4-422f-b3c4-a1bdf40facdb" );
	FUGID( PIN_OUTPUT_STARTED, "e6bf944e-5f46-4994-bd51-13c2aa6415b7" );
	FUGID( PIN_OUTPUT_FINISHED, "a2bbf374-0dc8-42cb-b85a-6a43b58a348f" );

	mPinInputFilename = pinInput( "Filename", PIN_INPUT_FILENAME );

	mPinInputImage = pinInput( "Image", PIN_INPUT_IMAGE );

	mPinInputAudio = pinInput( "Audio", PIN_INPUT_AUDIO );

	mPinInputStartTime = pinInput( "Start Time", PIN_INPUT_START );

	mPinInputDuration = pinInput( "Duration", PIN_INPUT_DURATION );

	mPinInputRecord = pinInput( "Record", PIN_INPUT_RECORD );

	mValOutputFilename = pinOutput<fugio::FilenameInterface *>( "Filename", mPinOutputFilename, PID_FILENAME, PIN_OUTPUT_FILENAME );

	mValOutputImageSize = pinOutput<fugio::VariantInterface *>( "Image Size", mPinOutputImageSize, PID_SIZE, PIN_OUTPUT_IMAGE_SIZE );

	pinOutput<fugio::PinControlInterface *>( "Started", mPinOutputStarted, PID_TRIGGER, PIN_OUTPUT_STARTED );
	pinOutput<fugio::PinControlInterface *>( "Finished", mPinOutputFinished, PID_TRIGGER, PIN_OUTPUT_FINISHED );

	mPinInputImage->setDescription( tr( "The input image to record" ) );

	mPinInputAudio->setDescription( tr( "The input audio to record" ) );

	mFrameCount			= 0;
	mFrameCountAudio    = 0;

#if defined( FFMPEG_SUPPORTED )
	mOutputFormat		= 0;
	mFormatContext		= 0;
	mStreamVideo		= 0;
	mStreamAudio        = 0;
	mScaleContext		= 0;
	mCodecContextVideo	= 0;
	mCodecVideo			= 0;
	mCodecContextAudio  = 0;
	mCodecAudio         = 0;
	mOptionsAudio       = 0;
	mOptionsVideo       = 0;
	mFrameVideo         = 0;
	mFrameAudio         = 0;

#if defined( TL_USE_LIB_AV )
#else
	mSwrContext			= 0;
#endif

#endif

	mMediaPreset        = 0;

	mAudioInstance		= 0;

	mTimeStart    = 0;
	mTimeDuration = 30.0;
	mTimeEnd      = 0;

	mQuality = 0.75;
	mSpeed   = 0.5;

	mFrameScale = FRAME_STRETCH;

}

bool MediaRecorderNode::initialise()
{
	if( !NodeControlBase::initialise() )
	{
		return( false );
	}

	if( !mDockWidget )
	{
		fugio::EditorInterface	*EI = qobject_cast<fugio::EditorInterface *>( mNode->context()->global()->findInterface( IID_EDITOR ) );

		if( EI )
		{
			if( ( mDockWidget = new QDockWidget( QString( "Media Recorder: %1" ).arg( mNode->name() ), EI->mainWindow() ) ) == 0 )
			{
				return( false );
			}

			if( ( mDockForm = new MediaRecorderForm( *this, mDockWidget ) ) == 0 )
			{
				return( false );
			}

			mDockWidget->setWidget( mDockForm );

			mDockWidget->setObjectName( fugio::utils::uuid2string( mNode->uuid() ) );

			EI->mainWindow()->addDockWidget( mDockArea, mDockWidget );

			connect( this, SIGNAL(destroyed()), mDockWidget, SLOT(deleteLater()) );
		}
	}

	return( true );
}

bool MediaRecorderNode::deinitialise()
{
	if( mDockWidget )
	{
		mDockWidget->deleteLater();

		mDockWidget = 0;
	}

	return( NodeControlBase::deinitialise() );
}

void MediaRecorderNode::loadSettings( QSettings &pSettings )
{
	NodeControlBase::loadSettings( pSettings );

	mMediaPresetName = pSettings.value( "preset", mMediaPresetName ).toString();

	mFrameScale = FrameScale( pSettings.value( "frameScale", mFrameScale ).toInt() );

	mSpeed   = pSettings.value( "speed", mSpeed ).toReal();
	mQuality = pSettings.value( "quality", mQuality ).toReal();

	mTimeStart = pSettings.value( "timeStart", mTimeStart ).toReal();
	mTimeDuration = pSettings.value( "timeDuration", mTimeDuration ).toReal();
	mTimeEnd = pSettings.value( "timeEnd", mTimeEnd ).toReal();
}

void MediaRecorderNode::saveSettings( QSettings &pSettings ) const
{
	NodeControlBase::saveSettings( pSettings );

	pSettings.setValue( "preset", mMediaPresetName );

	pSettings.setValue( "frameScale", int( mFrameScale ) );

	pSettings.setValue( "speed", mSpeed );
	pSettings.setValue( "quality", mQuality );

	pSettings.setValue( "timeStart", mTimeStart );
	pSettings.setValue( "timeDuration", mTimeDuration );
	pSettings.setValue( "timeEnd", mTimeEnd );
}

QWidget *MediaRecorderNode::gui()
{
	QPushButton		*GUI = new QPushButton( "Edit..." );

	connect( GUI, SIGNAL(clicked()), this, SLOT(onFormClicked()) );

	return( GUI );
}

void MediaRecorderNode::inputsUpdated( qint64 pTimeStamp )
{
	if( !pTimeStamp )
	{
		return;
	}

	if( mPinInputFilename->isUpdated( pTimeStamp ) )
	{
		QFileInfo		FileInfo;

		if( fugio::FilenameInterface *F = input<fugio::FilenameInterface *>( mPinInputFilename ) )
		{
			FileInfo = QFileInfo( F->filename() );
		}
		else
		{
			FileInfo = QFileInfo( variant( mPinInputFilename ).toString() );
		}

		if( mMediaPreset )
		{
			if( FileInfo.suffix().toLower() != mMediaPreset->fileExt() )
			{
				FileInfo = QFileInfo( FileInfo.filePath().append( '.' ).append( mMediaPreset->fileExt() ) );
			}
		}

		mFilename = FileInfo.filePath();
	}

	if( mPinInputStartTime->isUpdated( pTimeStamp ) )
	{
		mTimeStart = qMax( variant( mPinInputStartTime ).toReal(), 0.0 );
	}

	if( mPinInputDuration->isUpdated( pTimeStamp ) )
	{
		mTimeDuration = qMax( variant( mPinInputStartTime ).toReal(), 0.0 );
	}

	if( mPinInputRecord->isUpdated( pTimeStamp ) )
	{
		fugio::VariantInterface		*V = input<fugio::VariantInterface *>( mPinInputRecord );

		if( V )
		{
			if( V->variant().toBool() )
			{
				record( mFilename );
			}
			else
			{
				mCancel = true;
			}
		}
		else
		{
			record( mFilename );
		}
	}
}

void MediaRecorderNode::setMediaPreset( MediaPresetInterface *pMediaPreset )
{
	mMediaPreset = pMediaPreset;

	if( mMediaPreset )
	{
		QSize		ImageSize = QSize( mMediaPreset->videoFrameSize() );

		if( mValOutputImageSize->variant().toSize() != ImageSize )
		{
			mValOutputImageSize->setVariant( ImageSize );

			pinUpdated( mPinOutputImageSize );
		}
	}
}

void MediaRecorderNode::onFormClicked( void )
{
	if( mDockWidget && mDockWidget->isHidden() )
	{
		mDockWidget->show();
	}
}

void MediaRecorderNode::record( const QString &pFileName )
{
	int			AVErr = 0;

	if( mMediaPreset == 0 )
	{
		return;
	}

	mFilename = pFileName;

	mTimePrev = mTimeCurr = mTimeStart;

	mFrameCount = 0;
	mFrameCountAudio = 0;

#if defined( FFMPEG_SUPPORTED )

#if defined( TL_USE_LIB_AV )
	return;
#else
	if( ( AVErr = avformat_alloc_output_context2( &mFormatContext, NULL, NULL, pFileName.toLatin1() ) ) < 0 )
	{
		qWarning() << tr( "Couldn't av_guess_format of filename: %1 - %2" ).arg( pFileName ).arg( ffmpegPlugin::av_err( AVErr ) );

		return;
	}
#endif

	if( mFormatContext == 0 )
	{
		return;
	}

	mOutputFormat = mFormatContext->oformat;

	//-------------------------------------------------------------------------
	// Create video stream

	if( mOutputFormat->video_codec != AV_CODEC_ID_NONE )
	{
		fugio::VariantInterface			*Image = input<fugio::VariantInterface *>( mPinInputImage );

		if( !Image )
		{
			return;
		}

		if( ( mCodecVideo = avcodec_find_encoder( mOutputFormat->video_codec ) ) == 0 )
		{
			return;
		}

		if( ( mStreamVideo = avformat_new_stream( mFormatContext, NULL ) ) == 0 )
		{
			return;
		}

		AVRational			TimeBase;

		TimeBase.num = 1;
		TimeBase.den = mMediaPreset->videoFramesPerSecond();

		mCodecContextVideo = avcodec_alloc_context3( mCodecVideo );

		mCodecContextVideo->time_base = mStreamVideo->time_base = TimeBase;

		mMediaPreset->fillVideoCodecContext( mCodecContextVideo );

		if( mCodecContextVideo->codec_id == AV_CODEC_ID_H264 )
		{
			av_dict_set( &mOptionsVideo, "preset", "slow", 0 );
			av_dict_set( &mOptionsVideo, "profile", "baseline", 0 );
			av_dict_set( &mOptionsVideo, "level", "3.0", 0 );
		}

		mMediaPreset->setQuality( mCodecContextVideo, mStreamVideo, mQuality );
		mMediaPreset->setSpeed( mCodecContextVideo, mStreamVideo, mSpeed );

//		/* just for testing, we also add B frames */

//		if( mCodecContextVideo->codec_id == CODEC_ID_MPEG2VIDEO )
//		{
//			mCodecContextVideo->profile = FF_PROFILE_MPEG2_MAIN;

//			mCodecContextVideo->max_b_frames = 2;
//		}

//		/* Needed to avoid using macroblocks in which some coeffs overflow.
//			 * This does not happen with normal video, it just happens here as
//			 * the motion of the chroma plane does not match the luma plane. */

//		if( mCodecContextVideo->codec_id == CODEC_ID_MPEG1VIDEO )
//		{
//			mCodecContextVideo->mb_decision = 2;
//		}

		/* Some formats want stream headers to be separate. */

		if( ( mOutputFormat->flags & AVFMT_GLOBALHEADER ) != 0 )
		{
			mCodecContextVideo->flags |= CODEC_FLAG_GLOBAL_HEADER;
		}
	}

	if( mStreamVideo )
	{
		if( ( AVErr = avcodec_open2( mCodecContextVideo, NULL, &mOptionsVideo ) ) != 0 )
		{
			qWarning() << ffmpegPlugin::av_err( "avcodec_open2", AVErr );

			return;
		}

		avcodec_parameters_from_context( mStreamVideo->codecpar, mCodecContextVideo );

		if( ( mOutputFormat->flags & AVFMT_RAWPICTURE ) != 0 )
		{

		}
	}

	//-------------------------------------------------------------------------
	// Create audio stream

	if( mOutputFormat->audio_codec != AV_CODEC_ID_NONE )
	{
		if( ( mCodecAudio = avcodec_find_encoder( mOutputFormat->audio_codec ) ) == 0 )
		//if( ( mCodecAudio = avcodec_find_encoder( AV_CODEC_ID_MP3 ) ) == 0 )
		{
			return;
		}

		if( ( mStreamAudio = avformat_new_stream( mFormatContext, NULL ) ) == 0 )
		{
			return;
		}

		AVRational		TimeBase;

		TimeBase.num = 1;
		TimeBase.den = mMediaPreset->audioSampleRate();

		mCodecContextAudio = avcodec_alloc_context3( mCodecAudio );

		mCodecContextAudio->time_base = mStreamAudio->time_base = TimeBase;

		mMediaPreset->fillAudioCodecContext( mCodecContextAudio );

		if( mOutputFormat->flags & AVFMT_GLOBALHEADER )
		{
			mCodecContextAudio->flags |= CODEC_FLAG_GLOBAL_HEADER;
		}
	}

	if( mStreamAudio != 0 )
	{
		if( ( AVErr = avcodec_open2( mCodecContextAudio, NULL, &mOptionsAudio ) ) != 0 )
		{
			qWarning() << ffmpegPlugin::av_err( "avcodec_open2", AVErr );

			return;
		}

		avcodec_parameters_from_context( mStreamAudio->codecpar, mCodecContextAudio );
	}

	//-------------------------------------------------------------------------
	// Open video

	av_dump_format( mFormatContext, 0, pFileName.toLatin1(), 1 );

	//return;

	if( ( mOutputFormat->flags & AVFMT_NOFILE ) == 0 )
	{
		if( avio_open2( &mFormatContext->pb, pFileName.toLatin1(), AVIO_FLAG_WRITE, NULL, NULL ) < 0 )
		{
			return;
		}
	}

	if( avformat_write_header( mFormatContext, NULL ) != 0 )
	{
		return;
	}

	if( mStreamVideo != 0 )
	{
		if( ( mFrameVideo = av_frame_alloc() ) == 0 )
		{
			return;
		}

		av_frame_unref( mFrameVideo );

		int AlcRet = av_image_alloc( mFrameVideo->data, mFrameVideo->linesize, mCodecContextVideo->width, mCodecContextVideo->height, mCodecContextVideo->pix_fmt, 32 );

		if( AlcRet < 0 )
		{
			return;
		}

		mFrameVideo->width  = mCodecContextVideo->width;
		mFrameVideo->height = mCodecContextVideo->height;
		mFrameVideo->pts    = 0;
		mFrameVideo->format = mCodecContextVideo->pix_fmt;
	}

	//-------------------------------------------------------------------------

	if( mStreamAudio != 0 )
	{
		if( ( mFrameAudio = av_frame_alloc() ) == 0 )
		{
			return;
		}

		av_frame_unref( mFrameAudio );

		mFrameAudio->nb_samples     = mCodecContextAudio->frame_size;
		mFrameAudio->sample_rate    = mCodecContextAudio->sample_rate;
		mFrameAudio->format         = mCodecContextAudio->sample_fmt;
		mFrameAudio->channels       = mCodecContextAudio->channels;
		mFrameAudio->channel_layout = mCodecContextAudio->channel_layout;

		if( mFrameAudio->nb_samples == 0 )
		{
			mFrameAudio->nb_samples = 512;
		}

		int NumBytes = av_samples_get_buffer_size( NULL, mFrameAudio->channels, mFrameAudio->nb_samples, AVSampleFormat( mFrameAudio->format ), 0 );

		uint8_t		*Buffer = (uint8_t *)av_malloc( NumBytes );

		if( !Buffer )
		{
			av_free( mFrameAudio );

			mFrameAudio = 0;

			return;
		}

		avcodec_fill_audio_frame( mFrameAudio, mCodecContextAudio->channels, mCodecContextAudio->sample_fmt, (const uint8_t*)Buffer, NumBytes, 0 );

		mAudioOffsetInput  = 0;
		mAudioOffsetOutput = 0;

		mFrameCountAudio   = 0;
	}

	//-------------------------------------------------------------------------

	if( mStreamAudio )
	{
		fugio::AudioProducerInterface	*A = input<fugio::AudioProducerInterface *>( mPinInputAudio );

		if( A )
		{
			mAudioInstance = A->audioAllocInstance( 48000.0, fugio::AudioSampleFormat::Format32FS, mFrameAudio->channels );
		}
	}

	//-------------------------------------------------------------------------

	memset( &mPacketVideo, 0, sizeof( mPacketVideo ) );

	av_init_packet( &mPacketVideo );

	mPacketVideo.flags |= AV_PKT_FLAG_KEY;

	memset( &mPacketAudio, 0, sizeof( mPacketAudio ) );

	av_init_packet( &mPacketAudio );

	mPacketAudio.flags |= AV_PKT_FLAG_KEY;

	if( mStreamVideo != 0 )
	{
		mPacketVideo.stream_index = mStreamVideo->index;
	}

	if( mStreamAudio != 0 )
	{
		mPacketAudio.stream_index = mStreamAudio->index;
	}

	mNode->context()->setPlayheadTimerEnabled( false );

	mNode->context()->setPlayheadPosition( 0 );

	mNode->context()->notifyAboutToPlay();

	connect( mNode->context()->qobject(), SIGNAL(frameStart(qint64)), this, SLOT(frameStart(qint64)) );
	connect( mNode->context()->qobject(), SIGNAL(frameEnd(qint64)), this, SLOT(frameEnd(qint64)) );

	mCancel = false;

#if defined( RECORD_RAW_OUTPUT )
	QFile( "G:/record-src.raw" ).open( QFile::WriteOnly | QFile::Truncate );
	QFile( "G:/record-dst.raw" ).open( QFile::WriteOnly | QFile::Truncate );
#endif

#endif
}

void MediaRecorderNode::cancel()
{
	mCancel = true;
}

void MediaRecorderNode::frameStart( qint64 )
{
	if( mTimeCurr == mTimeStart )
	{
		emit recordingStarted();

		pinUpdated( mPinOutputStarted );

#if defined( FFMPEG_SUPPORTED )
		if( mStreamVideo )
		{
			QSize		ImageSize = QSize( mStreamVideo->codecpar->width, mStreamVideo->codecpar->height );

			if( mValOutputImageSize->variant().toSize() != ImageSize )
			{
				mValOutputImageSize->setVariant( ImageSize );

				pinUpdated( mPinOutputImageSize );
			}
		}
#endif

		if( mValOutputFilename->filename() != mFilename )
		{
			mValOutputFilename->setFilename( mFilename );

			pinUpdated( mPinOutputFilename );
		}
	}

	emit recording( mTimeCurr - mTimeStart );

	mNode->context()->playheadPlay( mTimePrev, mTimeCurr );
}

void MediaRecorderNode::frameEnd( qint64 )
{
	recordEntry();
}

void MediaRecorderNode::recordEntry()
{
#if defined( FFMPEG_SUPPORTED )
	const bool		HasVideo = ( mStreamVideo != 0 );
	const bool		HasAudio = ( mStreamAudio != 0 );

	qreal	TimeIncrement = ( 1.0 / 25.0 );

	if( HasVideo )
	{
		TimeIncrement = av_q2d( mCodecContextVideo->time_base );
	}
	else if( HasAudio )
	{
		TimeIncrement = qreal( mFrameAudio->nb_samples ) / qreal( mCodecContextAudio->sample_rate );
	}

	if( HasVideo )
	{
		if( !imageToFrame() )
		{
			return;
		}

		mFrameVideo->pts    = av_rescale_q( mFrameCount, mCodecContextVideo->time_base, mStreamVideo->time_base );
		mFrameVideo->format = mCodecContextVideo->pix_fmt;

		if( ( mFormatContext->flags & AVFMT_RAWPICTURE ) != 0 )
		{
			mPacketVideo.flags			|= AV_PKT_FLAG_KEY;
			mPacketVideo.stream_index	 = mStreamVideo->index;
			mPacketVideo.data			 = (uint8_t *)mFrameVideo;
			mPacketVideo.size			 = sizeof( AVPicture );

			if( av_interleaved_write_frame( mFormatContext, &mPacketVideo ) < 0 )
			{
				av_packet_unref( &mPacketVideo );

				return;
			}
		}
		else
		{
			if( avcodec_send_frame( mCodecContextVideo, mFrameVideo ) == 0 )
			{
				while( avcodec_receive_packet( mCodecContextVideo, &mPacketVideo ) == 0 )
				{
					mPacketVideo.stream_index = mStreamVideo->index;

					av_interleaved_write_frame( mFormatContext, &mPacketVideo );
				}
			}
		}

		mFrameCount++;
	}

	if( HasAudio )
	{
		int			LineSize;

		av_samples_get_buffer_size( &LineSize, mFrameAudio->channels, mFrameAudio->nb_samples, AVSampleFormat( mFrameAudio->format ), 1 );

		QVector<float *>	OutPtr( mBufferAudio.size() );

		while( mAudioOffsetOutput < mTimeCurr * mFrameAudio->sample_rate )
		{
			if( !mSwrContext )
			{
				if( ( mSwrContext = swr_alloc() ) != 0 )
				{
					av_opt_set_int( mSwrContext, "in_channel_layout", AV_CH_LAYOUT_STEREO,  0 );

					av_opt_set_int( mSwrContext, "in_sample_rate", 48000.0, 0 );

					av_opt_set_sample_fmt( mSwrContext, "in_sample_fmt", AV_SAMPLE_FMT_FLTP, 0 );

					if( av_frame_get_channel_layout( mFrameAudio ) == 0 )
					{
						switch( mFrameAudio->channels )
						{
							case 1:
								av_opt_set_int( mSwrContext, "out_channel_layout", AV_CH_LAYOUT_MONO, 0 );
								break;

							case 2:
								av_opt_set_int( mSwrContext, "out_channel_layout", AV_CH_LAYOUT_STEREO, 0 );
								break;

							default:
								Q_ASSERT( "CHANNELS" );
								break;
						}
					}
					else
					{
						av_opt_set_int( mSwrContext, "out_channel_layout", av_frame_get_channel_layout( mFrameAudio ), 0 );
					}

					av_opt_set_int( mSwrContext, "out_sample_rate", mFrameAudio->sample_rate, 0 );

					av_opt_set_sample_fmt( mSwrContext, "out_sample_fmt", AVSampleFormat( mFrameAudio->format ), 0 );

					int	Error;

					if( ( Error = swr_init( mSwrContext ) ) != 0 )
					{
						swr_free( &mSwrContext );

						return;
					}

					mBufferAudio.resize( 2 );

					for( int i = 0 ; i < mBufferAudio.size() ; i++ )
					{
						mBufferAudio[ i ].resize( mFrameAudio->nb_samples );
					}

					mBufferAudioOutput.resize( mBufferAudio.size() );

					OutPtr.resize( mBufferAudio.size() );
				}
			}

			while( mBufferAudioOutput[ 0 ].size() < LineSize )
			{
				for( int i = 0 ; i < mBufferAudio.size() ; i++ )
				{
					memset( mBufferAudio[ i ].data(), 0, mBufferAudio[ i ].size() * sizeof( float ) );

					OutPtr[ i ] = mBufferAudio[ i ].data();
				}

				if( mAudioInstance )
				{
					mAudioInstance->audio( mAudioOffsetInput, mFrameAudio->nb_samples, 0, mFrameAudio->channels, (void **)OutPtr.data() );
				}

#if defined( RECORD_RAW_OUTPUT )
				QFile		F( RECORD_RAW_OUTPUT );

				if( F.open( QIODevice::WriteOnly | QFile::Append ) )
				{
					F.write( (const char *)mBufferAudio[ 0 ].data(), mBufferAudio[ 0 ].size() * sizeof( float ) );

					F.close();
				}
#endif

				int					 out_samples = av_rescale_rnd( swr_get_delay( mSwrContext, 48000.0 ) + mFrameAudio->nb_samples, mFrameAudio->sample_rate, 48000.0, AV_ROUND_UP );

				const uint8_t		**input = (const uint8_t **)OutPtr.constData();
				uint8_t				*output[ AV_NUM_DATA_POINTERS ];
				int					 outlinesize;

				memset( output, 0, sizeof( output ) );

				if( av_samples_alloc( (uint8_t**)&output, &outlinesize, mFrameAudio->channels, out_samples, mCodecContextAudio->sample_fmt, 1 ) >= 0 )
				{
					if( ( out_samples = swr_convert( mSwrContext, output, out_samples, input, mFrameAudio->nb_samples ) ) >= 0 )
					{
						av_samples_get_buffer_size( &outlinesize, mFrameAudio->channels, out_samples, AVSampleFormat( mFrameAudio->format ), 1 );

						for( int i = 0 ; i < mBufferAudioOutput.size() ; i++ )
						{
							if( output[ i ] )
							{
								mBufferAudioOutput[ i ].append( (const char *)output[ i ], outlinesize );
							}
						}

#if defined( RECORD_RAW_OUTPUT )
						QFile		F( "G:/record-dst.raw" );

						if( F.open( QIODevice::WriteOnly | QFile::Append ) )
						{
							F.write( (const char *)output, BufferBytes );

							F.close();
						}
#endif
					}

					av_freep( output );
				}

				mAudioOffsetInput += mFrameAudio->nb_samples;
			}

#if 0
			QFile		F( "G:/record-dst.raw" );

			if( F.open( QIODevice::WriteOnly | QFile::Append ) )
			{
				F.write( mBufferAudioOutput.data(), BytesNeeded );

				F.close();
			}
#endif

			for( int i = 0 ; i < mBufferAudioOutput.size() ; i++ )
			{
				if( mFrameAudio->data[ i ] )
				{
					memcpy( mFrameAudio->data[ i ], mBufferAudioOutput[ i ].data(), LineSize );
				}

				mBufferAudioOutput[ i ].remove( 0, LineSize );
			}

			mFrameAudio->pts = av_rescale_q( mAudioOffsetOutput, mCodecContextAudio->time_base, mStreamAudio->time_base );

			if( avcodec_send_frame( mCodecContextAudio, mFrameAudio ) == 0 )
			{
				while( avcodec_receive_packet( mCodecContextAudio, &mPacketAudio ) == 0 )
				{
					mPacketAudio.stream_index = mStreamAudio->index;

					av_interleaved_write_frame( mFormatContext, &mPacketAudio );
				}
			}

			mAudioOffsetOutput += mFrameAudio->nb_samples;
		}
	}

	if( mTimeCurr > 0.0 )
	{
		mTimePrev += TimeIncrement;
	}

	mTimeCurr += TimeIncrement;

	// TODO: || mNode->context()->tracksPlaying()

	if( mCancel || ( mTimeDuration > 0 && mTimeCurr - mTimeStart >= mTimeDuration ) || ( mTimeDuration <= 0 && mTimeCurr >= mTimeEnd ) )
	{
		if( HasVideo )
		{
			avcodec_send_frame( mCodecContextVideo, NULL );

			while( avcodec_receive_packet( mCodecContextVideo, &mPacketVideo ) == 0 )
			{
				mPacketVideo.stream_index = mStreamVideo->index;

				av_interleaved_write_frame( mFormatContext, &mPacketVideo );
			}

			av_packet_unref( &mPacketVideo );
		}

		if( HasAudio )
		{
			avcodec_send_frame( mCodecContextAudio, NULL );

			while( avcodec_receive_packet( mCodecContextAudio, &mPacketAudio ) == 0 )
			{
				mPacketAudio.stream_index = mStreamAudio->index;

				av_interleaved_write_frame( mFormatContext, &mPacketAudio );
			}

			av_packet_unref( &mPacketAudio );
		}

		av_write_trailer( mFormatContext );

		avio_close( mFormatContext->pb );

		if( HasVideo )
		{
			avcodec_close( mCodecContextVideo );

			//av_free( mCodecContextVideo );

			mCodecContextVideo = 0;

			mCodecVideo = 0;

			mStreamVideo = 0;

			av_frame_free( &mFrameVideo );

			if( mScaleContext != 0 )
			{
				sws_freeContext( mScaleContext );

				mScaleContext = 0;
			}
		}

		if( HasAudio )
		{
			avcodec_close( mCodecContextAudio );

			//av_free( mCodecContextAudio );

			mCodecContextAudio = 0;

			mCodecAudio = 0;

			mStreamAudio = 0;

			av_frame_free( &mFrameAudio );

			if( mSwrContext != 0 )
			{
				swr_free( &mSwrContext );

				mSwrContext = 0;
			}

			mBufferAudio.clear();
		}

		if( mAudioInstance )
		{
			delete mAudioInstance;

			mAudioInstance = 0;
		}

		mOutputFormat = 0;

		avformat_free_context( mFormatContext );

		mFormatContext = 0;

		mNode->context()->stop();

		mNode->context()->setPlayheadTimerEnabled( true );

		disconnect( mNode->context()->qobject(), SIGNAL(frameStart(qint64)), this, SLOT(frameStart(qint64)) );
		disconnect( mNode->context()->qobject(), SIGNAL(frameEnd(qint64)), this, SLOT(frameEnd(qint64)) );

		emit recordingStopped();

		pinUpdated( mPinOutputFinished );
	}
#endif
}

QImage MediaRecorderNode::cropImage( const QImage &pImage, const QSize &pSize )
{
	const int	sw = pImage.width();
	const int	sh = pImage.height();
	const int	dw = pSize.width();
	const int	dh = pSize.height();

	QImage		DstImg;

	if( sw == dw )
	{
		int		of = ( sh - dh ) / 2;

		if( true ) //sh > dh )
		{
			DstImg = pImage.copy( 0, of, dw, dh );
		}
		else
		{
			DstImg = QImage( dw, dh, QImage::Format_RGB888 );

			DstImg.fill( Qt::black );

			if( true )
			{
				QPainter		Painter( &DstImg );

				Painter.drawImage( 0, of, pImage );
			}
		}
	}
	else
	{
		int		of = ( sw - dw ) / 2;

		if( true ) //sw > dw )
		{
			DstImg = pImage.copy( of, 0, dw, dh );
		}
		else
		{
			DstImg = QImage( dw, dh, QImage::Format_RGB888 );

			DstImg.fill( Qt::black );

			if( true )
			{
				QPainter		Painter( &DstImg );

				Painter.drawImage( of, 0, pImage );
			}
		}
	}

	return( DstImg );
}

bool MediaRecorderNode::imageToFrame( void )
{
	QImage				SrcImg = variant( mPinInputImage ).value<fugio::Image>().image();

	static QImage		TmpImg( 10, 10, QImage::Format_ARGB32 );

	if( SrcImg.isNull() )
	{
		SrcImg = TmpImg.copy();
	}

#if defined( FFMPEG_SUPPORTED )
	int		sw = SrcImg.width();
	int		sh = SrcImg.height();
	int		dw = mFrameVideo->width;
	int		dh = mFrameVideo->height;

	if( mFrameScale == FRAME_FIT )	// Fit
	{
		if( sw != dw || sh != dh )
		{
			if( dw - sw > dh - sh )
			{
				if( dh != sh )
				{
					SrcImg = SrcImg.scaledToHeight( dh, Qt::SmoothTransformation );
				}
			}
			else
			{
				if( dw != sw )
				{
					SrcImg = SrcImg.scaledToWidth( dw, Qt::SmoothTransformation );
				}
			}

			if( SrcImg.width() != dw || SrcImg.height() != dh )
			{
				SrcImg = cropImage( SrcImg, QSize( dw, dh ) );
			}
		}
	}
	else if( mFrameScale == FRAME_FILL )			// Fill
	{
		if( sw != dw || sh != dh )
		{
			if( dw - sw < dh - sh )
			{
				if( dh != sh )
				{
					SrcImg = SrcImg.scaledToHeight( dh, Qt::SmoothTransformation );
				}
			}
			else
			{
				if( dw != sw )
				{
					SrcImg = SrcImg.scaledToWidth( dw, Qt::SmoothTransformation );
				}
			}

			if( SrcImg.width() != dw || SrcImg.height() != dh )
			{
				SrcImg = cropImage( SrcImg, QSize( dw, dh ) );
			}
		}
	}

	if( SrcImg.format() != QImage::Format_ARGB32 )
	{
		SrcImg = SrcImg.convertToFormat( QImage::Format_ARGB32 );
	}

	if( mScaleContext == 0 )
	{
		AVPixelFormat		PixFmt;

		switch( SrcImg.format() )
		{
			case QImage::Format_RGB32:
				PixFmt = AV_PIX_FMT_RGB32;
				break;

			case QImage::Format_ARGB32:
			case QImage::Format_ARGB32_Premultiplied:
				PixFmt = AV_PIX_FMT_BGRA;
				break;

			case QImage::Format_RGB555:
			case QImage::Format_ARGB8555_Premultiplied:
				PixFmt = AV_PIX_FMT_RGB555;
				break;

			case QImage::Format_RGB888:
				PixFmt = AV_PIX_FMT_RGB24;
				break;

			case QImage::Format_RGB444:
			case QImage::Format_ARGB4444_Premultiplied:
				PixFmt = AV_PIX_FMT_RGB444;
				break;

			case QImage::Format_RGBX8888:
				PixFmt = AV_PIX_FMT_RGB0;
				break;

			case QImage::Format_RGBA8888:
			case QImage::Format_RGBA8888_Premultiplied:
				PixFmt = AV_PIX_FMT_RGBA;
				break;

			default:
				return( false );
		}

		if( ( mScaleContext = sws_getContext( SrcImg.width(), SrcImg.height(), PixFmt, mCodecContextVideo->width, mCodecContextVideo->height, mCodecContextVideo->pix_fmt, SWS_BILINEAR, 0, 0, 0 ) ) == 0 )
		{
			return( false );
		}
	}

	const uint8_t		*SrcDat[ AV_NUM_DATA_POINTERS ];
	int					 SrcSze[ AV_NUM_DATA_POINTERS ];

	for( int i = 0 ; i < AV_NUM_DATA_POINTERS ; i++ )
	{
		SrcDat[ i ] = 0;
		SrcSze[ i ] = 0;
	}

	SrcDat[ 0 ] = SrcImg.constBits();
	SrcSze[ 0 ] = SrcImg.bytesPerLine();

	sws_scale( mScaleContext, SrcDat, SrcSze, 0, SrcImg.height(), mFrameVideo->data, mFrameVideo->linesize );

	return( true );
#else
	return( false );
#endif
}
