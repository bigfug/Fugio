#include "ndisendnode.h"

#include <fugio/performance.h>
#include <fugio/audio/audio_producer_interface.h>
#include <fugio/context_signals.h>

NDISendNode::NDISendNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mProducerInstance( 0 )
{
	FUGID( PIN_INPUT_NAME,		"DF437432-F0A0-4048-9BC5-1C29FF534E14" );
	FUGID( PIN_INPUT_GROUPS,	"20EE221E-DB44-4A51-ACE8-D535503C23E5" );
	FUGID( PIN_INPUT_FPS,		"22EE69E2-4781-4C37-9EB9-F3DE647653A9" );
	FUGID( PIN_INPUT_ASPECT,	"FCAE2A96-5619-482D-8B4A-547A8B05BFF0" );
	FUGID( PIN_INPUT_VIDEO,		"74CD7ECC-A7A5-4131-9F8D-57F6DF064A2F" );
	FUGID( PIN_INPUT_AUDIO,		"959BCF7C-4E1D-4E0A-999D-1AFEFC18D2D8" );

	mPinInputName = pinInput( "Name", PIN_INPUT_NAME );

	mPinInputGroups = pinInput( "Groups", PIN_INPUT_GROUPS );

	mPinInputImage = pinInput( "FPS", PIN_INPUT_FPS );

	mPinInputImage = pinInput( "Aspect", PIN_INPUT_ASPECT );

	mPinInputImage = pinInput( "Image", PIN_INPUT_VIDEO );

	mPinInputAudio = pinInput( "Audio", PIN_INPUT_AUDIO );

#if defined( NDI_SUPPORTED )
	mNDIInstance = 0;
#endif
}

bool NDISendNode::deinitialise()
{
#if defined( NDI_SUPPORTED )
	if( mNDIInstance )
	{
		NDIlib_send_destroy( mNDIInstance );

		mNDIInstance = 0;
	}
#endif

	return( NodeControlBase::deinitialise() );
}

void NDISendNode::inputsUpdated( qint64 pTimeStamp )
{
	if( !mProducerInstance || mPinInputAudio->isUpdated( pTimeStamp ) )
	{
		fugio::AudioProducerInterface	*API = input<fugio::AudioProducerInterface *>( mPinInputAudio );

		if( !API || ( mProducerInstance && ( !mProducerInstance->isValid() || ( API && !API->isValid( mProducerInstance ) ) ) ) )
		{
			delete mProducerInstance;

			mProducerInstance = nullptr;
		}

		if( API && API->audioChannels() > 0 && !mProducerInstance )
		{
			mProducerInstance = API->audioAllocInstance( API->audioSampleRate(), fugio::AudioSampleFormat::Format32FS, API->audioChannels() );

			if( mProducerInstance )
			{
				mSamplePosition = 0;

				connect( mNode->context()->qobject(), SIGNAL(frameProcess(qint64)), this, SLOT(contextFrameStart(qint64)) );
			}
		}

		if( !mProducerInstance )
		{
			disconnect( mNode->context()->qobject(), SIGNAL(frameProcess(qint64)), this, SLOT(contextFrameStart(qint64)) );
		}
	}

#if defined( NDI_SUPPORTED )
	if( !mNDIInstance || mPinInputName->isUpdated( pTimeStamp ) || mPinInputGroups->isUpdated( pTimeStamp ) )
	{
		if( mNDIInstance )
		{
			NDIlib_send_destroy( mNDIInstance );

			mNDIInstance = 0;
		}

		mNDIName   = variant( mPinInputName ).toString();
		mNDIGroups = variant( mPinInputGroups ).toString();

		NDIlib_send_create_t create_params_Send;

		create_params_Send.p_ndi_name  = ( mNDIName.isEmpty() ? "Fugio" : mNDIName.toUtf8().constData() );
		create_params_Send.p_groups    = ( mNDIGroups.isEmpty() ? nullptr : mNDIGroups.toUtf8().constData() );;
		create_params_Send.clock_video = true;
		create_params_Send.clock_audio = true;

		mNDIInstance = NDIlib_send_create( &create_params_Send );
	}
#endif

#if defined( NDI_SUPPORTED )
	if( !mNDIInstance )
	{
		return;
	}
#endif

	if( mPinInputImage->isUpdated( pTimeStamp ) )
	{
		fugio::Image	SrcImg = variant<fugio::Image>( mPinInputImage );

		if( SrcImg.isValid() )
		{
			if( SrcImg.format() != fugio::ImageFormat::BGRA8 && SrcImg.format() != fugio::ImageFormat::RGBA8 && SrcImg.format() != fugio::ImageFormat::YUYV422 )
			{
				return;
			}

			fugio::Performance		Perf( mNode, "inputsUpdated", pTimeStamp );

#if defined( NDI_SUPPORTED )
			// Synchronise sending

			NDIlib_send_send_video( mNDIInstance, NULL );

			NDIlib_video_frame_t		VideoFrame;

			memset( &VideoFrame, 0, sizeof( VideoFrame ) );

			switch( SrcImg.format() )
			{
				case fugio::ImageFormat::BGRA8:
					VideoFrame.FourCC = NDIlib_FourCC_type_BGRA;
					break;

				case fugio::ImageFormat::RGBA8:
					VideoFrame.FourCC = NDIlib_FourCC_type_RGBA;
					break;

				case fugio::ImageFormat::YUYV422:
					VideoFrame.FourCC = NDIlib_FourCC_type_UYVY;
					break;

				default:
					return;
			}

			VideoFrame.xres = SrcImg.width();
			VideoFrame.yres = SrcImg.height();
			VideoFrame.frame_rate_N = 25;
			VideoFrame.frame_rate_D = 1;
			VideoFrame.picture_aspect_ratio = float( VideoFrame.xres ) / float( VideoFrame.yres );
			VideoFrame.frame_format_type = NDIlib_frame_format_type_progressive;
			VideoFrame.timecode = 0LL;
			VideoFrame.line_stride_in_bytes = SrcImg.lineSize( 0 );

			mImageBuffer.resize( SrcImg.bufferSize( 0 ) );

			memcpy( mImageBuffer.data(), SrcImg.buffer( 0 ), mImageBuffer.size() );

			VideoFrame.p_data = mImageBuffer.data();

			NDIlib_send_send_video_async( mNDIInstance, &VideoFrame );
#endif
		}
	}
}

void NDISendNode::contextFrameStart( qint64 pTimeStamp )
{
	fugio::AudioProducerInterface	*API = input<fugio::AudioProducerInterface *>( mPinInputAudio );

	if( !mProducerInstance || !API )
	{
		return;
	}

	const int	ChnCnt = mProducerInstance->channels();
	const int	SmpCnt = 2400;
	const int	ChnSze = SmpCnt * sizeof( float );
	const int	BufCnt = SmpCnt * ChnCnt;

	const qint64	CurPos = ( mNode->context()->global()->timestamp() * mProducerInstance->sampleRate() ) / 1000;

	if( CurPos - mSamplePosition > mProducerInstance->sampleRate() )
	{
		mSamplePosition = CurPos - SmpCnt - API->audioLatency();
	}

	if( CurPos - mSamplePosition < SmpCnt )
	{
		return;
	}

	fugio::Performance	P( mNode, "contextFrameStart", pTimeStamp );

	if( mAudioBuffer.size() != BufCnt )
	{
		mAudioBuffer.resize( BufCnt );
	}

	memset( mAudioBuffer.data(), 0, ChnCnt * ChnSze );

	QVector<float *>	AudPtr( ChnCnt );

	for( int i = 0 ; i < ChnCnt ; i++ )
	{
		AudPtr[ i ] = static_cast<float *>( mAudioBuffer.data() ) + ( SmpCnt * i );
	}

	mProducerInstance->audio( mSamplePosition, SmpCnt, 0, ChnCnt, reinterpret_cast<void **>( AudPtr.data() ) );

#if defined( NDI_SUPPORTED )
	NDIlib_audio_frame_t		AudioFrame;

	memset( &AudioFrame, 0, sizeof( AudioFrame ) );

	AudioFrame.channel_stride_in_bytes = ChnSze;
	AudioFrame.no_channels = ChnCnt;
	AudioFrame.no_samples  = SmpCnt;
	AudioFrame.p_data      = mAudioBuffer.data();
	AudioFrame.sample_rate = mProducerInstance->sampleRate();
	AudioFrame.timecode    = 0;

	NDIlib_send_send_audio( mNDIInstance, &AudioFrame );
#endif

	mSamplePosition += SmpCnt;
}
