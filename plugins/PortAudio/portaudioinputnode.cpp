#include "portaudioinputnode.h"

#include <QSettings>
#include <QDebug>

#include <fugio/audio/audio_producer_interface.h>
#include <fugio/context_interface.h>
#include <fugio/audio/uuid.h>

PortAudioInputNode::PortAudioInputNode( QSharedPointer<fugio::NodeInterface> pNode ) :
	NodeControlBase( pNode ), mPortAudio( nullptr )
{
#if defined( PORTAUDIO_SUPPORTED )
	if( ( mPortAudio = DevicePortAudio::newDevice( Pa_GetDefaultInputDevice() ) ) )
	{
		connect( mPortAudio.data(), SIGNAL(audio(const float**,quint64,int,qint64)), this, SLOT(audioInput(const float**,quint64,int,qint64)) );
	}
#endif

	mValAudio = pinOutput<fugio::AudioProducerInterface *>( "Audio", mPinAudio, PID_AUDIO );
}

PortAudioInputNode::~PortAudioInputNode()
{
	for( AudioBuffer &AB : mAudioBuffers )
	{
		for( int i = 0 ; i < AB.mChannels ; i++ )
		{
			delete [] AB.mData[ i ];
		}

		delete [] AB.mData;
	}

	if( mPortAudio )
	{
		mPortAudio.clear();
	}
}

void PortAudioInputNode::audio( qint64 pSamplePosition, qint64 pSampleCount, int pChannelOffset, int pChannelCount, float **pBuffers, qint64 pLatency, void *pInstanceData ) const
{
	Q_UNUSED( pInstanceData )
	Q_UNUSED( pLatency )

	qint64		DatRem = pSampleCount;

	for( const AudioBuffer &AB : mAudioBuffers )
	{
		qint64		PosBeg = AB.mPosition;
		qint64		PosEnd = AB.mPosition + AB.mSamples;

		if( pSamplePosition >= PosEnd || pSamplePosition + pSampleCount < PosBeg )
		{
			continue;
		}

		qint64		SrcPos = pSamplePosition - PosBeg;
		qint64		SrcLen = std::min<qint64>( SrcPos + pSampleCount, AB.mSamples - std::max<qint64>( 0, SrcPos ) );

		SrcPos = std::max<qint64>( 0, SrcPos );

		qint64		DstPos =std::max<qint64>( 0, PosBeg - pSamplePosition );

		for( int i = 0 ; i < AB.mChannels ; i++ )
		{
			if( i >= pChannelOffset && i < pChannelOffset + pChannelCount )
			{
				const float		*SrcPtr = &AB.mData[ i ][ SrcPos ];
				float			*DstPtr = &pBuffers[ i ][ DstPos ];

				for( int j = 0 ; j < SrcLen ; j++ )
				{
					DstPtr[ j ] += SrcPtr[ j ];
				}
			}
		}

		DatRem -= SrcLen;

		if( DatRem <= 0 )
		{
			break;
		}
	}
}

void PortAudioInputNode::audioInput( PortAudioInputNode::AudioBuffer &AB, const float **pData, quint64 pSampleCount, int pChannelCount, qint64 pSamplePosition )
{
	if( AB.mChannels != pChannelCount || AB.mSamples != pSampleCount )
	{
		if( AB.mData )
		{
			for( int i = 0 ; i < AB.mChannels ; i++ )
			{
				if( AB.mData[ i ] )
				{
					delete [] AB.mData[ i ];

					AB.mData[ i ] = nullptr;
				}
			}

			if( AB.mChannels != pChannelCount )
			{
				delete [] AB.mData;

				AB.mData = nullptr;
			}

			AB.mChannels = 0;
			AB.mSamples  = 0;
		}
	}

	if( !AB.mData )
	{
		if( ( AB.mData = new float *[ pChannelCount ] ) == nullptr )
		{
			return;
		}

		for( int i = 0 ; i < pChannelCount ; i++ )
		{
			AB.mData[ i ] = nullptr;
		}
	}

	for( int i = 0 ; i < pChannelCount ; i++ )
	{
		if( !AB.mData[ i ] )
		{
			AB.mData[ i ] = new float[ pSampleCount ];
		}

		if( AB.mData[ i ] )
		{
			memcpy( AB.mData[ i ], pData[ i ], sizeof( float ) * pSampleCount );
		}
	}

	AB.mChannels = pChannelCount;
	AB.mSamples  = pSampleCount;
	AB.mPosition = pSamplePosition;
}

void PortAudioInputNode::audioInput( const float **pData, quint64 pSampleCount, int pChannelCount, qint64 pSamplePosition )
{
	for( AudioBuffer &AB : mAudioBuffers )
	{
		quint64		TimeEnd = AB.mPosition + AB.mSamples;

		if( pSamplePosition - TimeEnd > 5 * mPortAudio->sampleRate() )
		{
			audioInput( AB, pData, pSampleCount, pChannelCount, pSamplePosition );

			return;
		}
	}

	AudioBuffer		AB;

	memset( &AB, 0, sizeof( AB ) );

	audioInput( AB, pData, pSampleCount, pChannelCount, pSamplePosition );

	mAudioBuffers.append( AB );
}

// not implemented yet

void *PortAudioInputNode::allocAudioInstance( qreal pSampleRate, fugio::AudioSampleFormat pSampleFormat, int pChannels )
{
	Q_UNUSED( pSampleRate )
	Q_UNUSED( pSampleFormat )
	Q_UNUSED( pChannels )

	return( nullptr );
}

void PortAudioInputNode::freeAudioInstance( void *pInstanceData )
{
	Q_UNUSED( pInstanceData )
}
