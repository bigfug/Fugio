#ifndef AUDIO_INSTANCE_BASE_H
#define AUDIO_INSTANCE_BASE_H

#include <QWeakPointer>

#include <fugio/global.h>

#include <fugio/node_control_interface.h>
#include <fugio/audio/audio_sample_format.h>
#include <fugio/audio/audio_producer_interface.h>

FUGIO_NAMESPACE_BEGIN

class AudioInstanceBase
{
public:
	AudioInstanceBase( QSharedPointer<fugio::AudioProducerInterface> pProducer, qreal pSampleRate, fugio::AudioSampleFormat pSampleFormat, int pChannels )
		: mProducer( pProducer ), mSampleRate( pSampleRate ), mSampleFormat( pSampleFormat ), mChannels( pChannels )
	{

	}

	virtual ~AudioInstanceBase( void ) {}

	inline qreal sampleRate( void ) const
	{
		return( mSampleRate );
	}

	inline fugio::AudioSampleFormat sampleFormat( void ) const
	{
		return( mSampleFormat );
	}

	inline int channels( void ) const
	{
		return( mChannels );
	}

	inline QSharedPointer<fugio::AudioProducerInterface> producer( void ) const
	{
		return( mProducer.toStrongRef() );
	}

	bool isValid( void ) const
	{
		QSharedPointer<fugio::AudioProducerInterface>	API = producer();

		if( !API )
		{
			return( false );
		}

		return( true );
	}

	virtual void audio( qint64 pSamplePosition, qint64 pSampleCount, int pChannelOffset, int pChannelCount, void **pBuffers ) = 0;

protected:
	QWeakPointer<fugio::AudioProducerInterface>			mProducer;
	const qreal											mSampleRate;
	const fugio::AudioSampleFormat						mSampleFormat;
	const int											mChannels;
};

FUGIO_NAMESPACE_END

#endif // AUDIO_INSTANCE_BASE_H
