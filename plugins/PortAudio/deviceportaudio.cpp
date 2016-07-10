#include "deviceportaudio.h"

#include <QDebug>
#include <QDateTime>

#include <fugio/context_interface.h>
#include <fugio/audio/audio_producer_interface.h>

#include "portaudiooutputnode.h"
#include "portaudioplugin.h"

#define AUDIO_DEFAULT_SAMPLE_RATE	(48000)

QList<QWeakPointer<DevicePortAudio>>	 DevicePortAudio::mDeviceList;

void DevicePortAudio::deviceInitialise()
{
//	PaHostApiIndex	HostApiCount   = Pa_GetHostApiCount();
//	PaHostApiIndex	DefaultHostApi = Pa_GetDefaultHostApi();

#if defined( PORTAUDIO_SUPPORTED )
	qDebug() << "DefInp:" << Pa_GetDefaultInputDevice() << "DefOut:" << Pa_GetDefaultOutputDevice();

	PaDeviceIndex	DevCnt = Pa_GetDeviceCount();

	for( PaDeviceIndex DevIdx = 0 ; DevIdx < DevCnt ; DevIdx++ )
	{
		const PaDeviceInfo	*DevInf = Pa_GetDeviceInfo( DevIdx );

		const PaHostApiInfo *HstInf = Pa_GetHostApiInfo( DevInf->hostApi );

		qDebug() << HstInf->name << DevInf->name << DevInf->maxInputChannels << DevInf->maxOutputChannels;
	}
#endif
}

void DevicePortAudio::deviceDeinitialise()
{

}

void DevicePortAudio::devicePacketStart( qint64 pTimeStamp )
{
	Q_UNUSED( pTimeStamp )
}

void DevicePortAudio::devicePacketEnd()
{
}

void DevicePortAudio::deviceCfgSave( QSettings &pDataStream )
{
	Q_UNUSED( pDataStream )
}

void DevicePortAudio::deviceCfgLoad( QSettings &pDataStream )
{
	Q_UNUSED( pDataStream )
}

#if defined( PORTAUDIO_SUPPORTED )
QSharedPointer<DevicePortAudio> DevicePortAudio::newDevice( PaDeviceIndex pDevIdx )
{
	for( QSharedPointer<DevicePortAudio> DevAud : mDeviceList )
	{
		if( DevAud && DevAud->mDeviceIndex == pDevIdx )
		{
			return( DevAud );
		}
	}

	QSharedPointer<DevicePortAudio> NewDev = QSharedPointer<DevicePortAudio>( new DevicePortAudio( pDevIdx ) );

	if( NewDev )
	{
		mDeviceList.append( NewDev );

		//gApp->devices().registerDevice( NewDev );
	}

	return( NewDev );
}

QStringList DevicePortAudio::deviceOutputNameList()
{
	QStringList		DevLst;

	PaDeviceIndex	DevCnt = Pa_GetDeviceCount();

	for( PaDeviceIndex DevIdx = 0 ; DevIdx < DevCnt ; DevIdx++ )
	{
		const PaDeviceInfo	*DevInf = Pa_GetDeviceInfo( DevIdx );

		const PaHostApiInfo *HstInf = Pa_GetHostApiInfo( DevInf->hostApi );

		if( DevInf->maxOutputChannels > 0 )
		{
			DevLst << QString( "%1: %2" ).arg( HstInf->name ).arg( DevInf->name );
		}
	}

	return( DevLst );
}

QString DevicePortAudio::deviceOutputDefaultName()
{
	PaDeviceIndex		 DevIdx = Pa_GetDefaultOutputDevice();

	if( DevIdx == paNoDevice )
	{
		return( QString() );
	}

	const PaDeviceInfo	*DevInf = Pa_GetDeviceInfo( DevIdx );

	const PaHostApiInfo *HstInf = Pa_GetHostApiInfo( DevInf->hostApi );

	return( QString( "%1: %2" ).arg( HstInf->name ).arg( DevInf->name ) );
}

PaDeviceIndex DevicePortAudio::deviceOutputNameIndex(const QString &pDeviceName)
{
	PaDeviceIndex	DevCnt = Pa_GetDeviceCount();

	for( PaDeviceIndex DevIdx = 0 ; DevIdx < DevCnt ; DevIdx++ )
	{
		const PaDeviceInfo	*DevInf = Pa_GetDeviceInfo( DevIdx );

		if( DevInf->maxOutputChannels <= 0 )
		{
			continue;
		}

		const PaHostApiInfo *HstInf = Pa_GetHostApiInfo( DevInf->hostApi );

		if( pDeviceName ==  QString( "%1: %2" ).arg( HstInf->name ).arg( DevInf->name ) )
		{
			return( DevIdx );
		}
	}

	return( paNoDevice );
}

QStringList DevicePortAudio::deviceInputNameList()
{
	QStringList		DevLst;

	PaDeviceIndex	DevCnt = Pa_GetDeviceCount();

	for( PaDeviceIndex DevIdx = 0 ; DevIdx < DevCnt ; DevIdx++ )
	{
		const PaDeviceInfo	*DevInf = Pa_GetDeviceInfo( DevIdx );

		const PaHostApiInfo *HstInf = Pa_GetHostApiInfo( DevInf->hostApi );

		if( DevInf->maxInputChannels > 0 )
		{
			DevLst << QString( "%1: %2" ).arg( HstInf->name ).arg( DevInf->name );
		}
	}

	return( DevLst );
}

QString DevicePortAudio::deviceInputDefaultName()
{
	PaDeviceIndex		 DevIdx = Pa_GetDefaultInputDevice();

	if( DevIdx == paNoDevice )
	{
		return( QString() );
	}

	const PaDeviceInfo	*DevInf = Pa_GetDeviceInfo( DevIdx );

	const PaHostApiInfo *HstInf = Pa_GetHostApiInfo( DevInf->hostApi );

	return( QString( "%1: %2" ).arg( HstInf->name ).arg( DevInf->name ) );
}

PaDeviceIndex DevicePortAudio::deviceInputNameIndex( const QString &pDeviceName )
{
	PaDeviceIndex	DevCnt = Pa_GetDeviceCount();

	for( PaDeviceIndex DevIdx = 0 ; DevIdx < DevCnt ; DevIdx++ )
	{
		const PaDeviceInfo	*DevInf = Pa_GetDeviceInfo( DevIdx );

		if( DevInf->maxInputChannels <= 0 )
		{
			continue;
		}

		const PaHostApiInfo *HstInf = Pa_GetHostApiInfo( DevInf->hostApi );

		if( pDeviceName ==  QString( "%1: %2" ).arg( HstInf->name ).arg( DevInf->name ) )
		{
			return( DevIdx );
		}
	}

	return( paNoDevice );
}

//void DevicePortAudio::delDevice( DevicePortAudio *pDelDev )
//{
//	if( pDelDev )
//	{
//		//gApp->devices().unregisterDevice( pDelDev );

//		mDeviceList.removeAll( pDelDev );

//		delete pDelDev;
//	}
//}

#endif

//-----------------------------------------------------------------------------

#if defined( PORTAUDIO_SUPPORTED )
DevicePortAudio::DevicePortAudio( PaDeviceIndex pDeviceIndex )
	: mDeviceIndex( pDeviceIndex ), mStreamOutput( 0 ), mStreamInput( 0 ), mOutputTimeLatency ( 0 ), mOutputSampleRate( 0 )
{
	const PaDeviceInfo	*DevInf = Pa_GetDeviceInfo( mDeviceIndex );

	if( !DevInf )
	{
		return;
	}

	mInputChannelCount  = DevInf->maxInputChannels;
	mOutputChannelCount = DevInf->maxOutputChannels;

	if( DevInf->maxInputChannels > 0 )
	{
		deviceInputOpen( DevInf );
	}

	if( DevInf->maxOutputChannels > 0 )
	{
		deviceOutputOpen( DevInf );
	}
}

DevicePortAudio::~DevicePortAudio( void )
{
	if( mInputChannelCount > 0 )
	{
		deviceInputClose();
	}

	if( mOutputChannelCount > 0 )
	{
		deviceOutputClose();
	}
}
#endif
void DevicePortAudio::setTimeOffset( qreal pTimeOffset )
{
	Q_UNUSED( pTimeOffset )

//	mTimeOffset				= pTimeOffset;
//	mStreamOutputTimeOffset = mTimeOffset;
	//	mStreamOutputOffset     = 0;
}

void *DevicePortAudio::allocAudioInstance( qreal pSampleRate, AudioSampleFormat pSampleFormat, int pChannels )
{
	if( pSampleRate != mInputSampleRate || pSampleFormat != mInputSampleFormat || pChannels != mInputChannelCount )
	{
		return( 0 );
	}

	InputInstanceData		*AID = new InputInstanceData();

	if( AID )
	{
		AID->mSampleRate   = pSampleRate;
		AID->mSampleFormat = pSampleFormat;
		AID->mChannels     = pChannels;
	}

	return( AID );
}

void DevicePortAudio::freeAudioInstance( void *pInstanceData )
{
	InputInstanceData		*AID = static_cast<InputInstanceData *>( pInstanceData );

	if( AID )
	{
		delete AID;
	}
}

void DevicePortAudio::audio( qint64 pSamplePosition, qint64 pSampleCount, int pChannelOffset, int pChannelCount, float **pBuffers, void *pInstanceData ) const
{
	InputInstanceData		*AID = static_cast<InputInstanceData *>( pInstanceData );

	if( !AID )
	{
		return;
	}

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

		qint64		DstPos = std::max<qint64>( 0, PosBeg - pSamplePosition );

		for( int i = 0 ; i < mInputChannelCount ; i++ )
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

#if defined( PORTAUDIO_SUPPORTED )
int DevicePortAudio::streamCallbackStatic( const void *input, void *output, unsigned long frameCount, const PaStreamCallbackTimeInfo *timeInfo, PaStreamCallbackFlags statusFlags, void *userData)
{
	Q_UNUSED( statusFlags )

	if( input )
	{
		return( ((DevicePortAudio *)userData)->streamCallbackInput( input, frameCount, timeInfo, statusFlags ) );
	}

	if( output )
	{
		return( ((DevicePortAudio *)userData)->streamCallbackOutput( output, frameCount, timeInfo, statusFlags ) );
	}

	return( paContinue );
}

int DevicePortAudio::streamCallbackOutput( void *output, unsigned long frameCount, const PaStreamCallbackTimeInfo *timeInfo, PaStreamCallbackFlags statusFlags )
{
	Q_UNUSED( statusFlags )

//	if( mStreamOutputOffset == 0 )
//	{
//		mStreamOutputOffset = timeInfo->currentTime;

//		mAudioOffset = mStreamOutputTimeOffset * sampleRate();
//	}

//	mStreamOutputTime = ( timeInfo->currentTime - mStreamOutputOffset ) + mStreamOutputTimeOffset;

	//-------------------------------------------------------------------------

	mOutputTimeInfo = *timeInfo;

	//-------------------------------------------------------------------------

	float									**AudioBuffers = (float **)output;

	for( int i = 0 ; i < mOutputChannelCount ; i++ )
	{
		memset( AudioBuffers[ i ], 0, frameCount * sizeof( float ) );
	}

	//-------------------------------------------------------------------------

	mProducerMutex.lock();

	for( const AudioInstanceData &AID : mProducers )
	{
		AID.mProducer->audio( mOutputAudioOffset, frameCount, 0, mOutputChannelCount, AudioBuffers, AID.mInstance );
	}

	mProducerMutex.unlock();

	mOutputAudioOffset += frameCount;

	return( paContinue );
}

int DevicePortAudio::streamCallbackInput( const void *input, unsigned long frameCount, const PaStreamCallbackTimeInfo *timeInfo, PaStreamCallbackFlags statusFlags )
{
	Q_UNUSED( statusFlags )

	qint64		CurrTime = ( PortAudioPlugin::instance()->fugio()->timestamp() * qint64( mInputSampleRate ) ) / 1000;

	if( !mInputAudioOffset )
	{
		mInputAudioOffset = CurrTime;

		mInputAudioOffset -= ( mInputSampleRate * mInputTimeLatency ) / 1000.0;
	}

	if( CurrTime - mInputAudioOffset > 1000 )
	{
		qDebug() << "DevicePortAudio" << ( CurrTime - mInputAudioOffset );

		mInputAudioOffset = CurrTime;
	}

	mInputTimeInfo = *timeInfo;

	mProducerMutex.lock();

	while( !mAudioBuffers.isEmpty() )
	{
		AudioBuffer	AB = mAudioBuffers.first();

		if( mInputAudioOffset - ( AB.mPosition + AB.mSamples ) > 2 * mInputSampleRate )
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

				delete [] AB.mData;

				AB.mData = nullptr;
			}

			mAudioBuffers.removeFirst();
		}
		else
		{
			break;
		}
	}

	AudioBuffer		AB;

	memset( &AB, 0, sizeof( AB ) );

	audioInput( AB, (const float **)input, frameCount, mInputChannelCount, mInputAudioOffset );

	mAudioBuffers.append( AB );

	mInputAudioOffset += frameCount;

	mProducerMutex.unlock();

	return( paContinue );
}

void DevicePortAudio::audioInput( DevicePortAudio::AudioBuffer &AB, const float **pData, qint64 pSampleCount, int pChannelCount, qint64 pSamplePosition )
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

void DevicePortAudio::deviceInputOpen( const PaDeviceInfo *DevInf )
{
	deviceInputClose();

	PaStreamParameters	 StrPrm;

	memset( &StrPrm, 0, sizeof( StrPrm ) );

	StrPrm.device           = mDeviceIndex;
	StrPrm.channelCount     = DevInf->maxInputChannels;
	StrPrm.sampleFormat     = paNonInterleaved | paFloat32;
	StrPrm.suggestedLatency = DevInf->defaultLowInputLatency;

	if( Pa_OpenStream( &mStreamInput, &StrPrm, nullptr, outputSampleRate(), paFramesPerBufferUnspecified, paNoFlag, &DevicePortAudio::streamCallbackStatic, this ) != paNoError )
	{
		return;
	}

	mInputChannelCount = DevInf->maxInputChannels;

	const PaStreamInfo	*StreamInfo = Pa_GetStreamInfo( mStreamInput );

	qDebug() << DevInf->name << StreamInfo->sampleRate << StreamInfo->inputLatency << mInputChannelCount;

	mInputSampleRate   = StreamInfo->sampleRate;
	mInputTimeLatency  = StreamInfo->inputLatency;
	mInputAudioOffset  = 0; //QDateTime::currentMSecsSinceEpoch() * qint64( mSampleRate / 1000.0 );
	mInputSampleFormat = fugio::AudioSampleFormat::Format32FS;

	if( Pa_StartStream( mStreamInput ) != paNoError )
	{
		return;
	}
}

void DevicePortAudio::deviceOutputOpen( const PaDeviceInfo *DevInf )
{
	deviceOutputClose();

	PaStreamParameters	 StrPrm;

	memset( &StrPrm, 0, sizeof( StrPrm ) );

	StrPrm.device           = mDeviceIndex;
	StrPrm.channelCount     = DevInf->maxOutputChannels;
	StrPrm.sampleFormat     = paNonInterleaved | paFloat32;
	StrPrm.suggestedLatency = DevInf->defaultLowOutputLatency;

	if( Pa_OpenStream( &mStreamOutput, 0, &StrPrm, outputSampleRate(), paFramesPerBufferUnspecified, paNoFlag, &DevicePortAudio::streamCallbackStatic, this ) != paNoError )
	{
		return;
	}

	mOutputChannelCount = DevInf->maxOutputChannels;

	const PaStreamInfo	*StreamInfo = Pa_GetStreamInfo( mStreamOutput );

	qDebug() << DevInf->name << StreamInfo->sampleRate << StreamInfo->outputLatency << mOutputChannelCount;

	mOutputSampleRate   = StreamInfo->sampleRate;
	mOutputTimeLatency  = StreamInfo->outputLatency;
	mOutputAudioOffset  = ( PortAudioPlugin::instance()->fugio()->timestamp() * mOutputSampleRate ) / 1000;

	mOutputAudioOffset -= ( mOutputTimeLatency * mOutputSampleRate ) / 1000;

	if( Pa_StartStream( mStreamOutput ) != paNoError )
	{
		return;
	}

	//emit latencyUpdated( mTimeLatency );
}

void DevicePortAudio::deviceOutputClose()
{
	if( mStreamOutput != 0 )
	{
		Pa_CloseStream( mStreamOutput );

		mStreamOutput = 0;
	}

//	mStreamOutputOffset = 0;

	mOutputSampleRate   = 0;
	mOutputTimeLatency  = 0;
	mOutputChannelCount = 0;
}

void DevicePortAudio::deviceInputClose()
{
	if( mStreamInput )
	{
		Pa_CloseStream( mStreamInput );

		mStreamInput = nullptr;
	}

	for( AudioBuffer &AB : mAudioBuffers )
	{
		for( int i = 0 ; i < AB.mChannels ; i++ )
		{
			delete [] AB.mData[ i ];
		}

		delete [] AB.mData;
	}

	mInputTimeLatency  = 0;
	mInputChannelCount = 0;
}
#endif

qreal DevicePortAudio::outputSampleRate() const
{
	return( mOutputSampleRate > 0 ? mOutputSampleRate : QSettings().value( "audio/output-sample-rate", double( AUDIO_DEFAULT_SAMPLE_RATE ) ).toDouble() );
}

void DevicePortAudio::addProducer( AudioProducerInterface *pAudioProducer )
{
	AudioInstanceData		AID;

	AID.mProducer = pAudioProducer;
	AID.mInstance = pAudioProducer->allocAudioInstance( mOutputSampleRate, fugio::AudioSampleFormat::Format32FS, mOutputChannelCount );

	mProducerMutex.lock();

	mProducers.append( AID );

	mProducerMutex.unlock();
}

void DevicePortAudio::remProducer( AudioProducerInterface *pAudioProducer )
{
	mProducerMutex.lock();

	for( int i = 0 ; i < mProducers.size() ; i++ )
	{
		AudioInstanceData		AID = mProducers.at( i );

		if( AID.mProducer != pAudioProducer )
		{
			continue;
		}

		AID.mProducer->freeAudioInstance( AID.mInstance );

		mProducers.removeAt( i );

		break;
	}

	mProducerMutex.unlock();
}


int DevicePortAudio::audioChannels() const
{
	return( mInputChannelCount );
}

qreal DevicePortAudio::audioSampleRate() const
{
	return( mInputSampleRate );
}

AudioSampleFormat DevicePortAudio::audioSampleFormat() const
{
	return( mInputSampleFormat );
}

qint64 DevicePortAudio::audioLatency() const
{
	return( ( mInputSampleRate * mInputTimeLatency ) / 1000.0 );
}
