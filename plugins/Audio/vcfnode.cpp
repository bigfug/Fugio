#include "vcfnode.h"

#include <fugio/core/uuid.h>
#include <fugio/audio/uuid.h>

#include <fugio/context_interface.h>
#include <fugio/context_signals.h>
#include <fugio/pin_signals.h>

#include <qmath.h>
#include <cmath>

VCFNode::VCFNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mCutoff( 1 ), mResonance( 0 )
{
	mPinAudioInput = pinInput( "Audio" );
	mPinCutoff     = pinInput( "Cutoff" );
	mPinResonance  = pinInput( "Resonance" );

	mAudioOutput = pinOutput<fugio::AudioProducerInterface *>( "Audio", mPinAudioOutput, PID_AUDIO );
}

bool VCFNode::initialise()
{
	if( !NodeControlBase::initialise() )
	{
		return( false );
	}

	connect( mPinCutoff->qobject(), SIGNAL(linked(QSharedPointer<fugio::PinInterface>)), this, SLOT(cutoffLinked(QSharedPointer<fugio::PinInterface>)) );
	connect( mPinCutoff->qobject(), SIGNAL(unlinked(QSharedPointer<fugio::PinInterface>)), this, SLOT(cutoffUnlinked(QSharedPointer<fugio::PinInterface>)) );

	connect( mPinResonance->qobject(), SIGNAL(linked(QSharedPointer<fugio::PinInterface>)), this, SLOT(resonanceLinked(QSharedPointer<fugio::PinInterface>)) );
	connect( mPinResonance->qobject(), SIGNAL(unlinked(QSharedPointer<fugio::PinInterface>)), this, SLOT(resonanceUnlinked(QSharedPointer<fugio::PinInterface>)) );

	inputsUpdated( 0 );

	return( true );
}

bool VCFNode::deinitialise()
{
	disconnect( mPinCutoff->qobject(), SIGNAL(linked(QSharedPointer<fugio::PinInterface>)), this, SLOT(cutoffLinked(QSharedPointer<fugio::PinInterface>)) );
	disconnect( mPinCutoff->qobject(), SIGNAL(unlinked(QSharedPointer<fugio::PinInterface>)), this, SLOT(cutoffUnlinked(QSharedPointer<fugio::PinInterface>)) );

	disconnect( mPinResonance->qobject(), SIGNAL(linked(QSharedPointer<fugio::PinInterface>)), this, SLOT(resonanceLinked(QSharedPointer<fugio::PinInterface>)) );
	disconnect( mPinResonance->qobject(), SIGNAL(unlinked(QSharedPointer<fugio::PinInterface>)), this, SLOT(resonanceUnlinked(QSharedPointer<fugio::PinInterface>)) );

	return( NodeControlBase::deinitialise() );
}

void VCFNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	mCutoff = variant( mPinCutoff ).toFloat();
	mResonance = variant( mPinResonance ).toFloat();
}

fugio::AudioInstanceBase *VCFNode::audioAllocInstance( qreal pSampleRate, fugio::AudioSampleFormat pSampleFormat, int pChannels )
{
	AudioInstanceData		*InsDat = new AudioInstanceData( qSharedPointerDynamicCast<fugio::AudioProducerInterface>( mNode->control() ), pSampleRate, pSampleFormat, pChannels );

	if( InsDat )
	{
		InsDat->mAudIns = 0;
		InsDat->mAudPts = 0;
		InsDat->mAudSmp = 0;

		InsDat->mCutoffInstance = 0;

		InsDat->mResonanceInstance = 0;

		InsDat->mAudDat.resize( pChannels );

		AudioChannelData		ACD;

		memset( &ACD, 0, sizeof( ACD ) );

		for( int i = 0 ; i < pChannels ; i++ )
		{
			InsDat->mChnDat << ACD;
		}

		if( fugio::AudioProducerInterface *IAP = input<fugio::AudioProducerInterface *>( mPinAudioInput ) )
		{
			InsDat->mAudIns = IAP->audioAllocInstance( pSampleRate, pSampleFormat, pChannels );
		}

		if( fugio::AudioProducerInterface *IAP = input<fugio::AudioProducerInterface *>( mPinCutoff ) )
		{
			InsDat->mCutoffInstance = IAP->audioAllocInstance( pSampleRate, pSampleFormat, 1 );
		}

		if( fugio::AudioProducerInterface *IAP = input<fugio::AudioProducerInterface *>( mPinResonance ) )
		{
			InsDat->mResonanceInstance = IAP->audioAllocInstance( pSampleRate, pSampleFormat, 1 );
		}

		mInstanceDataMutex.lock();

		mInstanceData.append( InsDat );

		mInstanceDataMutex.unlock();
	}

	return( InsDat );
}

void VCFNode::cutoffLinked( QSharedPointer<fugio::PinInterface> pPin )
{
	if( !pPin->hasControl() )
	{
		return;
	}

	fugio::AudioProducerInterface *IAP = qobject_cast<fugio::AudioProducerInterface *>( pPin->control()->qobject() );

	if( !IAP )
	{
		return;
	}

	QMutexLocker	MutLck( &mInstanceDataMutex );

	for( AudioInstanceData *AID : mInstanceData )
	{
		AID->mCutoffInstance = IAP->audioAllocInstance( AID->sampleRate(), AID->sampleFormat(), 1 );
	}
}

void VCFNode::cutoffUnlinked(QSharedPointer<fugio::PinInterface> pPin)
{
	if( !pPin->hasControl() )
	{
		return;
	}

	fugio::AudioProducerInterface *IAP = qobject_cast<fugio::AudioProducerInterface *>( pPin->control()->qobject() );

	if( !IAP )
	{
		return;
	}

	QMutexLocker	MutLck( &mInstanceDataMutex );

	for( AudioInstanceData *AID : mInstanceData )
	{
		if( AID->mCutoffInstance )
		{
			delete AID->mCutoffInstance;

			AID->mCutoffInstance = nullptr;
		}
	}
}

void VCFNode::resonanceLinked( QSharedPointer<fugio::PinInterface> pPin )
{
	if( !pPin->hasControl() )
	{
		return;
	}

	fugio::AudioProducerInterface *IAP = qobject_cast<fugio::AudioProducerInterface *>( pPin->control()->qobject() );

	if( !IAP )
	{
		return;
	}

	QMutexLocker	MutLck( &mInstanceDataMutex );

	for( AudioInstanceData *AID : mInstanceData )
	{
		AID->mResonanceInstance = IAP->audioAllocInstance( AID->sampleRate(), AID->sampleFormat(), 1 );
	}
}

void VCFNode::resonanceUnlinked(QSharedPointer<fugio::PinInterface> pPin)
{
	if( !pPin->hasControl() )
	{
		return;
	}

	fugio::AudioProducerInterface *IAP = qobject_cast<fugio::AudioProducerInterface *>( pPin->control()->qobject() );

	if( !IAP )
	{
		return;
	}

	QMutexLocker	MutLck( &mInstanceDataMutex );

	for( AudioInstanceData *AID : mInstanceData )
	{
		if( AID->mResonanceInstance )
		{
			delete AID->mResonanceInstance;

			AID->mResonanceInstance = nullptr;
		}
	}
}

void VCFNode::updateFilter(const float cutoff, const float res, float &p, float &k, float &r )
{
	p = cutoff * ( 1.8f - 0.8f * cutoff );
	k = 2.0f * qSin( cutoff * M_PI * 0.5f ) - 1.0f;

	float		t1 = ( 1.0f - p ) * 1.386249f;
	float		t2 = 12.0f + t1 * t1;

	r  = res * ( t2 + 6.0f * t1 ) / ( t2 - 6.0f * t1 );
}

void VCFNode::audio( qint64 pSamplePosition, qint64 pSampleCount, int pChannelOffset, int pChannelCount, void **pBuffers, AudioInstanceData *pInstanceData ) const
{
	if( pInstanceData->mAudIns )
	{
		pInstanceData->mAudIns->audio( pSamplePosition, pSampleCount, pChannelOffset, pChannelCount, pBuffers );
	}
	else
	{
		return;
	}

	if( pInstanceData->mCutoffInstance )
	{
		pInstanceData->mCutoffData.resize( pSampleCount );

		float		*SmpPtr = pInstanceData->mCutoffData.data();

		memset( SmpPtr, 0, sizeof( float ) * pSampleCount );

		pInstanceData->mCutoffInstance->audio( pSamplePosition, pSampleCount, 0, 1, (void **)&SmpPtr  );
	}
	else
	{
		pInstanceData->mCutoffData.clear();
	}

	if( pInstanceData->mResonanceInstance )
	{
		pInstanceData->mResonanceData.resize( pSampleCount );

		float		*SmpPtr = pInstanceData->mResonanceData.data();

		memset( SmpPtr, 0, sizeof( float ) * pSampleCount );

		pInstanceData->mResonanceInstance->audio( pSamplePosition, pSampleCount, 0, 1, (void **)&SmpPtr );
	}
	else
	{
		pInstanceData->mResonanceData.clear();
	}

	// http://musicdsp.org/showArchiveComment.php?ArchiveID=24

	float		cutoff = qBound( 0.0f, mCutoff, 1.0f );
	float		res    = qBound( 0.0f, mResonance, 1.0f );

	float		p, k, r;

	updateFilter( cutoff, res, p, k, r );

	for( int c = pChannelOffset ; c < pChannelCount ; c++ )
	{
		AudioChannelData	&ACD = pInstanceData->mChnDat[ c ];

		float				*AudDst = reinterpret_cast<float **>( pBuffers )[ c ];

		for( qint64 i = 0 ; i < pSampleCount ; i++ )
		{
			if( !pInstanceData->mCutoffData.isEmpty() || !pInstanceData->mResonanceData.isEmpty() )
			{
				if( !pInstanceData->mCutoffData.isEmpty() )
				{
					cutoff = qBound( 0.0f, pInstanceData->mCutoffData[ i ], 1.0f );
				}

				if( !pInstanceData->mResonanceData.isEmpty() )
				{
					res = qBound( 0.0f, pInstanceData->mResonanceData[ i ], 1.0f );
				}

				updateFilter( cutoff, res, p, k, r );
			}

			float			x = qBound( -1.0f, AudDst[ i ], 1.0f ) - r * ACD.y4;

#if defined( QT_DEBUG )
            if( std::isinf( x ) || std::isnan( x ) )
			{
				x = qBound( -1.0f, AudDst[ i ], 1.0f );
			}
#endif

			//Four cascaded onepole filters (bilinear transform)

			ACD.y1=x*p + ACD.oldx*p - k*ACD.y1;
			ACD.y2=ACD.y1*p+ACD.oldy1*p - k*ACD.y2;
			ACD.y3=ACD.y2*p+ACD.oldy2*p - k*ACD.y3;
			ACD.y4=ACD.y3*p+ACD.oldy3*p - k*ACD.y4;

			//Clipper band limited sigmoid

			ACD.y4 -= ( ACD.y4 * ACD.y4 * ACD.y4 ) / 6.0f;

            if( std::isnan( ACD.y4 ) )
			{
				memset( &ACD, 0, sizeof( ACD ) );

				x = 0;
			}

			ACD.oldx = x;
			ACD.oldy1 = ACD.y1;
			ACD.oldy2 = ACD.y2;
			ACD.oldy3 = ACD.y3;

			AudDst[ i ] = ACD.y4;
		}
	}
}

int VCFNode::audioChannels() const
{
	return( 1 );
}

qreal VCFNode::audioSampleRate() const
{
	return( 48000 );
}

fugio::AudioSampleFormat VCFNode::audioSampleFormat() const
{
	return( fugio::AudioSampleFormat::Format32FS );
}

qint64 VCFNode::audioLatency() const
{
	fugio::AudioProducerInterface *IAP = input<fugio::AudioProducerInterface *>( mPinAudioInput );

	return( IAP ? IAP->audioLatency() : 0 );
}
