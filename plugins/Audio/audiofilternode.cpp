#include "audiofilternode.h"

#include <QImage>
#include <QPainter>

#include <fugio/core/uuid.h>
#include <fugio/audio/uuid.h>

#include <fugio/context_interface.h>
#include <fugio/context_signals.h>

#include <qmath.h>

AudioFilterNode::AudioFilterNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mLastDisplayChange( 0 ), mLastDisplayUpdate( 0 )
{
	mPinAudioInput = pinInput( "Audio" );
	mPinFreqLower  = pinInput( "Lower" );
	mPinFreqUpper  = pinInput( "Upper" );
	mPinTaps       = pinInput( "Taps" );

	mPinFreqLower->setValue( 0.0f );
	mPinFreqUpper->setValue( 48000.0f / 2.0f );
	mPinTaps->setValue( 15 );

	mAudioOutput = pinOutput<fugio::AudioProducerInterface *>( "Audio", mPinAudioOutput, PID_AUDIO );
}

bool AudioFilterNode::initialise()
{
	if( !NodeControlBase::initialise() )
	{
		return( false );
	}

	connect( mNode->context()->qobject(), SIGNAL(frameStart(qint64)), this, SLOT(onContextFrame(qint64)) );

	inputsUpdated( 0 );

	return( true );
}

bool AudioFilterNode::deinitialise()
{
	disconnect( mNode->context()->qobject(), SIGNAL(frameStart(qint64)), this, SLOT(onContextFrame(qint64)) );

	return( true );
}

void AudioFilterNode::inputsUpdated( qint64 pTimeStamp )
{
	float		SampleRate = 48000.0f;

//	if( fugio::AudioProducerInterface *IAP = input<fugio::AudioProducerInterface *>( mPinAudioInput ) )
//	{
//		SampleRate = IAP->sampleRate();
//	}

	float		FreqLower  = variant( mPinFreqLower ).toFloat();
	float		FreqUpper  = variant( mPinFreqUpper ).toFloat();
	int			TapCnt     = variant( mPinTaps ).toInt();

	FreqLower = qBound( 0.0f, FreqLower, SampleRate / 2.0f );
	FreqUpper = qBound( 0.0f, FreqUpper, SampleRate / 2.0f );

	FreqLower = qMin( FreqLower, FreqUpper );

	TapCnt = qBound( 1, TapCnt, 1000 );

	if( true )
	{
		QMutexLocker		Lock( &mInstanceDataMutex );

		mSampleRate     = SampleRate;
		mLowerFrequency = FreqLower;
		mUpperFrequency = FreqUpper;
		mTaps.resize( TapCnt );

		updateTaps();
	}

	mLastDisplayChange = pTimeStamp;
}

fugio::AudioInstanceBase *AudioFilterNode::audioAllocInstance( qreal pSampleRate, fugio::AudioSampleFormat pSampleFormat, int pChannels )
{
	AudioInstanceData		*InsDat = new AudioInstanceData( qSharedPointerDynamicCast<fugio::AudioProducerInterface>( mNode->control() ), pSampleRate, pSampleFormat, pChannels );

	if( InsDat )
	{
		InsDat->mAudIns = 0;
		InsDat->mAudPts = 0;
		InsDat->mAudSmp = 0;

		fugio::AudioProducerInterface		*IAP = input<fugio::AudioProducerInterface *>( mPinAudioInput );

		if( IAP )
		{
			InsDat->mAudIns = IAP->audioAllocInstance( pSampleRate, pSampleFormat, pChannels );
		}

		mInstanceDataMutex.lock();

		mInstanceData.append( InsDat );

		mInstanceDataMutex.unlock();
	}

	return( InsDat );
}

void AudioFilterNode::onContextFrame( qint64 pTimeStamp )
{
	if( mLastDisplayChange > mLastDisplayUpdate && pTimeStamp - mLastDisplayUpdate >= 100 )
	{
		emit updateDisplay( mTaps );

		mLastDisplayUpdate = mLastDisplayChange;
	}
}

void AudioFilterNode::updateTaps( void )
{
	const int		NumTap = mTaps.size();
	float			mm;

	if( mLowerFrequency <= 0.0f && mUpperFrequency >= mSampleRate / 2.0f )
	{
		mTaps[ 0 ] = 1;

		for( int i = 1 ; i < NumTap ; i++ )
		{
			mTaps[ i ] = 0;
		}
	}
	else if( mLowerFrequency >= mUpperFrequency )
	{
		for( int i = 0 ; i < NumTap ; i++ )
		{
			mTaps[ i ] = 0;
		}
	}
	else if( mLowerFrequency <= 0.0f )
	{
		const float		Lambda = M_PI * mUpperFrequency / ( mSampleRate / 2.0f );

		for( int i = 0 ; i < NumTap ; i++ )
		{
			mm = i - ( NumTap - 1.0f ) / 2.0f;

			if( !mm )
			{
				mTaps[ i ] = Lambda / M_PI;
			}
			else
			{
				mTaps[ i ] = qSin( mm * Lambda ) / ( mm * M_PI );
			}
		}
	}
	else if( mUpperFrequency >= mSampleRate / 2 )
	{
		const float		Lambda = M_PI * mLowerFrequency / ( mSampleRate / 2.0f );

		for( int i = 0 ; i < NumTap ; i++ )
		{
			mm = i - ( NumTap - 1.0f ) / 2.0f;

			if( !mm )
			{
				mTaps[ i ] = 1.0f - ( Lambda / M_PI );
			}
			else
			{
				mTaps[ i ] = -qSin( mm * Lambda ) / ( mm * M_PI );
			}
		}
	}
	else
	{
		const float		Lambda = M_PI * mLowerFrequency / ( mSampleRate / 2.0f );
		const float		Phi    = M_PI * mUpperFrequency / ( mSampleRate / 2.0f );

		for( int i = 0 ; i < NumTap ; i++ )
		{
			mm = i - ( NumTap - 1.0f ) / 2.0f;

			if( !mm )
			{
				mTaps[ i ] = ( Phi - Lambda ) / M_PI;
			}
			else
			{
				mTaps[ i ] = ( qSin( mm * Phi ) - qSin( mm * Lambda ) ) / ( mm * M_PI );
			}
		}
	}
}

void AudioFilterNode::audio( qint64 pSamplePosition, qint64 pSampleCount, int pChannelOffset, int pChannelCount, void **pBuffers, AudioInstanceData *pInstanceData ) const
{
	fugio::AudioProducerInterface	*IAP = input<fugio::AudioProducerInterface *>( mPinAudioInput );

	if( !IAP )
	{
		return;
	}

	if( pInstanceData->mAudDat.size() != pChannelOffset + pChannelCount )
	{
		pInstanceData->mAudDat.resize( pChannelOffset + pChannelCount );
	}

	for( int i = 0 ; i < pInstanceData->mAudDat.size() ; i++ )
	{
		pInstanceData->mAudDat[ i ].resize( mTaps.size() );
	}

	if( mLowerFrequency >= mUpperFrequency )
	{
		return;
	}

	pInstanceData->mAudIns->audio( pSamplePosition, pSampleCount, pChannelOffset, pChannelCount, pBuffers );

	if( mLowerFrequency <= 0.0f && mUpperFrequency >= mSampleRate / 2.0f )
	{
		return;
	}

	const int		 TapCnt = mTaps.size();
	const float		*TapDat = &mTaps[ 0 ];

	for( int c = pChannelOffset ; c < pChannelCount ; c++ )
	{
		float		*AudDat = &pInstanceData->mAudDat[ c ][ 0 ];

		for( qint64 i = 0 ; i < pSampleCount ; i++ )
		{
			for( int j = TapCnt - 1 ; j > 0 ; j-- )
			{
				AudDat[ j ] = AudDat[ j - 1 ];
			}

			AudDat[ 0 ] = reinterpret_cast<float **>( pBuffers )[ c ][ i ];

			float		AudFlt = 0;

			for( int k = 0 ; k < TapCnt ; k++ )
			{
				AudFlt += AudDat[ k ] * TapDat[ k ];
			}

			((float *)pBuffers[ c ])[ i ] = AudFlt;
		}
	}
}

QWidget *AudioFilterNode::gui()
{
	AudioFilterDisplay		*GUI = new AudioFilterDisplay();

	if( GUI )
	{
		connect( this, SIGNAL(updateDisplay(QVector<float>)), GUI, SLOT(updateDisplay(QVector<float>)) );

		if( !mTaps.isEmpty() )
		{
			GUI->updateDisplay( mTaps );
		}
	}

	return( GUI );
}


int AudioFilterNode::audioChannels() const
{
	fugio::AudioProducerInterface		*IAP = input<fugio::AudioProducerInterface *>( mPinAudioInput );

	return( IAP ? IAP->audioChannels() : 0 );
}

qreal AudioFilterNode::audioSampleRate() const
{
	fugio::AudioProducerInterface		*IAP = input<fugio::AudioProducerInterface *>( mPinAudioInput );

	return( IAP ? IAP->audioSampleRate() : 0 );
}

fugio::AudioSampleFormat AudioFilterNode::audioSampleFormat() const
{
	return( fugio::AudioSampleFormat::Format32FS );
}

qint64 AudioFilterNode::audioLatency() const
{
	fugio::AudioProducerInterface		*IAP = input<fugio::AudioProducerInterface *>( mPinAudioInput );

	return( IAP ? IAP->audioLatency() : 0 );
}
