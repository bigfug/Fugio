#include "portaudioinputnode.h"

#include <QSettings>
#include <QPushButton>
#include <QInputDialog>
#include <QDebug>

#include <fugio/audio/audio_producer_interface.h>
#include <fugio/context_interface.h>
#include <fugio/audio/uuid.h>

PortAudioInputNode::PortAudioInputNode( QSharedPointer<fugio::NodeInterface> pNode ) :
	NodeControlBase( pNode ), mPortAudio( nullptr )
{
	mValAudio = pinOutput<fugio::AudioProducerInterface *>( "Audio", mPinAudio, PID_AUDIO );

#if defined( PORTAUDIO_SUPPORTED )
	mDeviceName = DevicePortAudio::deviceInputDefaultName();
#endif
}

PortAudioInputNode::~PortAudioInputNode()
{
	if( mPortAudio )
	{
		//disconnect( mPortAudio.data(), SIGNAL(audio(const float**,quint64,int,qint64)), this, SLOT(audioInput(const float**,quint64,int,qint64)) );

		mPortAudio.clear();
	}
}

bool PortAudioInputNode::initialise()
{
	if( !NodeControlBase::initialise() )
	{
		return( false );
	}

#if defined( PORTAUDIO_SUPPORTED )
	PaDeviceIndex		DevIdx = DevicePortAudio::deviceInputNameIndex( mDeviceName );

	if( DevIdx == paNoDevice )
	{
		return( false );
	}

	if( ( mPortAudio = DevicePortAudio::newDevice( DevIdx ) ) )
	{
		//connect( mPortAudio.data(), SIGNAL(audio(const float**,quint64,int,qint64)), this, SLOT(audioInput(const float**,quint64,int,qint64)) );
	}

	return( true );
#else
	mNode->setStatus( fugio::NodeInterface::Error );
	mNode->setStatusMessage( "No PortAudio Support" );

	return( false );
#endif
}

bool PortAudioInputNode::deinitialise()
{
	if( mPortAudio )
	{
		//disconnect( mPortAudio.data(), SIGNAL(audio(const float**,quint64,int,qint64)), this, SLOT(audioInput(const float**,quint64,int,qint64)) );

		mPortAudio.clear();
	}

	return( NodeControlBase::deinitialise() );
}

void PortAudioInputNode::audio( qint64 pSamplePosition, qint64 pSampleCount, int pChannelOffset, int pChannelCount, float **pBuffers, qint64 pLatency, void *pInstanceData ) const
{
	Q_UNUSED( pInstanceData )
	Q_UNUSED( pLatency )

	//AudioInstanceData		*AID = static_cast<AudioInstanceData *>( pInstanceData );

	mPortAudio->audio( pSamplePosition, pSampleCount, pChannelOffset, pChannelCount, pBuffers );
}

void PortAudioInputNode::clicked()
{
#if defined( PORTAUDIO_SUPPORTED )
	bool		OK;
	QString		NewDev = QInputDialog::getItem( nullptr, mNode->name(), tr( "Select Audio Device" ), DevicePortAudio::deviceInputNameList(), DevicePortAudio::deviceInputNameList().indexOf( mDeviceName ), false, &OK );

	if( OK && NewDev != mDeviceName )
	{
		audioDeviceSelected( NewDev );
	}
#endif
}

void *PortAudioInputNode::allocAudioInstance( qreal pSampleRate, fugio::AudioSampleFormat pSampleFormat, int pChannels )
{
	AudioInstanceData		*AID = new AudioInstanceData();

	if( AID )
	{
		AID->mSampleRate   = pSampleRate;
		AID->mSampleFormat = pSampleFormat;
		AID->mChannels     = pChannels;
	}

	return( AID );
}

void PortAudioInputNode::freeAudioInstance( void *pInstanceData )
{
	AudioInstanceData		*AID = static_cast<AudioInstanceData *>( pInstanceData );

	if( AID )
	{
		delete AID;
	}
}

void PortAudioInputNode::audioDeviceSelected(const QString &pDeviceName)
{
	if( mPortAudio )
	{
		//disconnect( mPortAudio.data(), SIGNAL(audio(const float**,quint64,int,qint64)), this, SLOT(audioInput(const float**,quint64,int,qint64)) );

		mPortAudio.clear();
	}

	mDeviceName = pDeviceName;

#if defined( PORTAUDIO_SUPPORTED )
	PaDeviceIndex		DevIdx = DevicePortAudio::deviceInputNameIndex( mDeviceName );

	if( DevIdx == paNoDevice )
	{
		return;
	}

	if( ( mPortAudio = DevicePortAudio::newDevice( DevIdx ) ) )
	{
		//connect( mPortAudio.data(), SIGNAL(audio(const float**,quint64,int,qint64)), this, SLOT(audioInput(const float**,quint64,int,qint64)) );
	}
#endif
}

QWidget *PortAudioInputNode::gui()
{
	QPushButton		*GUI = new QPushButton( tr( "Choose Device" ) );

	if( GUI )
	{
		connect( GUI, SIGNAL(released()), this, SLOT(clicked()) );
	}

	return( GUI );
}

void PortAudioInputNode::loadSettings( QSettings &pSettings )
{
	mDeviceName = pSettings.value( "device", mDeviceName ).toString();

	emit audioDeviceChanged( mDeviceName );
}

void PortAudioInputNode::saveSettings( QSettings &pSettings )
{
	pSettings.setValue( "device", mDeviceName );
}


int PortAudioInputNode::audioChannels() const
{
	return( mPortAudio ? mPortAudio->inputChannelCount() : 0 );
}

qreal PortAudioInputNode::audioSampleRate() const
{
	return( mPortAudio ? mPortAudio->inputSampleRate() : 0 );
}

fugio::AudioSampleFormat PortAudioInputNode::audioSampleFormat() const
{
	return( mPortAudio ? mPortAudio->inputSampleFormat() : fugio::AudioSampleFormat::FMT_UNKNOWN );
}
