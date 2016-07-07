#ifndef AUDIOPIN_H
#define AUDIOPIN_H

#include <fugio/audio/uuid.h>
#include <fugio/pincontrolbase.h>
#include <fugio/audio/audio_producer_interface.h>

class AudioPin : public fugio::PinControlBase, public fugio::AudioProducerInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::AudioProducerInterface )

public:
	Q_INVOKABLE explicit AudioPin( QSharedPointer<fugio::PinInterface> pPin );

	virtual ~AudioPin( void );

	//-------------------------------------------------------------------------
	// fugio::PinControlInterface

	virtual QString toString( void ) const Q_DECL_OVERRIDE
	{
		return( QString( "" ) );
	}

	virtual QString description( void ) const Q_DECL_OVERRIDE
	{
		return( "Audio" );
	}

	//-------------------------------------------------------------------------
	// fugio::PinControlInterface

	virtual void audio( qint64 pSamplePosition, qint64 pSampleCount, int pChannelOffset, int pChannelCount, float **pBuffers, qint64 pLatency, void *pInstanceData ) const Q_DECL_OVERRIDE
	{
		if( producer() )
		{
			producer()->audio( pSamplePosition, pSampleCount, pChannelOffset, pChannelCount, pBuffers, pLatency, pInstanceData );
		}
	}

	virtual void *allocAudioInstance( qreal pSampleRate, fugio::AudioSampleFormat pSampleFormat, int pChannels ) Q_DECL_OVERRIDE
	{
		return( producer() ? producer()->allocAudioInstance( pSampleRate, pSampleFormat, pChannels ) : nullptr );
	}

	virtual void freeAudioInstance( void *pInstanceData ) Q_DECL_OVERRIDE
	{
		if( producer() )
		{
			producer()->freeAudioInstance( pInstanceData );
		}
	}

private:
	fugio::AudioProducerInterface *producer( void );

	const fugio::AudioProducerInterface *producer( void ) const;

	fugio::AudioProducerInterface		*mProducer;
};

#endif // AUDIOPIN_H
