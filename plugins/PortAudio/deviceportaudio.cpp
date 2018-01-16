#include "deviceportaudio.h"

#include <QDebug>
#include <QDateTime>

#include <fugio/context_interface.h>
#include <fugio/audio/audio_producer_interface.h>

#include "portaudiooutputnode.h"
#include "portaudioplugin.h"

#define AUDIO_DEFAULT_SAMPLE_RATE	(48000)

QList<QWeakPointer<DevicePortAudio>>	 DevicePortAudio::mDeviceList;

#if defined( Q_OS_WIN )
#include <windows.h>
#endif

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

		QString				 DevNam;

#if defined( Q_OS_WIN )
		wchar_t wideName[ MAX_PATH ];

		MultiByteToWideChar( CP_UTF8, 0, DevInf->name, -1, wideName, MAX_PATH - 1 );

		DevNam = QString::fromWCharArray( wideName );
#else
		DevNam = QString::fromLocal8Bit( DevInf->name );
#endif

		qDebug() << HstInf->name << DevNam << DevInf->maxInputChannels << DevInf->maxOutputChannels;
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

QString DevicePortAudio::deviceName( PaDeviceIndex pDevIdx )
{
	if( pDevIdx == paNoDevice )
	{
		return( QString() );
	}

	const PaDeviceInfo	*DevInf = Pa_GetDeviceInfo( pDevIdx );

	if( !DevInf )
	{
		return( QString() );
	}

	QString				 DevNam;

#if defined( Q_OS_WIN )
	wchar_t wideName[ MAX_PATH ];

	MultiByteToWideChar( CP_UTF8, 0, DevInf->name, -1, wideName, MAX_PATH - 1 );

	DevNam = QString::fromWCharArray( wideName );
#else
	DevNam = QString::fromLocal8Bit( DevInf->name );
#endif


	const PaHostApiInfo *HstInf = Pa_GetHostApiInfo( DevInf->hostApi );

	if( !HstInf )
	{
		return( QString() );
	}

	return( QString( "%1: %2" ).arg( HstInf->name ).arg( DevNam ) );
}

QStringList DevicePortAudio::deviceOutputNameList()
{
	QStringList		DevLst;

	PaDeviceIndex	DevCnt = Pa_GetDeviceCount();

	for( PaDeviceIndex DevIdx = 0 ; DevIdx < DevCnt ; DevIdx++ )
	{
		const PaDeviceInfo	*DevInf = Pa_GetDeviceInfo( DevIdx );

		if( DevInf->maxOutputChannels <= 0 )
		{
			continue;
		}

		DevLst << deviceName( DevIdx );
	}

	return( DevLst );
}

QString DevicePortAudio::deviceOutputDefaultName()
{
	return( deviceName( Pa_GetDefaultOutputDevice() ) );
}

PaDeviceIndex DevicePortAudio::deviceOutputNameIndex( const QString &pDeviceName )
{
	PaDeviceIndex	DevCnt = Pa_GetDeviceCount();

	for( PaDeviceIndex DevIdx = 0 ; DevIdx < DevCnt ; DevIdx++ )
	{
		const PaDeviceInfo	*DevInf = Pa_GetDeviceInfo( DevIdx );

		if( DevInf->maxOutputChannels <= 0 )
		{
			continue;
		}

		if( pDeviceName == deviceName( DevIdx ) )
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

		if( DevInf->maxInputChannels > 0 )
		{
			DevLst << deviceName( DevIdx );
		}
	}

	return( DevLst );
}

QString DevicePortAudio::deviceInputDefaultName()
{
	return( deviceName( Pa_GetDefaultInputDevice() ) );
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

		if( pDeviceName == deviceName( DevIdx ) )
		{
			return( DevIdx );
		}
	}

	return( paNoDevice );
}

#endif

//-----------------------------------------------------------------------------

#if defined( PORTAUDIO_SUPPORTED )
DevicePortAudio::DevicePortAudio( PaDeviceIndex pDeviceIndex )
	: mDeviceIndex( pDeviceIndex ), mStreamOutput( 0 ), mStreamInput( 0 ), mOutputTimeLatency( 0 ), mInputTimeLatency( 0 ),
	  mOutputAudioOffset( 0 ), mInputAudioOffset( 0 ), mOutputSampleRate( 0 ), mOutputChannelCount( 0 ), mInputChannelCount( 0 ),
	  mInputSampleRate( 0 ), mInputSampleFormat( fugio::AudioSampleFormat::FormatUnknown )
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
		mInputChannelCount = DevInf->maxInputChannels;
		mInputSampleRate   = AUDIO_DEFAULT_SAMPLE_RATE;
		mInputSampleFormat = fugio::AudioSampleFormat::Format32FS;
		mInputTimeLatency  = DevInf->defaultLowInputLatency;
		mInputAudioOffset  = 0;
	}

	if( DevInf->maxOutputChannels > 0 )
	{
		mOutputChannelCount = DevInf->maxOutputChannels;
		mOutputSampleRate   = AUDIO_DEFAULT_SAMPLE_RATE;
		mOutputTimeLatency  = DevInf->defaultLowOutputLatency;
		mOutputAudioOffset  = 0;
	}
}
#endif

DevicePortAudio::~DevicePortAudio( void )
{
#if defined( PORTAUDIO_SUPPORTED )
	if( mStreamInput )
	{
		deviceInputClose();
	}

	if( mStreamOutput )
	{
		deviceOutputClose();
	}
#endif
}

void DevicePortAudio::setTimeOffset( qreal pTimeOffset )
{
	Q_UNUSED( pTimeOffset )

//	mTimeOffset				= pTimeOffset;
//	mStreamOutputTimeOffset = mTimeOffset;
	//	mStreamOutputOffset     = 0;
}

fugio::AudioInstanceBase *DevicePortAudio::audioAllocInstance( qreal pSampleRate, AudioSampleFormat pSampleFormat, int pChannels )
{
#if defined( PORTAUDIO_SUPPORTED )
	if( !mStreamInput )
	{
		const PaDeviceInfo	*DevInf = Pa_GetDeviceInfo( mDeviceIndex );

		if( !DevInf )
		{
			return( 0 );
		}

		deviceInputOpen( DevInf );
	}

	if( pSampleRate != mInputSampleRate || pSampleFormat != mInputSampleFormat || pChannels != mInputChannelCount )
	{
		return( 0 );
	}

	for( QSharedPointer<DevicePortAudio> DevAud : mDeviceList )
	{
		if( DevAud && DevAud->mDeviceIndex == mDeviceIndex )
		{
			AudioInstanceData		*AID = new AudioInstanceData( DevAud, pSampleRate, pSampleFormat, pChannels );

			if( AID )
			{
				AID->mDeviceIndex = mDeviceIndex;
			}

			return( AID );
		}
	}
#endif

	return( nullptr );
}

void DevicePortAudio::audio( qint64 pSamplePosition, qint64 pSampleCount, int pChannelOffset, int pChannelCount, void **pBuffers, AudioInstanceData *pInstanceData ) const
{
	Q_UNUSED( pInstanceData )

	QMutexLocker		Lock( &mProducerMutex );

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
				float			*DstPtr = &reinterpret_cast<float **>( pBuffers )[ i ][ DstPos ];

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
void DevicePortAudio::deviceInputOpen( const PaDeviceInfo *DevInf )
{
	deviceInputClose();

	PaStreamParameters	 StrPrm;

	memset( &StrPrm, 0, sizeof( StrPrm ) );

	StrPrm.device           = mDeviceIndex;
	StrPrm.channelCount     = DevInf->maxInputChannels;
	StrPrm.sampleFormat     = paNonInterleaved | paFloat32;
	StrPrm.suggestedLatency = DevInf->defaultLowInputLatency;

	if( Pa_OpenStream( &mStreamInput, &StrPrm, nullptr, mInputSampleRate, paFramesPerBufferUnspecified, paNoFlag, &DevicePortAudio::streamCallbackStatic, this ) != paNoError )
	{
		return;
	}

	mInputChannelCount = DevInf->maxInputChannels;

	const PaStreamInfo	*StreamInfo = Pa_GetStreamInfo( mStreamInput );

	qDebug() << "INPUT:" << deviceName( mDeviceIndex ) << StreamInfo->sampleRate << StreamInfo->inputLatency << mInputChannelCount;

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

#if defined( Q_OS_RASPBERRY_PI )
	StrPrm.suggestedLatency = DevInf->defaultHighOutputLatency;
#else
	StrPrm.suggestedLatency = DevInf->defaultLowOutputLatency;
#endif

	if( Pa_OpenStream( &mStreamOutput, 0, &StrPrm, outputSampleRate(), paFramesPerBufferUnspecified, paNoFlag, &DevicePortAudio::streamCallbackStatic, this ) != paNoError )
	{
		return;
	}

	mOutputChannelCount = DevInf->maxOutputChannels;

	const PaStreamInfo	*StreamInfo = Pa_GetStreamInfo( mStreamOutput );

	qDebug() << "OUTPUT:" << deviceName( mDeviceIndex ) << StreamInfo->sampleRate << StreamInfo->outputLatency << mOutputChannelCount;

	mOutputSampleRate   = StreamInfo->sampleRate;
	mOutputTimeLatency  = StreamInfo->outputLatency;
	mOutputAudioOffset  = 0;

	//mOutputAudioOffset -= ( mOutputTimeLatency * mOutputSampleRate ) / 1000;

	if( Pa_StartStream( mStreamOutput ) != paNoError )
	{
		return;
	}

	//emit latencyUpdated( mTimeLatency );
}

void DevicePortAudio::deviceOutputClose()
{
	if( mStreamOutput )
	{
		Pa_CloseStream( mStreamOutput );

		mStreamOutput = 0;
	}
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
}

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

	qint64	ExpectedOutputOffset  = ( PortAudioPlugin::instance()->fugio()->timestamp() * mOutputSampleRate ) / 1000;

	if( qAbs( ExpectedOutputOffset - mOutputAudioOffset ) > mOutputSampleRate / 4 )
	{
		qDebug() << "Audio Output Offset Reset" << ( ExpectedOutputOffset - mOutputAudioOffset );

		mOutputAudioOffset = ExpectedOutputOffset;
	}

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

	// Calculate the maximum input latency

	qint64		MaxLat = 0;

	for( PortAudioOutputNode *PAO : mProducers )
	{
		MaxLat = std::max<qint64>( PAO->audioLatency(), MaxLat );
	}

	MaxLat += mOutputTimeLatency * mOutputSampleRate;

	// Get the audio

	for( PortAudioOutputNode *PAO : mProducers )
	{
		PAO->audio( mOutputAudioOffset - MaxLat, frameCount, 0, mOutputChannelCount, (void **)AudioBuffers, nullptr );
	}

	mProducerMutex.unlock();

	mOutputAudioOffset += frameCount;

	return( paContinue );
}

int DevicePortAudio::streamCallbackInput( const void *input, unsigned long frameCount, const PaStreamCallbackTimeInfo *timeInfo, PaStreamCallbackFlags statusFlags )
{
	Q_UNUSED( statusFlags )

	qint64		CurrTime = ( ( PortAudioPlugin::instance()->fugio()->timestamp() * qint64( mInputSampleRate ) ) / 1000 ) - audioLatency();

	if( !mInputAudioOffset )
	{
		mInputAudioOffset = CurrTime;
	}

	if( CurrTime - mInputAudioOffset > mInputSampleRate / 4 )
	{
		qDebug() << "Input Offset Reset" << ( CurrTime - mInputAudioOffset );

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

#endif

qreal DevicePortAudio::outputSampleRate() const
{
	return( mOutputSampleRate );
}

void DevicePortAudio::addOutput( PortAudioOutputNode *pOutput )
{
#if defined( PORTAUDIO_SUPPORTED )
	if( mProducers.isEmpty() && mOutputChannelCount )
	{
		const PaDeviceInfo	*DevInf = Pa_GetDeviceInfo( mDeviceIndex );

		if( !DevInf )
		{
			return;
		}

		deviceOutputOpen( DevInf );
	}
#endif

	mProducerMutex.lock();

	mProducers.append( pOutput );

	mProducerMutex.unlock();
}

void DevicePortAudio::remOutput( PortAudioOutputNode *pOutput )
{
	mProducerMutex.lock();

	mProducers.removeAll( pOutput );

	mProducerMutex.unlock();

#if defined( PORTAUDIO_SUPPORTED )
	if( mProducers.isEmpty() )
	{
		deviceOutputClose();
	}
#endif
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
#if defined( PORTAUDIO_SUPPORTED )
	return( ( mInputSampleRate * mInputTimeLatency ) / 1000.0 );
#else
	return( 0 );
#endif
}

bool DevicePortAudio::isValid( AudioInstanceBase *pInstance ) const
{
	return( pInstance ? pInstance->isValid() : false );
}
