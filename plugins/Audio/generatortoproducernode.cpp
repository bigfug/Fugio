#include "generatortoproducernode.h"

#include <fugio/audio/uuid.h>

GeneratorToProducerNode::GeneratorToProducerNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_IN_GENERATOR,	"187BE548-2EDD-4C97-86A0-FA5F095B2B49" );
	FUGID( PIN_OUT_PRODUCER,	"808BBAF5-2A8A-4C78-A549-E919B2DB2D29" );

	mPinAudioGenerator = pinInput( "Generator", PIN_IN_GENERATOR );

	mAudioOutput = pinOutput<fugio::AudioProducerInterface *>( "Producer", mPinAudioOutput, PID_AUDIO, PIN_OUT_PRODUCER );

	mPinAudioGenerator->registerPinInputType( PID_AUDIO_GENERATOR );
}

void GeneratorToProducerNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );
}

void GeneratorToProducerNode::audio( qint64 pSamplePosition, qint64 pSampleCount, int pChannelOffset, int pChannelCount, float **pBuffers, qint64 pLatency, void *pInstanceData ) const
{
	Q_UNUSED( pSamplePosition )
	Q_UNUSED( pSampleCount )

	AudioInstanceData		*AID = static_cast<AudioInstanceData *>( pInstanceData );

	for( int c = pChannelOffset ; c < pChannelCount ; c++ )
	{

	}
}

void *GeneratorToProducerNode::allocAudioInstance( qreal pSampleRate, fugio::AudioSampleFormat pSampleFormat, int pChannels )
{
	AudioInstanceData		*AID = new AudioInstanceData();

	if( AID )
	{
		AID->mSampleRate   = pSampleRate;
		AID->mSampleFormat = pSampleFormat;
		AID->mChannels     = pChannels;

		mInstanceDataMutex.lock();

		mInstanceData << AID;

		mInstanceDataMutex.unlock();
	}

	return( AID );
}

void GeneratorToProducerNode::freeAudioInstance( void *pInstanceData )
{
	AudioInstanceData		*AID = static_cast<AudioInstanceData *>( pInstanceData );

	if( AID )
	{
		mInstanceDataMutex.lock();

		mInstanceData.removeAll( AID );

		mInstanceDataMutex.unlock();

		delete AID;
	}
}
