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

	qDebug() << "DefInp:" << Pa_GetDefaultInputDevice() << "DefOut:" << Pa_GetDefaultOutputDevice();

	PaDeviceIndex	DevCnt = Pa_GetDeviceCount();

	for( PaDeviceIndex DevIdx = 0 ; DevIdx < DevCnt ; DevIdx++ )
	{
		const PaDeviceInfo	*DevInf = Pa_GetDeviceInfo( DevIdx );

		const PaHostApiInfo *HstInf = Pa_GetHostApiInfo( DevInf->hostApi );

		qDebug() << HstInf->name << DevInf->name << DevInf->maxInputChannels << DevInf->maxOutputChannels;
	}
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

//void DevicePortAudio::delDevice( DevicePortAudio *pDelDev )
//{
//	if( pDelDev )
//	{
//		//gApp->devices().unregisterDevice( pDelDev );

//		mDeviceList.removeAll( pDelDev );

//		delete pDelDev;
//	}
//}

//-----------------------------------------------------------------------------

DevicePortAudio::DevicePortAudio( PaDeviceIndex pDeviceIndex )
	: mDeviceIndex( pDeviceIndex ), mStreamOutput( 0 ), mStreamInput( 0 ), mOutputTimeLatency ( 0 ), mSampleRate( 0 )
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

void DevicePortAudio::setTimeOffset( qreal pTimeOffset )
{
	Q_UNUSED( pTimeOffset )

//	mTimeOffset				= pTimeOffset;
//	mStreamOutputTimeOffset = mTimeOffset;
//	mStreamOutputOffset     = 0;
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
		AID.mProducer->audio( mOutputAudioOffset, frameCount, 0, mOutputChannelCount, AudioBuffers, mOutputTimeLatency * mSampleRate, AID.mInstance );
	}

	mProducerMutex.unlock();

	mOutputAudioOffset += frameCount;

	return( paContinue );
}

int DevicePortAudio::streamCallbackInput( const void *input, unsigned long frameCount, const PaStreamCallbackTimeInfo *timeInfo, PaStreamCallbackFlags statusFlags )
{
	Q_UNUSED( statusFlags )

	if( !mInputAudioOffset )
	{
		mInputAudioOffset = PortAudioPlugin::instance()->fugio()->timestamp() * qint64( mSampleRate / 1000.0 );
	}

	mInputTimeInfo = *timeInfo;

	emit audio( (const float **)input, frameCount, mInputChannelCount, mInputAudioOffset );

	mInputAudioOffset += frameCount;

	return( paContinue );
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

	if( Pa_OpenStream( &mStreamInput, &StrPrm, nullptr, sampleRate(), paFramesPerBufferUnspecified, paNoFlag, &DevicePortAudio::streamCallbackStatic, this ) != paNoError )
	{
		return;
	}

	mInputChannelCount = DevInf->maxInputChannels;

	const PaStreamInfo	*StreamInfo = Pa_GetStreamInfo( mStreamInput );

	qDebug() << DevInf->name << StreamInfo->sampleRate << StreamInfo->inputLatency << mInputChannelCount;

	mSampleRate        = StreamInfo->sampleRate;
	mInputTimeLatency  = StreamInfo->inputLatency;
	mInputAudioOffset  = 0; //QDateTime::currentMSecsSinceEpoch() * qint64( mSampleRate / 1000.0 );

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

	if( Pa_OpenStream( &mStreamOutput, 0, &StrPrm, sampleRate(), paFramesPerBufferUnspecified, paNoFlag, &DevicePortAudio::streamCallbackStatic, this ) != paNoError )
	{
		return;
	}

	mOutputChannelCount = DevInf->maxOutputChannels;

	const PaStreamInfo	*StreamInfo = Pa_GetStreamInfo( mStreamOutput );

	qDebug() << DevInf->name << StreamInfo->sampleRate << StreamInfo->outputLatency << mOutputChannelCount;

	mSampleRate   = StreamInfo->sampleRate;
	mOutputTimeLatency  = StreamInfo->outputLatency;
	mOutputAudioOffset  = PortAudioPlugin::instance()->fugio()->timestamp() * qint64( mSampleRate / 1000.0 );

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

	mSampleRate   = 0;
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

	mInputTimeLatency  = 0;
	mInputChannelCount = 0;
}

qreal DevicePortAudio::sampleRate() const
{
	return( mSampleRate > 0 ? mSampleRate : QSettings().value( "audio/sample-rate", double( AUDIO_DEFAULT_SAMPLE_RATE ) ).toDouble() );
}

void DevicePortAudio::addProducer( AudioProducerInterface *pAudioProducer )
{
	AudioInstanceData		AID;

	AID.mProducer = pAudioProducer;
	AID.mInstance = pAudioProducer->allocAudioInstance( mSampleRate, FMT_FLT_S, mOutputChannelCount );

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

//void DevicePortAudio::setCurrentNode( QSharedPointer<fugio::NodeInterface> pNode )
//{
//	QSharedPointer<fugio::NodeInterface>	N = mNode.toStrongRef();

//	if( N && N->hasControl() )
//	{
//		if( N->hasControl() )
//		{
//			QSharedPointer<PinControlInterface>		  C;
//			PortAudioOutputNode						 *PortAudioNode;

//			if( ( PortAudioNode = qobject_cast<PortAudioOutputNode *>( N->control()->qobject() ) ) != 0 && ( C = PortAudioNode->audioControl() ) != 0 )
//			{
//				InterfaceAudioProducer	*A = qobject_cast<InterfaceAudioProducer *>( C->qobject() );

//				if( A && A == mProducer && mProducerInstance )
//				{
//					mProducer->freeAudioInstance( mProducerInstance );
//				}
//			}
//		}

//		N.clear();

//		mProducer = nullptr;
//		mProducerInstance = nullptr;
//	}

//	mNode = pNode;

//	if( pNode && pNode->hasControl() )
//	{
//		QSharedPointer<PinControlInterface>		  C;
//		PortAudioOutputNode						 *PortAudioNode;

//		if( ( PortAudioNode = qobject_cast<PortAudioOutputNode *>( pNode->control()->qobject() ) ) != 0 && ( C = PortAudioNode->audioControl() ) != 0 )
//		{
//			InterfaceAudioProducer	*A = qobject_cast<InterfaceAudioProducer *>( C->qobject() );

//			if( A )
//			{
//				mProducer = A;
//				mProducerInstance = mProducer->allocAudioInstance();
//			}
//		}

//		pNode->context()->updateNode( pNode );
//	}
//}

