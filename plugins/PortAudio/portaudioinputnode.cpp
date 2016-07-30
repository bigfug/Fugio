#include "portaudioinputnode.h"

#include <QSettings>
#include <QPushButton>
#include <QInputDialog>
#include <QDebug>

#include <fugio/audio/audio_producer_interface.h>
#include <fugio/context_interface.h>
#include <fugio/audio/uuid.h>

PortAudioInputNode::PortAudioInputNode( QSharedPointer<fugio::NodeInterface> pNode ) :
	NodeControlBase( pNode ), mPortAudio( nullptr ), mInstance( nullptr )
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
		return( true );
	}
#else
	mNode->setStatus( fugio::NodeInterface::Error );
	mNode->setStatusMessage( "No PortAudio Support" );
#endif

	return( false );
}

bool PortAudioInputNode::deinitialise()
{
	if( mPortAudio )
	{
		mPortAudio.clear();
	}

	return( NodeControlBase::deinitialise() );
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

void PortAudioInputNode::audioDeviceSelected(const QString &pDeviceName)
{
	if( mPortAudio )
	{
		mPortAudio.clear();
	}

	mDeviceName = pDeviceName;

#if defined( PORTAUDIO_SUPPORTED )
	mDeviceIndex = DevicePortAudio::deviceInputNameIndex( mDeviceName );

	if( mDeviceIndex == paNoDevice )
	{
		return;
	}

	if( ( mPortAudio = DevicePortAudio::newDevice( mDeviceIndex ) ) )
	{
	}
#endif

	pinUpdated( mPinAudio );
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

void PortAudioInputNode::saveSettings( QSettings &pSettings ) const
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
	return( mPortAudio ? mPortAudio->inputSampleFormat() : fugio::AudioSampleFormat::FormatUnknown );
}

bool PortAudioInputNode::isValid( fugio::AudioInstanceBase *pInstance ) const
{
	if( !mPortAudio )
	{
		return( false );
	}

	if( !mPortAudio->isValid( pInstance ) )
	{
		return( false );
	}

	DevicePortAudio::AudioInstanceData	*AudIns = dynamic_cast<DevicePortAudio::AudioInstanceData *>( pInstance );

	if( !AudIns )
	{
		return( false );
	}

	if( AudIns->mDeviceIndex != mDeviceIndex )
	{
		return( false );
	}

	return( true );
}

fugio::AudioInstanceBase *PortAudioInputNode::audioAllocInstance( qreal pSampleRate, fugio::AudioSampleFormat pSampleFormat, int pChannels )
{
	if( !mPortAudio )
	{
		return( nullptr );
	}

	return( mPortAudio->audioAllocInstance( pSampleRate, pSampleFormat, pChannels ) );
}

qint64 PortAudioInputNode::audioLatency() const
{
	return( mPortAudio ? mPortAudio->audioLatency() : 0 );
}
