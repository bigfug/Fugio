#include "mononode.h"

#include <fugio/core/uuid.h>
#include <fugio/audio/uuid.h>

#include <fugio/context_interface.h>
#include <fugio/context_signals.h>
#include <fugio/pin_signals.h>

#include <qmath.h>
#include <cmath>

MonoNode::MonoNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_AUDIO,	"7F95A010-BD9F-49DA-9C5F-16A0576DE9F0" );
	FUGID( PIN_OUTPUT_AUDIO, "0B270425-0CE9-4015-870B-08415A1E3595" );

	mPinAudioInput = pinInput( "Audio", PIN_INPUT_AUDIO );

	mAudioOutput = pinOutput<fugio::AudioProducerInterface *>( "Audio", mPinAudioOutput, PID_AUDIO, PIN_OUTPUT_AUDIO );
}

bool MonoNode::initialise()
{
	if( !NodeControlBase::initialise() )
	{
		return( false );
	}

	return( true );
}

bool MonoNode::deinitialise()
{
	return( NodeControlBase::deinitialise() );
}

void MonoNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );
}

void *MonoNode::audioAllocInstance( qreal pSampleRate, fugio::AudioSampleFormat pSampleFormat, int pChannels )
{
	if( pChannels != 1 )
	{
		return( 0 );
	}

	fugio::AudioProducerInterface *IAP = input<fugio::AudioProducerInterface *>( mPinAudioInput );

	if( IAP && ( IAP->audioSampleFormat() != pSampleFormat || IAP->audioSampleRate() != pSampleRate ) )
	{
		return( 0 );
	}

	AudioInstanceData		*InsDat = new AudioInstanceData();

	if( InsDat )
	{
		InsDat->mSampleRate   = IAP->audioSampleRate();
		InsDat->mSampleFormat = IAP->audioSampleFormat();
		InsDat->mChannels     = IAP->audioChannels();
		InsDat->mAudIns = 0;

		if( IAP )
		{
			InsDat->mAudIns = IAP->audioAllocInstance( InsDat->mSampleRate, InsDat->mSampleFormat, InsDat->mChannels );
		}

		mInstanceDataMutex.lock();

		mInstanceData.append( InsDat );

		mInstanceDataMutex.unlock();
	}

	return( InsDat );
}

void MonoNode::audioFreeInstance( void *pInstanceData )
{
	AudioInstanceData		*InsDat = static_cast<AudioInstanceData *>( pInstanceData );

	if( InsDat )
	{
		mInstanceDataMutex.lock();

		mInstanceData.removeAll( InsDat );

		mInstanceDataMutex.unlock();

		if( fugio::AudioProducerInterface *IAP = input<fugio::AudioProducerInterface *>( mPinAudioInput ) )
		{
			IAP->audioFreeInstance( InsDat->mAudIns );

			InsDat->mAudIns = nullptr;
		}

		delete InsDat;
	}
}

void MonoNode::audio( qint64 pSamplePosition, qint64 pSampleCount, int pChannelOffset, int pChannelCount, void **pBuffers, void *pInstanceData ) const
{
	AudioInstanceData		*InsDat = static_cast<AudioInstanceData *>( pInstanceData );

	if( !InsDat )
	{
		return;
	}

	if( fugio::AudioProducerInterface *IAP = input<fugio::AudioProducerInterface *>( mPinAudioInput ) )
	{
		if( InsDat->mChannels == 1 )
		{
			IAP->audio( pSamplePosition, pSampleCount, pChannelOffset, pChannelCount, pBuffers, InsDat->mAudIns );
		}
		else
		{
			QVector<QVector<float>>					 AudDat( InsDat->mChannels );
			QVector<float *>						 AudPtr( InsDat->mChannels );

			for( int i = 0 ; i < InsDat->mChannels ; i++ )
			{
				AudDat[ i ].resize( pSampleCount );
				AudPtr[ i ] = AudDat[ i ].data();
			}

			IAP->audio( pSamplePosition, pSampleCount, pChannelOffset, pChannelCount, (void **)AudPtr.data(), InsDat->mAudIns );

			for( int i = 0 ; i < InsDat->mChannels ; i++ )
			{
				const int	c = pChannelOffset + i;

				const float	*SrcDat = AudPtr[ c ];
				float		*DstDat = (float *)pBuffers[ 0 ];

				for( qint64 s = 0 ; s < pSampleCount ; s++ )
				{
					*DstDat++ += *SrcDat++;
				}
			}
		}
	}
}

int MonoNode::audioChannels() const
{
	return( 1 );
}

qreal MonoNode::audioSampleRate() const
{
	return( 48000 );
}

fugio::AudioSampleFormat MonoNode::audioSampleFormat() const
{
	return( fugio::AudioSampleFormat::Format32FS );
}

qint64 MonoNode::audioLatency() const
{
	fugio::AudioProducerInterface *IAP = input<fugio::AudioProducerInterface *>( mPinAudioInput );

	return( IAP ? IAP->audioLatency() : 0 );
}
