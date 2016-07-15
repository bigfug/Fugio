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
	// fugio::AudioProducerInterface

//	virtual void audio( qint64 pSamplePosition, qint64 pSampleCount, int pChannelOffset, int pChannelCount, void **pBuffers, fugio::AudioInstanceBase *pInstanceData ) const Q_DECL_OVERRIDE
//	{
//		if( producer() )
//		{
//			producer()->audio( pSamplePosition, pSampleCount, pChannelOffset, pChannelCount, pBuffers, pInstanceData );
//		}
//	}

	virtual fugio::AudioInstanceBase *audioAllocInstance( qreal pSampleRate, fugio::AudioSampleFormat pSampleFormat, int pChannels ) Q_DECL_OVERRIDE
	{
		return( producer() ? producer()->audioAllocInstance( pSampleRate, pSampleFormat, pChannels ) : nullptr );
	}

	virtual int audioChannels() const Q_DECL_OVERRIDE;
	virtual qreal audioSampleRate() const Q_DECL_OVERRIDE;
	virtual fugio::AudioSampleFormat audioSampleFormat() const Q_DECL_OVERRIDE;
	virtual qint64 audioLatency() const Q_DECL_OVERRIDE;

	virtual bool isValid( fugio::AudioInstanceBase *pInstance ) const Q_DECL_OVERRIDE
	{
		Q_UNUSED( pInstance )

		return( true );
	}

private:
	fugio::AudioProducerInterface *producer( void );

	const fugio::AudioProducerInterface *producer( void ) const;

	fugio::AudioProducerInterface		*mProducer;
};

#endif // AUDIOPIN_H
