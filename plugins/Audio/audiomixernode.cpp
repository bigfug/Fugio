#include "audiomixernode.h"
#include <fugio/node_signals.h>

#include "audiopin.h"

AudioMixerNode::AudioMixerNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	QSharedPointer<fugio::PinInterface>	P1 = pinInput( "Audio" );

	P1->setAutoRename( true );

	mValOutput = pinOutput<fugio::AudioProducerInterface *>( "Audio Mix", mPinOutput, PID_AUDIO );

	P1->setDescription( tr( "The first audio source" ) );

	mPinOutput->setDescription( tr( "The audio mix from all the input channels added together" ) );

	connect( mNode->qobject(), SIGNAL(pinLinked(QSharedPointer<fugio::PinInterface>,QSharedPointer<fugio::PinInterface>)), this, SLOT(pinLinked(QSharedPointer<fugio::PinInterface>,QSharedPointer<fugio::PinInterface>)) );

	connect( mNode->qobject(), SIGNAL(pinUnlinked(QSharedPointer<fugio::PinInterface>,QSharedPointer<fugio::PinInterface>)), this, SLOT(pinUnlinked(QSharedPointer<fugio::PinInterface>,QSharedPointer<fugio::PinInterface>)) );
}

void AudioMixerNode::audio( qint64 pSamplePosition, qint64 pSampleCount, int pChannelOffset, int pChannelCount, void **pBuffers, const AudioInstanceData *pInstanceData ) const
{
	pInstanceData->mMutex.lock();

	for( auto it = pInstanceData->mInstanceData.begin() ; it != pInstanceData->mInstanceData.end() ; it++ )
	{
		it.value()->audio( pSamplePosition, pSampleCount, pChannelOffset, pChannelCount, pBuffers );
	}

	pInstanceData->mMutex.unlock();
}

QList<QUuid> AudioMixerNode::pinAddTypesInput() const
{
	QList<QUuid>	IdList;

	IdList << PID_AUDIO;

	return( IdList );
}

bool AudioMixerNode::canAcceptPin( fugio::PinInterface *pPin ) const
{
	return( pPin->direction() == PIN_OUTPUT );
}

fugio::AudioInstanceBase *AudioMixerNode::audioAllocInstance( qreal pSampleRate, fugio::AudioSampleFormat pSampleFormat, int pChannels )
{
	AudioInstanceData		*InsDat = new AudioInstanceData( qSharedPointerDynamicCast<fugio::AudioProducerInterface>( mNode->control() ), pSampleRate, pSampleFormat, pChannels );

	if( InsDat )
	{
		for( QSharedPointer<fugio::PinInterface> DstPin : mNode->enumInputPins() )
		{
			QSharedPointer<fugio::PinInterface>			SrcPin = DstPin->connectedPin();

			if( !SrcPin )
			{
				continue;
			}

			QSharedPointer<fugio::PinControlInterface>		C = SrcPin->control();

			if( !C )
			{
				continue;
			}

			fugio::AudioProducerInterface *A = qobject_cast<fugio::AudioProducerInterface *>( C->qobject() );

			if( !A )
			{
				continue;
			}

			InsDat->mMutex.lock();

			InsDat->mInstanceData.insert( A, A->audioAllocInstance( pSampleRate, pSampleFormat, pChannels ) );

			InsDat->mMutex.unlock();
		}

		mInstanceDataMutex.lock();

		mInstanceData << InsDat;

		mInstanceDataMutex.unlock();
	}

	return( InsDat );
}

void AudioMixerNode::pinLinked( QSharedPointer<fugio::PinInterface> pPinSrc, QSharedPointer<fugio::PinInterface> pPinDst )
{
	if( pPinSrc->direction() != PIN_INPUT )
	{
		return;
	}

	fugio::AudioProducerInterface		*IAP = pPinDst->hasControl() ? qobject_cast<fugio::AudioProducerInterface *>( pPinDst->control()->qobject() ) : nullptr;

	if( IAP )
	{
		QMutexLocker	L1( &mInstanceDataMutex );

		for( AudioInstanceData *InsDat : mInstanceData )
		{
			QMutexLocker	L2( &InsDat->mMutex );

			if( !InsDat->mInstanceData.contains( IAP ) )
			{
				InsDat->mInstanceData.insert( IAP, IAP->audioAllocInstance( InsDat->sampleRate(), InsDat->sampleFormat(), InsDat->channels() ) );
			}
		}
	}
}

void AudioMixerNode::pinUnlinked( QSharedPointer<fugio::PinInterface> pPinSrc, QSharedPointer<fugio::PinInterface> pPinDst )
{
	if( pPinSrc->direction() != PIN_INPUT )
	{
		return;
	}

	fugio::AudioProducerInterface		*IAP = pPinDst->hasControl() ? qobject_cast<fugio::AudioProducerInterface *>( pPinDst->control()->qobject() ) : nullptr;

	if( IAP )
	{
		QMutexLocker	L1( &mInstanceDataMutex );

		for( AudioInstanceData *InsDat : mInstanceData )
		{
			QMutexLocker	L2( &InsDat->mMutex );

			QMap<fugio::AudioProducerInterface *,fugio::AudioInstanceBase *>::iterator	it = InsDat->mInstanceData.find( IAP );

			if( it != InsDat->mInstanceData.end() )
			{
				delete it.value();

				InsDat->mInstanceData.remove( it.key() );
			}
		}
	}
}

int AudioMixerNode::audioChannels() const
{
	QMutexLocker	Lock( &mInstanceDataMutex );

	int			Channels = 0;

	for( AudioInstanceData *InsDat : mInstanceData )
	{
		Channels = std::max( Channels, InsDat->channels() );
	}

	return( Channels );
}

qreal AudioMixerNode::audioSampleRate() const
{
	return( 48000 );
}

fugio::AudioSampleFormat AudioMixerNode::audioSampleFormat() const
{
	return( fugio::AudioSampleFormat::Format32FS );
}

qint64 AudioMixerNode::audioLatency() const
{
	qint64			Latency = 0;

	for( QSharedPointer<fugio::PinInterface> P : mNode->enumInputPins() )
	{
		fugio::AudioProducerInterface *IAP = input<fugio::AudioProducerInterface *>( P );

		if( IAP )
		{
			Latency = std::max<qint64>( Latency, IAP->audioLatency() );
		}
	}

	return( Latency );
}


bool AudioMixerNode::pinShouldAutoRename(fugio::PinInterface *pPin) const
{
	return( pPin->direction() == PIN_INPUT );
}

bool AudioMixerNode::initialise()
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

bool AudioMixerNode::deinitialise()
{
	mInstanceDataMutex.lock();

	for( AudioInstanceData *AID : mInstanceData )
	{
		AID->setEnabed( true );
	}

	mInstanceDataMutex.unlock();

	return( NodeControlBase::deinitialise() );
}
