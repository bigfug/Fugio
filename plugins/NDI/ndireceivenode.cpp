#include "ndireceivenode.h"

#include <QPushButton>

#include <fugio/core/uuid.h>
#include <fugio/image/uuid.h>
#include <fugio/audio/uuid.h>

#include <fugio/node_interface.h>
#include <fugio/context_interface.h>
#include <fugio/context_signals.h>

#include <fugio/performance.h>

NDIReceiveNode::NDIReceiveNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_SOURCE,	"DF437432-F0A0-4048-9BC5-1C29FF534E14" );
	FUGID( PIN_OUTPUT_IMAGE,	"20EE221E-DB44-4A51-ACE8-D535503C23E5" );
	FUGID( PIN_OUTPUT_AUDIO,	"74CD7ECC-A7A5-4131-9F8D-57F6DF064A2F" );

	mValInputSource = pinInput<fugio::ChoiceInterface *>( "Source", mPinInputSource, PID_CHOICE, PIN_INPUT_SOURCE );

	mValOutputImage = pinOutput<fugio::VariantInterface *>( "Image", mPinOutputImage, PID_IMAGE, PIN_OUTPUT_IMAGE );

	mValOutputAudio = pinOutput<fugio::AudioProducerInterface *>( "Audio", mPinOutputAudio, PID_AUDIO, PIN_OUTPUT_AUDIO );

#if defined( NDI_SUPPORTED )
	mNDIFind        = 0;
	mNDIFindTimeout = 500;
	mNDIInstance    = 0;
#endif

	mNDIAudioChannels = 0;
	mNDIAudioSampleRate = 0;
	mNDIAudioSampleFormat = fugio::AudioSampleFormat::Format32FS;
}

bool NDIReceiveNode::initialise()
{
	if( !NodeControlBase::initialise() )
	{
		return( false );
	}

#if !defined( NDI_SUPPORTED )
	mNode->setStatus( fugio::NodeInterface::Error );

	return( false );
#endif

#if defined( NDI_SUPPORTED )
	NDIlib_find_create_t find_create;

	find_create.show_local_sources = true;
	find_create.p_groups = NULL;

	// Create the instance
	mNDIFind = NDIlib_find_create2( &find_create );
#endif

	updateNDISources();

	return( true );
}

bool NDIReceiveNode::deinitialise()
{
	disconnect( mNode->context()->qobject(), SIGNAL(frameStart(qint64)), this, SLOT(contextFrameStart(qint64)) );

#if defined( NDI_SUPPORTED )
	if( mNDIInstance )
	{
		NDIlib_recv_destroy( mNDIInstance );

		mNDIInstance = 0;
	}

	if( mNDIFind )
	{
		NDIlib_find_destroy( mNDIFind );

		mNDIFind = 0;
	}

	clearAudio();
#endif

	return( NodeControlBase::deinitialise() );
}

void NDIReceiveNode::inputsUpdated( qint64 pTimeStamp )
{
	if( mPinInputSource->isUpdated( pTimeStamp ) )
	{
#if defined( NDI_SUPPORTED )
		const QString				 SourceName = variant( mPinInputSource ).toString();

		NDIlib_recv_create_t		 RecvCreate;

		memset( &RecvCreate, 0, sizeof( RecvCreate ) );

		RecvCreate.allow_video_fields   = false;
		RecvCreate.bandwidth            = NDIlib_recv_bandwidth_highest;

#if !defined( Q_OS_WIN )
//		RecvCreate.prefer_UYVY          = false;
#endif

		uint32_t					 NDICount = 0;

		const NDIlib_source_t		*NDISources = NDIlib_find_get_current_sources( mNDIFind, &NDICount );

		for( uint32_t i = 0 ; i < NDICount ; i++ )
		{
			if( SourceName != QString( NDISources[ i ].p_ndi_name ) )
			{
				continue;
			}

			RecvCreate.source_to_connect_to = NDISources[ i ];
		}

		if( RecvCreate.source_to_connect_to.p_ndi_name )
		{
			const QString				 SourceId = QString( "%1/%2" ).arg( RecvCreate.source_to_connect_to.p_ip_address ).arg( RecvCreate.source_to_connect_to.p_ndi_name );

			if( SourceId != mNDISourceId )
			{
				if( mNDIInstance )
				{
					NDIlib_recv_destroy( mNDIInstance );

					mNDIInstance = 0;

					disconnect( mNode->context()->qobject(), SIGNAL(frameStart(qint64)), this, SLOT(contextFrameStart(qint64)) );
				}

				mNDIInstance = NDIlib_recv_create2( &RecvCreate );

				if( mNDIInstance )
				{
					connect( mNode->context()->qobject(), SIGNAL(frameStart(qint64)), this, SLOT(contextFrameStart(qint64)) );
				}
			}
		}
#endif
	}
}

void NDIReceiveNode::updateNDISources()
{
	QStringList		NDISources;

#if defined( NDI_SUPPORTED )
	if( mNDIFind )
	{
		uint32_t no_sources = 0;

//		while( !no_sources )
		{
			NDIlib_find_wait_for_sources( mNDIFind, mNDIFindTimeout );

			const NDIlib_source_t* p_sources = NDIlib_find_get_current_sources( mNDIFind, &no_sources );

			for( uint32_t i = 0 ; i < no_sources ; i++ )
			{
				NDISources << QString( p_sources[ i ].p_ndi_name );
			}
		}

		mNDIFindTimeout = 0;
	}
#endif

	mValInputSource->setChoices( NDISources );
}

void NDIReceiveNode::contextFrameStart( qint64 pTimeStamp )
{
#if defined( NDI_SUPPORTED )
	NDIlib_video_frame_t		VideoFrame;
	NDIlib_audio_frame_t		AudioFrame;
	NDIlib_metadata_frame_t		MetaDataFrame;

	NDIlib_frame_type_e			RecvType = NDIlib_recv_capture( mNDIInstance, &VideoFrame, &AudioFrame, &MetaDataFrame, 0 );

	if( RecvType == NDIlib_frame_type_none )
	{
		return;
	}

	fugio::Performance			Perf( mNode, "contextFrameStart", pTimeStamp );

	do
	{
		switch( RecvType )
		{
			case NDIlib_frame_type_none:	// just here to stop compiler warning
				break;

			case NDIlib_frame_type_video:
				processVideoFrame( VideoFrame );

				NDIlib_recv_free_video( mNDIInstance, &VideoFrame );
				break;

			case NDIlib_frame_type_audio:
				processAudioFrame( AudioFrame );

				NDIlib_recv_free_audio( mNDIInstance, &AudioFrame );
				break;

			case NDIlib_frame_type_metadata:
				NDIlib_recv_free_metadata( mNDIInstance, &MetaDataFrame );
				break;

			case NDIlib_frame_type_error:
				break;
		}
	}
	while( ( RecvType = NDIlib_recv_capture( mNDIInstance, &VideoFrame, &AudioFrame, &MetaDataFrame, 0 ) ) != NDIlib_frame_type_none );
#endif
}

void NDIReceiveNode::audio( qint64 pSamplePosition, qint64 pSampleCount, int pChannelOffset, int pChannelCount, void **pBuffers, NDIReceiveNode::AudioInstanceData *pInstanceData ) const
{
#if defined( NDI_SUPPORTED )
	QMutexLocker		Lock( &mAudioMutex );

	if( mAudioFrames.isEmpty() )
	{
		pInstanceData->mSamplePosition = 0;

		return;
	}

	if( !pInstanceData->mSamplePosition )
	{
		pInstanceData->mSamplePosition = pSamplePosition;

		pInstanceData->mAudioOffset = mAudioFrames.last().mOffset;
	}

	pSamplePosition -= pInstanceData->mSamplePosition;

	qint64			SamplesCopied = 0;

	for( const AudioFrame &AF : mAudioFrames )
	{
		qint64		FrmPos = AF.mOffset - pInstanceData->mAudioOffset;

		if( FrmPos + AF.mFrame.no_samples < pSamplePosition || FrmPos > pSamplePosition + pSampleCount )
		{
			continue;
		}

		const int		SrcIdx = std::max<int>( pSamplePosition - FrmPos, 0 );
		const int		SrcLen = AF.mFrame.no_samples - SrcIdx;
		const int		DstIdx = std::max<int>( FrmPos - pSamplePosition, 0 );
		const int		DstLen = std::min<int>( pSampleCount - DstIdx, SrcLen );

		//qDebug() << pSamplePosition << pSampleCount << SrcIdx << SrcLen << DstIdx << DstLen;

		for( int i = 0 ; i < pChannelCount ; i++ )
		{
			const int c = pChannelOffset + i;

			if( c > AF.mFrame.no_channels )
			{
				break;
			}

			const float		*SrcDat = reinterpret_cast<const float *>( reinterpret_cast<const quint8 *>( AF.mAudio ) + ( AF.mFrame.channel_stride_in_bytes * c ) ) + SrcIdx;
			float			*DstDat = reinterpret_cast<float *>( pBuffers[ i ] ) + DstIdx;

			memcpy( DstDat, SrcDat, DstLen * sizeof( float ) );
		}

		SamplesCopied += DstLen;

		if( SamplesCopied == pSampleCount )
		{
			break;
		}
	}
#endif
}

#if defined( NDI_SUPPORTED )
void NDIReceiveNode::processVideoFrame( NDIlib_video_frame_t &pFrame )
{
	fugio::Image	DstImg = mValOutputImage->variant().value<fugio::Image>();

	if( pFrame.xres != DstImg.width() || pFrame.yres != DstImg.height() )
	{
		DstImg.setSize( pFrame.xres, pFrame.yres );
	}

	if( pFrame.frame_format_type != NDIlib_frame_format_type_progressive )
	{
		return;
	}

	if( pFrame.FourCC == NDIlib_FourCC_type_BGRA )
	{
		DstImg.setFormat( fugio::ImageFormat::BGRA8 );
	}
	else if( pFrame.FourCC == NDIlib_FourCC_type_BGRX )
	{
		DstImg.setFormat( fugio::ImageFormat::BGRA8 );
	}
	else if( pFrame.FourCC == NDIlib_FourCC_type_UYVY )
	{
		DstImg.setFormat( fugio::ImageFormat::YUYV422 );
	}

	DstImg.setLineSize( 0, pFrame.line_stride_in_bytes );

	memcpy( DstImg.internalBuffer( 0 ), pFrame.p_data, DstImg.bufferSize( 0 ) );

	pinUpdated( mPinOutputImage );
}

void NDIReceiveNode::processAudioFrame( NDIlib_audio_frame_t &pFrame )
{
	AudioFrame			AF;

	if( mNDIAudioChannels != pFrame.no_channels || mNDIAudioSampleRate != pFrame.sample_rate )
	{
		clearAudio();

		mNDIAudioChannels   = pFrame.no_channels;
		mNDIAudioSampleRate = pFrame.sample_rate;
	}

	AF.mAudio = new float[ ( pFrame.channel_stride_in_bytes / sizeof( float ) ) * pFrame.no_channels ];
	AF.mFrame = pFrame;
	AF.mFrame.p_data = AF.mAudio;
	AF.mOffset = mAudioOffset;

	memcpy( AF.mAudio, pFrame.p_data, pFrame.channel_stride_in_bytes * pFrame.no_channels );

	QMutexLocker		Lock( &mAudioMutex );

	if( mAudioFrames.isEmpty() )
	{
		mAudioOffset = pFrame.timecode;
	}
	else
	{
		AudioFrame		OldFrm;

		while( mAudioFrames.last().mOffset - mAudioFrames.first().mOffset > 5 * pFrame.sample_rate )
		{
			OldFrm = mAudioFrames.takeFirst();

			delete [] OldFrm.mAudio;
		}
	}

	mAudioFrames.append( AF );

	mAudioOffset += pFrame.no_samples;
}

void NDIReceiveNode::clearAudio()
{
	QMutexLocker		Lock( &mAudioMutex );

	while( !mAudioFrames.isEmpty() )
	{
		AudioFrame		AF = mAudioFrames.takeFirst();

		if( AF.mAudio )
		{
			delete [] AF.mAudio;

			AF.mAudio = 0;
		}
	}

	mAudioOffset = 0;
}
#endif

int NDIReceiveNode::audioChannels() const
{
	return( mNDIAudioChannels );
}

qreal NDIReceiveNode::audioSampleRate() const
{
	return( mNDIAudioSampleRate );
}

fugio::AudioSampleFormat NDIReceiveNode::audioSampleFormat() const
{
	return( mNDIAudioSampleFormat );
}

qint64 NDIReceiveNode::audioLatency() const
{
	return( 0 );
}

fugio::AudioInstanceBase *NDIReceiveNode::audioAllocInstance( qreal pSampleRate, fugio::AudioSampleFormat pSampleFormat, int pChannels )
{
	QSharedPointer<fugio::AudioProducerInterface>	AI = qSharedPointerDynamicCast<fugio::AudioProducerInterface>( mNode->control() );

	if( !AI )
	{
		return( 0 );
	}

	return( new AudioInstanceData( AI, pSampleRate, pSampleFormat, pChannels ) );
}

bool NDIReceiveNode::isValid( fugio::AudioInstanceBase *pInstance ) const
{
	AudioInstanceData	*AudDat = dynamic_cast<AudioInstanceData *>( pInstance );

	if( !AudDat )
	{
		return( false );
	}

	return( true );
}

QWidget *NDIReceiveNode::gui()
{
	QPushButton		*GUI = new QPushButton( tr( "Refresh" ) );

	if( GUI )
	{
		connect( GUI, SIGNAL(released()), this, SLOT(updateNDISources()) );
	}

	return( GUI );
}
