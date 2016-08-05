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

	mInstanceDataMutex.lock();

	for( AudioInstanceData *AID : mInstanceData )
	{
		AID->setEnabed( true );
	}

	mInstanceDataMutex.unlock();

	return( true );
}

bool MonoNode::deinitialise()
{
	mInstanceDataMutex.lock();

	for( AudioInstanceData *AID : mInstanceData )
	{
		AID->setEnabed( false );
	}

	mInstanceDataMutex.unlock();

	return( NodeControlBase::deinitialise() );
}

void MonoNode::inputsUpdated( qint64 pTimeStamp )
{
	if( mPinAudioInput->isUpdated( pTimeStamp ) )
	{
		pinUpdated( mPinAudioOutput );
	}
}

fugio::AudioInstanceBase *MonoNode::audioAllocInstance( qreal pSampleRate, fugio::AudioSampleFormat pSampleFormat, int pChannels )
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

	AudioInstanceData		*InsDat = new AudioInstanceData( qSharedPointerDynamicCast<fugio::AudioProducerInterface>( mNode->control() ), pSampleRate, pSampleFormat, pChannels );

	if( InsDat )
	{
		InsDat->mAudIns = 0;

		if( IAP )
		{
			InsDat->mAudIns = IAP->audioAllocInstance( IAP->audioSampleRate(), IAP->audioSampleFormat(), IAP->audioChannels() );
		}

		mInstanceDataMutex.lock();

		mInstanceData.append( InsDat );

		mInstanceDataMutex.unlock();
	}

	return( InsDat );
}

void MonoNode::audio( qint64 pSamplePosition, qint64 pSampleCount, int pChannelOffset, int pChannelCount, void **pBuffers, AudioInstanceData *pInstanceData ) const
{
	if( !pInstanceData || !pInstanceData->mAudIns )
	{
		return;
	}

	const int SrcChnCnt = pInstanceData->mAudIns->channels();

	if( SrcChnCnt == 1 )
	{
		pInstanceData->mAudIns->audio( pSamplePosition, pSampleCount, pChannelOffset, pChannelCount, pBuffers );
	}
	else
	{
		QVector<QVector<float>>					 AudDat( SrcChnCnt );
		QVector<float *>						 AudPtr( SrcChnCnt );

		for( int i = 0 ; i < SrcChnCnt ; i++ )
		{
			AudDat[ i ].resize( pSampleCount );
			AudPtr[ i ] = AudDat[ i ].data();
		}

		pInstanceData->mAudIns->audio( pSamplePosition, pSampleCount, 0, SrcChnCnt, (void **)AudPtr.data() );

		for( int c = 0 ; c < SrcChnCnt ; c++ )
		{
			const float	*SrcDat = AudPtr[ c ];
			float		*DstDat = (float *)pBuffers[ 0 ];

			for( qint64 s = 0 ; s < pSampleCount ; s++ )
			{
				*DstDat++ += *SrcDat++;
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
