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
		disconnect( mPortAudio.data(), SIGNAL(audio(const float**,quint64,int,qint64)), this, SLOT(audioInput(const float**,quint64,int,qint64)) );

		mPortAudio.clear();
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
		connect( mPortAudio.data(), SIGNAL(audio(const float**,quint64,int,qint64)), this, SLOT(audioInput(const float**,quint64,int,qint64)) );
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
		disconnect( mPortAudio.data(), SIGNAL(audio(const float**,quint64,int,qint64)), this, SLOT(audioInput(const float**,quint64,int,qint64)) );

		mPortAudio.clear();
	}

	return( NodeControlBase::deinitialise() );
}

void PortAudioInputNode::audio( qint64 pSamplePosition, qint64 pSampleCount, int pChannelOffset, int pChannelCount, float **pBuffers, qint64 pLatency, void *pInstanceData ) const
{
	AudioInstanceData		*AID = static_cast<AudioInstanceData *>( pInstanceData );

	Q_UNUSED( pLatency )

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

		for( int i = 0 ; i < AID->mChannels ; i++ )
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

void PortAudioInputNode::audioInput( PortAudioInputNode::AudioBuffer &AB, const float **pData, quint64 pSampleCount, int pChannelCount, qint64 pSamplePosition )
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

void PortAudioInputNode::audioInput( const float **pData, quint64 pSampleCount, int pChannelCount, qint64 pSamplePosition )
{
	for( AudioBuffer &AB : mAudioBuffers )
	{
		quint64		TimeEnd = AB.mPosition + AB.mSamples;

		if( pSamplePosition - TimeEnd > 5 * mPortAudio->outputSampleRate() )
		{
			audioInput( AB, pData, pSampleCount, pChannelCount, pSamplePosition );

			return;
		}
	}

	AudioBuffer		AB;

	memset( &AB, 0, sizeof( AB ) );

	audioInput( AB, pData, pSampleCount, pChannelCount, pSamplePosition );

	mAudioBuffers.append( AB );
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
		disconnect( mPortAudio.data(), SIGNAL(audio(const float**,quint64,int,qint64)), this, SLOT(audioInput(const float**,quint64,int,qint64)) );

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
		connect( mPortAudio.data(), SIGNAL(audio(const float**,quint64,int,qint64)), this, SLOT(audioInput(const float**,quint64,int,qint64)) );
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
