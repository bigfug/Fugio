#include "mononode.h"

#include <fugio/core/uuid.h>
#include <fugio/audio/uuid.h>
#include <fugio/audio/audio_generator_interface.h>

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

void *MonoNode::allocAudioInstance( qreal pSampleRate, fugio::AudioSampleFormat pSampleFormat, int pChannels )
{
	fugio::AudioGeneratorInterface *IAG = input<fugio::AudioGeneratorInterface *>( mPinAudioInput );

	if( IAG && ( IAG->audioSampleFormat() != pSampleFormat || IAG->audioSampleRate() != pSampleRate ) )
	{
		return( 0 );
	}

	AudioInstanceData		*InsDat = new AudioInstanceData();

	if( InsDat )
	{
		if( IAG )
		{
			InsDat->mSampleRate   = IAG->audioSampleRate();
			InsDat->mSampleFormat = IAG->audioSampleFormat();
			InsDat->mChannels     = IAG->audioChannels();
		}
		else
		{
			InsDat->mSampleRate   = pSampleRate;
			InsDat->mSampleFormat = pSampleFormat;
			InsDat->mChannels     = pChannels;
		}

		InsDat->mAudIns = 0;

		if( fugio::AudioProducerInterface *IAP = input<fugio::AudioProducerInterface *>( mPinAudioInput ) )
		{
			InsDat->mAudIns = IAP->allocAudioInstance( InsDat->mSampleRate, InsDat->mSampleFormat, InsDat->mChannels );
		}

		mInstanceDataMutex.lock();

		mInstanceData.append( InsDat );

		mInstanceDataMutex.unlock();
	}

	return( InsDat );
}

void MonoNode::freeAudioInstance( void *pInstanceData )
{
	AudioInstanceData		*InsDat = static_cast<AudioInstanceData *>( pInstanceData );

	if( InsDat )
	{
		mInstanceDataMutex.lock();

		mInstanceData.removeAll( InsDat );

		mInstanceDataMutex.unlock();

		if( fugio::AudioProducerInterface *IAP = input<fugio::AudioProducerInterface *>( mPinAudioInput ) )
		{
			IAP->freeAudioInstance( InsDat->mAudIns );

			InsDat->mAudIns = nullptr;
		}

		delete InsDat;
	}
}

void MonoNode::audio( qint64 pSamplePosition, qint64 pSampleCount, int pChannelOffset, int pChannelCount, float **pBuffers, void *pInstanceData ) const
{
	AudioInstanceData		*InsDat = static_cast<AudioInstanceData *>( pInstanceData );

	if( !InsDat )
	{
		return;
	}

	if( fugio::AudioProducerInterface *IAP = input<fugio::AudioProducerInterface *>( mPinAudioInput ) )
	{
		QVector<QVector<float>>					 AudDat( InsDat->mChannels );
		QVector<float *>						 AudPtr( InsDat->mChannels );

		for( int i = 0 ; i < InsDat->mChannels ; i++ )
		{
			AudDat[ i ].resize( pSampleCount );
			AudPtr[ i ] = AudDat[ i ].data();
		}

		IAP->audio( pSamplePosition, pSampleCount, pChannelOffset, pChannelCount, AudPtr.data(), InsDat->mAudIns );

		for( int i = 0 ; i < InsDat->mChannels ; i++ )
		{
			const int	c = pChannelOffset + i;

			const float	*SrcDat = AudPtr[ c ];
			float		*DstDat = pBuffers[ 0 ];

			for( qint64 s = 0 ; s < pSampleCount ; s++ )
			{
				*DstDat++ += *SrcDat++;
			}
		}
	}
}
