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

	rebuildDeviceList();
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

#if !defined( PORTAUDIO_SUPPORTED )
	mNode->setStatus( fugio::NodeInterface::Error );
	mNode->setStatusMessage( "No PortAudio Support" );

	return( false );
#else
	if( mDeviceName.isEmpty() )
	{
		mDeviceName = mDeviceList.at( 1 );
	}

	audioDeviceSelected( mDeviceName );

	return( true );
#endif
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
	rebuildDeviceList();

#if defined( PORTAUDIO_SUPPORTED )
	bool		OK;
	QString		NewDev = QInputDialog::getItem( nullptr, mNode->name(), tr( "Select Audio Device" ), mDeviceList, mDeviceList.indexOf( mDeviceName ), false, &OK );

	if( OK && NewDev != mDeviceName )
	{
		audioDeviceSelected( NewDev );
	}
#endif
}

void PortAudioInputNode::rebuildDeviceList()
{
	mDeviceList.clear();

	mDeviceList << tr( "None" );
	mDeviceList << tr( "Default Audio Input" );

#if defined( PORTAUDIO_SUPPORTED )
	mDeviceList << DevicePortAudio::deviceInputNameList();
#endif
}

void PortAudioInputNode::audioDeviceSelected( const QString &pDeviceName )
{
	if( pDeviceName == mDeviceName && mPortAudio )
	{
		return;
	}

	if( mPortAudio )
	{
		mPortAudio.clear();
	}

	mDeviceName = pDeviceName;

	emit audioDeviceChanged( mDeviceName );

#if defined( PORTAUDIO_SUPPORTED )
	const int	DevNamIdx = mDeviceList.indexOf( mDeviceName );

	if( DevNamIdx == 0 )
	{
		mNode->setStatus( fugio::NodeInterface::Initialised );
	}
	else
	{
		PaDeviceIndex		DevIdx;

		if( DevNamIdx == 1 )
		{
			DevIdx = Pa_GetDefaultInputDevice();
		}
		else
		{
			DevIdx = DevicePortAudio::deviceInputNameIndex( mDeviceName );
		}

		if( DevIdx == paNoDevice )
		{
			mNode->setStatus( fugio::NodeInterface::Warning );
		}

		if( ( mPortAudio = DevicePortAudio::newDevice( DevIdx ) ) )
		{
			mNode->setStatus( fugio::NodeInterface::Initialised );
		}
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
	rebuildDeviceList();

	int		Index = pSettings.value( "index", 1 ).toInt();

	QString	DeviceName = pSettings.value( "device", mDeviceName ).toString();

	if( Index >= 0 && Index <= 1 )
	{
		DeviceName = mDeviceList[ Index ];
	}

	audioDeviceSelected( DeviceName );
}

void PortAudioInputNode::saveSettings( QSettings &pSettings ) const
{
	pSettings.setValue( "device", mDeviceName );
	pSettings.setValue( "index", mDeviceList.indexOf( mDeviceName ) );
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

#if defined( PORTAUDIO_SUPPORTED )
	if( AudIns->mDeviceIndex != mDeviceIndex )
	{
		return( false );
	}
#endif

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
