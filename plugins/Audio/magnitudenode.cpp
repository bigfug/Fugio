#include "magnitudenode.h"

#include <fugio/core/uuid.h>
#include <fugio/audio/uuid.h>

#include <fugio/context_interface.h>
#include <fugio/core/variant_interface.h>
#include <fugio/audio/fft_interface.h>
#include <fugio/audio/audio_producer_interface.h>

#include <fugio/context_signals.h>

#include <qmath.h>

#define MAG_SMP	(48000/50)

MagnitudeNode::MagnitudeNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mMagnitude( 0 ), mSamplePosition( 0 ), mProducerInstance( nullptr )
{
	mPinAudio = pinInput( "Audio" );

	mPinSampleCount = pinInput( "Samples" );

	mPinSampleCount->setValue( MAG_SMP );

	mValOutput = pinOutput<fugio::VariantInterface *>( "Magnitude", mPinOutput, PID_FLOAT );

	mPinAudio->setDescription( tr( "The source audio signal" ) );

	mPinSampleCount->setDescription( tr( "The number of samples to count in each calculation" ) );

	mPinOutput->setDescription( tr( "The largest magnitude of the audio signal" ) );
}

bool MagnitudeNode::initialise()
{
	if( !fugio::NodeControlBase::initialise() )
	{
		return( false );
	}

	connect( mNode->context()->qobject(), SIGNAL(frameProcess(qint64)), this, SLOT(onContextProcess(qint64)) );

	return( true );
}

bool MagnitudeNode::deinitialise()
{
	disconnect( mNode->context()->qobject(), SIGNAL(frameProcess(qint64)), this, SLOT(onContextProcess(qint64)) );

	return( NodeControlBase::deinitialise() );
}

void MagnitudeNode::onContextProcess( qint64 pTimeStamp )
{
	if( mProducerInstance && !mProducerInstance->isValid() )
	{
		delete mProducerInstance;

		mProducerInstance = nullptr;
	}

	fugio::AudioProducerInterface	*API = input<fugio::AudioProducerInterface *>( mPinAudio );

	if( !API )
	{
		if( mProducerInstance )
		{
			delete mProducerInstance;

			mProducerInstance = nullptr;

			mSamplePosition = 0;
		}

		return;
	}

	if( !mProducerInstance )
	{
		mProducerInstance = API->audioAllocInstance( 48000, fugio::AudioSampleFormat::Format32FS, 1 );
	}

	if( !mProducerInstance )
	{
		return;
	}

	if( !mSamplePosition )
	{
		mSamplePosition = pTimeStamp * ( 48000 / 1000 );
	}

	qint64	CurPos = pTimeStamp * ( 48000 / 1000 );

	if( CurPos - mSamplePosition >= MAG_SMP )
	{
		int			ChannelCount = 1;

		if( ChannelCount != mAudDat.size() )
		{
			mAudDat.resize( ChannelCount );

			for( auto &V : mAudDat )
			{
				V.resize( 48000 );
			}
		}

		QVector<float *>	AudPtr;

		AudPtr.resize( ChannelCount );

		for( int i = 0 ; i < ChannelCount ; i++ )
		{
			AudPtr[ i ] = mAudDat[ i ].data();

			memset( AudPtr[ i ], 0, sizeof( float ) * MAG_SMP );
		}

		mProducerInstance->audio( mSamplePosition, MAG_SMP, 0, 1, (void **)AudPtr.data() );

		float		Mag = 0;

		for( int c = 0 ; c < ChannelCount ; c++ )
		{
			float	*S = AudPtr[ c ];

			for( int i = 0 ; i < MAG_SMP ; i++, S++ )
			{
				float	M = fabs( *S );

				if( M > Mag )
				{
					Mag = M;
				}
			}
		}

		mMagnitude = Mag;

		mSamplePosition += MAG_SMP;
	}

	if( mMagnitude != mValOutput->variant().toFloat() )
	{
		mValOutput->setVariant( mMagnitude );

		pinUpdated( mPinOutput );
	}
}
