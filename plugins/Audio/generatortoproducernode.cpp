#include "generatortoproducernode.h"

#include <fugio/audio/uuid.h>

GeneratorToProducerNode::GeneratorToProducerNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mInputProducer( nullptr ), mInputInstance( nullptr )
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

	fugio::AudioGeneratorInterface	*AGI = input<fugio::AudioGeneratorInterface *>( mPinAudioGenerator );

	if( !AGI )
	{
		return;
	}

	fugio::AudioProducerInterface	*API = input<fugio::AudioProducerInterface *>( mPinAudioGenerator );

	if( API != mInputProducer )
	{
		if( mInputProducer )
		{
			mInputProducer->freeAudioInstance( mInputInstance );

			mInputProducer = nullptr;
			mInputInstance = nullptr;
		}

		mInputProducer = API;

		if( !API )
		{
			return;
		}

		mInputInstance = mInputProducer->allocAudioInstance( AGI->audioSampleRate(), AGI->audioSampleFormat(), AGI->audioChannels() );
	}

	if( AGI->audioChannels() != mGeneratorChannelCount ||
		AGI->audioSampleFormat() != mGeneratorSampleFormat ||
		AGI->audioSampleRate() != mGeneratorSampleRate )
	{
		mGeneratorChannelCount = AGI->audioChannels();
		mGeneratorSampleFormat = AGI->audioSampleFormat();
		mGeneratorSampleRate   = AGI->audioSampleRate();

	}

	if( mGeneratorSampleFormat == fugio::AudioSampleFormat::FormatUnknown )
	{
		return;
	}
}

void GeneratorToProducerNode::audio( qint64 pSamplePosition, qint64 pSampleCount, int pChannelOffset, int pChannelCount, float **pBuffers, qint64 pLatency, void *pInstanceData ) const
{
	Q_UNUSED( pLatency )

	if( !mInputProducer || !mInputInstance )
	{
		return;
	}

	fugio::AudioGeneratorInterface	*AGI = input<fugio::AudioGeneratorInterface *>( mPinAudioGenerator );

	if( !AGI )
	{
		return;
	}

	fugio::AudioProducerInterface	*API = input<fugio::AudioProducerInterface *>( mPinAudioGenerator );

	if( !API )
	{
		return;
	}

	AudioInstanceData		*AID = static_cast<AudioInstanceData *>( pInstanceData );

	if( AID->mSampleRate == mGeneratorSampleRate &&
		AID->mSampleFormat == mGeneratorSampleFormat &&
		AID->mChannels     == mGeneratorChannelCount )
	{
		API->audio( pSamplePosition, pSampleCount, pChannelOffset, pChannelCount, pBuffers, pLatency, mInputInstance );

		return;
	}

	// At present we only convert between types and channels, not sample rate

	if(  true ) //AID->mSampleRate != mGeneratorSampleRate )
	{
		return;
	}

	QVector<const void *>		GeneratorData( AGI->audioChannels() );

	qint64			DestSamplePosition = pSamplePosition;
	qint64			DestSampleCount    = pSampleCount;

	qint64			GeneratorSamplePosition;
	qint64			GeneratorSampleCount;

	while( DestSampleCount > 0 )
	{
		if( !AGI->audioLock( DestSamplePosition, DestSampleCount, GeneratorData.data(), GeneratorSamplePosition, GeneratorSampleCount ) )
		{
			break;
		}

		switch( mGeneratorSampleFormat )
		{
			case fugio::AudioSampleFormat::Format8UI:
				{
					const quint8 *SrcBuf = static_cast<const quint8 *>( GeneratorData[ 0 ] );

					convert8UIto32FS( SrcBuf, GeneratorSampleCount, mGeneratorChannelCount, pBuffers, GeneratorSamplePosition - pSamplePosition, pChannelOffset, pChannelCount );
				}
				break;

			case fugio::AudioSampleFormat::Format32FS:
				{
					for( int i = 0 ; i < pChannelCount ; i++ )
					{
						const int	c = pChannelOffset + i;

						const float	*SrcBuf = static_cast<const float *>( GeneratorData[ c ] );
						float		*DstBuf = &static_cast<float *>( pBuffers[ c ] )[ GeneratorSamplePosition - pSamplePosition ];

						for( qint64 o = 0 ; o < GeneratorSampleCount ; o++ )
						{
							*DstBuf++ += *SrcBuf++;
						}
					}
				}
				break;

			default:
				break;
		}

		AGI->audioUnlock( GeneratorSamplePosition, GeneratorSampleCount );

		DestSamplePosition = GeneratorSamplePosition + GeneratorSampleCount;
		DestSampleCount    = ( pSamplePosition + pSampleCount ) - DestSamplePosition;
	}

	if( DestSampleCount > 0 )
	{
		qDebug() << DestSampleCount << "not found";
	}

	if( mGeneratorSampleFormat == fugio::AudioSampleFormat::Format32FS )
	{
		return;
	}

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

		AID->mGeneratorSamplePosition = -1;

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

void GeneratorToProducerNode::convert8UIto32FS( const quint8 *pSrcDat, qint64 pSrcCnt, int pSrcChn, float **pDstDat, qint64 pDstOff, int pChannelOffset, int pChannelCount )
{
	for( int i = 0 ; i < pChannelCount ; i++ )
	{
		const int c = pChannelOffset + i;

		if( pSrcChn <= c )
		{
			return;
		}

		const quint8	*SrcPtr = pSrcDat + ( pSrcChn * c );
		float			*DstPtr = pDstDat[ c ] + pDstOff;

		for( int s = 0 ; s < pSrcCnt ; s++, SrcPtr += pSrcChn )
		{
			*DstPtr++ = float( *SrcPtr - 127 ) / 128.0f;
		}
	}
}
