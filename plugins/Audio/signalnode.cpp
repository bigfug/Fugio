#include "signalnode.h"

#include <fugio/core/uuid.h>
#include <fugio/audio/uuid.h>

#include <fugio/context_interface.h>
#include <fugio/core/variant_interface.h>

#include <fugio/context_signals.h>

#include <QFile>

//#define WRITE_SIGNAL_FILENAME	"/Users/bigfug/Desktop/TEST_FILE.raw"

QMap<QString,SignalNode::SignalType>		 SignalNode::mSignalTypes;

SignalNode::SignalNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mSignalType( SINE ), mFrequency( 440.0 ), mVolume( 1.0 ), mPhaseCenter( 0.5 ), mPhaseOffset( 0.0 ), mBias( 0.0 )
{
	static const QUuid	PID_SIGNAL_TYPE  = QUuid( "{0030CB72-CCAA-43FC-86C4-5CF225C97BCC}" );
	static const QUuid	PID_PHASE_CENTER = QUuid( "{496F6FA1-9DFB-4315-8186-EAAA643647EA}" );
	static const QUuid	PID_PHASE_OFFSET = QUuid( "{6AA558E8-FB65-4B89-A1FD-C88CEBCD6B15}" );
	static const QUuid	PID_BIAS         = QUuid( "{731375BF-4B74-419E-B9BF-9001FAD002BD}" );

	if( mSignalTypes.isEmpty() )
	{
		mSignalTypes.insert( "Sine",     SignalType::SINE );
		mSignalTypes.insert( "Square",   SignalType::SQUARE );
		mSignalTypes.insert( "Triangle", SignalType::TRIANGLE );
		mSignalTypes.insert( "Saw",      SignalType::SAW );
		mSignalTypes.insert( "White",    SignalType::WHITE );
		mSignalTypes.insert( "Pink",     SignalType::PINK );
		mSignalTypes.insert( "Brown",    SignalType::BROWN );
	}

	mPinSignalTypeChoice = pinInput<fugio::ChoiceInterface *>( tr( "Signal Type" ), mPinSignalType, PID_CHOICE, PID_SIGNAL_TYPE );

	mPinSignalType->setValue( mSignalTypes.key( SignalType::SINE ) );

	mPinSignalTypeChoice->setChoices( mSignalTypes.keys() );

	mPinInputFrequency = pinInput( tr( "Frequency (Hz)" ) );

	mPinInputFrequency->setValue( mFrequency );

	mPinInputVolume = pinInput( tr( "Volume" ) );

	mPinInputVolume->setValue( mVolume );

	mPinInputPhaseCenter = pinInput( tr( "Phase Center" ), PID_PHASE_CENTER );

	mPinInputPhaseCenter->setValue( mPhaseCenter );

	mPinInputPhaseOffset = pinInput( tr( "Phase Offset" ), PID_PHASE_OFFSET );

	mPinInputPhaseOffset->setValue( mPhaseOffset );

	mPinInputBias = pinInput( tr( "Bias" ), PID_BIAS );

	mPinInputBias->setValue( mBias );

	mValOutput = pinOutput<fugio::AudioProducerInterface *>( "Audio", mPinOutput, PID_AUDIO );

	mPinInputFrequency->setDescription( tr( "The freqency to generate in hertz (Hz)" ) );

	mPinInputVolume->setDescription( tr( "The volume of the frequency (usually 0.0 - 1.0)" ) );

	mPinInputPhaseCenter->setDescription( tr( "The phase offset between positive and minus" ) );

	mPinOutput->setDescription( tr( "The generated frequency as an audio output" ) );

#if defined( WRITE_SIGNAL_FILENAME )
	if( true )
	{
		QFile		TEST_FILE( WRITE_SIGNAL_FILENAME );

		TEST_FILE.remove();
	}
#endif
}

bool SignalNode::initialise()
{
	if( !fugio::NodeControlBase::initialise() )
	{
		return( false );
	}

	connect( mNode->context()->qobject(), SIGNAL(frameStart(qint64)), this, SLOT(onContextFrame(qint64)) );

	return( true );
}

bool SignalNode::deinitialise()
{
	disconnect( mNode->context()->qobject(), SIGNAL(frameStart(qint64)), this, SLOT(onContextFrame(qint64)) );

	return( NodeControlBase::deinitialise() );
}

void SignalNode::generateSignal( qint64 pSamplePosition, qint64 pSampleCount, const float pWaveLength, float *pBuffer, float pVolume, AudioInstanceData &AID ) const
{
	const float		SmpPos = float( pSamplePosition ) + ( mPhaseOffset * pWaveLength );

	float		*DstPtr = pBuffer;

	AID.phase_delta = double( pWaveLength );// / double( AID.mSampleRate );

	switch( mSignalType )
	{
		case SINE:
			for( int i = 0 ; i < pSampleCount ; i++ )
			{
				const float		SmpPhs = fmodf( ( SmpPos + float( i ) ) / pWaveLength, 1.0f );
				const float		SmpOff = offset( SmpPhs );
				const float		SmpVal = qSin( SmpOff * 2.0 * M_PI );

				*DstPtr++ = mBias + ( SmpVal * pVolume );
			}
			break;

		case SQUARE:
			for( int i = 0 ; i < pSampleCount ; i++ )
			{
				const float		SmpPhs = fmodf( ( SmpPos + float( i ) ) / pWaveLength, 1.0f );
				const float		SmpVal = SmpPhs > mPhaseCenter ? -1.0 : 1.0;

				*DstPtr++ = mBias + ( SmpVal * pVolume );
			}
			break;

		case SAW:
			for( int i = 0 ; i < pSampleCount ; i++ )
			{
				const float		SmpPhs = fmodf( ( SmpPos + float( i ) ) / pWaveLength, 1.0f );
				const float		SmpOff = offset( SmpPhs );
				const float		SmpVal = 2.0f * ( SmpOff - qFloor( SmpOff + 0.5f ) );

				*DstPtr++ = mBias + ( SmpVal * pVolume );
			}
			break;

		case TRIANGLE:
			for( int i = 0 ; i < pSampleCount ; i++ )
			{
				const float		SmpPhs = fmodf( ( SmpPos + float( i ) ) / pWaveLength, 1.0f );
				const float		SmpOff = offset( SmpPhs );
				const float		SmpVal = 1.0f - qAbs( SmpOff - 0.5f ) * 4.0f;

				*DstPtr++ = mBias + ( SmpVal * pVolume );
			}
			break;

		case WHITE:
			for( int i = 0 ; i < pSampleCount ; i++ )
			{
				const float		SmpVal = AID.mVRand.white() * 2.0f;

				*DstPtr++ = mBias + ( SmpVal * pVolume );
			}
			break;

		case PINK:
			for( int i = 0 ; i < pSampleCount ; i++ )
			{
				const float		SmpVal = AID.mVRand.pink() * 2.0f;

				*DstPtr++ = mBias + ( SmpVal * pVolume );
			}
			break;

		case BROWN:
			for( int i = 0 ; i < pSampleCount ; i++ )
			{
				const float		SmpVal = AID.mVRand.brown() * 2.0f;

				*DstPtr++ = mBias + ( SmpVal * pVolume );
			}
			break;
	}
}

fugio::AudioInstanceBase *SignalNode::audioAllocInstance( qreal pSampleRate, fugio::AudioSampleFormat pSampleFormat, int pChannels )
{
	AudioInstanceData		*InsDat = new AudioInstanceData( qSharedPointerDynamicCast<fugio::AudioProducerInterface>( mNode->control() ), pSampleRate, pSampleFormat, pChannels );

	if( InsDat )
	{
		InsDat->mPhase = 0.0;
		InsDat->mSamplePosition = 0;
		InsDat->mVRand.seed();

		InsDat->phase_index = 0;
		InsDat->phase_delta = 0;
	}

	return( InsDat );
}

void SignalNode::audio( qint64 pSamplePosition, qint64 pSampleCount, int pChannelOffset, int pChannelCount, void **pBuffers, AudioInstanceData *pInstanceData ) const
{
	if( mFrequency <= 0.0 || !pInstanceData || mVolume <= 0 )
	{
		return;
	}

	const double		 SamplesPerPhase = ( pInstanceData->sampleRate() / mFrequency );

	if( pInstanceData->mSamplePosition <= 0 )
	{
		pInstanceData->mSamplePosition = pSamplePosition;
		pInstanceData->mPhase          = fmod( double( pSamplePosition ) / SamplesPerPhase, 1.0 );
	}

	pInstanceData->mSmpBuf.resize( pSampleCount );

	generateSignal( pInstanceData->mPhase * SamplesPerPhase, pSampleCount, SamplesPerPhase, pInstanceData->mSmpBuf.data(), mVolume, *pInstanceData );

#if defined( WRITE_SIGNAL_FILENAME )
	if( true )
	{
		QFile		TEST_FILE( WRITE_SIGNAL_FILENAME );

		if( TEST_FILE.open( QIODevice::Append ) || TEST_FILE.open( QIODevice::WriteOnly ) )
		{
			TEST_FILE.write( (const char *)pInstanceData->mSmpBuf.data(), sizeof( float ) * pSampleCount );

			TEST_FILE.close();
		}
	}
#endif

	for( int i = 0 ; i < pChannelCount ; i++ )
	{
		float		*DstPtr = (float *)pBuffers[ pChannelOffset + i ];
		float		*SrcPtr = pInstanceData->mSmpBuf.data();

		for( int j = 0 ; j < pSampleCount ; j++ )
		{
			*DstPtr++ += *SrcPtr++;
		}
	}

	pInstanceData->mPhase += double( pSampleCount ) / SamplesPerPhase;

	pInstanceData->mPhase = fmod( pInstanceData->mPhase, 1.0 );

	pInstanceData->mSamplePosition += pSampleCount;
}

void SignalNode::onContextFrame( qint64 pTimeStamp )
{
	Q_UNUSED( pTimeStamp )

//	qint64				 SamplePosition = ( pTimeStamp * 48000 ) / 1000;

//	mAudioBuffer.processAudio( SamplePosition );
}

void SignalNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	mSignalType = mSignalTypes.value( variant( mPinSignalType ).toString() );

	mFrequency = variant( mPinInputFrequency ).toDouble();

	mVolume = std::max( 0.0, variant( mPinInputVolume ).toDouble() );

	mPhaseCenter = qBound( 0.0, variant( mPinInputPhaseCenter ).toDouble(), 1.0 );

	mPhaseOffset = variant( mPinInputPhaseOffset ).toDouble();

	mBias = variant( mPinInputBias ).toDouble();
}

QWidget *SignalNode::gui()
{
	return( 0 );
}

int SignalNode::audioChannels() const
{
	return( 1 );
}

qreal SignalNode::audioSampleRate() const
{
	return( 48000 );
}

fugio::AudioSampleFormat SignalNode::audioSampleFormat() const
{
	return( fugio::AudioSampleFormat::Format32FS );
}

qint64 SignalNode::audioLatency() const
{
	return( 0 );
}
