#ifndef AUDIOGENERATORPIN_H
#define AUDIOGENERATORPIN_H

#include <fugio/audio/uuid.h>
#include <fugio/pincontrolbase.h>
#include <fugio/audio/audio_generator_interface.h>

class AudioGeneratorPin : public fugio::PinControlBase, public fugio::AudioGeneratorInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::AudioGeneratorInterface )

public:
	Q_INVOKABLE explicit AudioGeneratorPin( QSharedPointer<fugio::PinInterface> pPin );

	virtual ~AudioGeneratorPin( void ) {}

	//-------------------------------------------------------------------------
	// fugio::PinControlInterface

	virtual QString toString( void ) const Q_DECL_OVERRIDE
	{
		return( QString( "" ) );
	}

	virtual QString description( void ) const Q_DECL_OVERRIDE
	{
		return( "Audio Generator" );
	}

	//-------------------------------------------------------------------------
	// AudioGeneratorInterface interface
public:
	virtual int audioChannels() const Q_DECL_OVERRIDE;
	virtual qreal audioSampleRate() const Q_DECL_OVERRIDE;
	virtual fugio::AudioSampleFormat audioSampleFormat() const Q_DECL_OVERRIDE;
	virtual bool audioLock(qint64 pSamplePosition, qint64 pSampleCount, const void **pBuffers, qint64 &pReturnedPosition, qint64 &pReturnedCount) Q_DECL_OVERRIDE;
	virtual void audioUnlock(qint64 pSamplePosition, qint64 pSampleCount) Q_DECL_OVERRIDE;

private:
	fugio::AudioGeneratorInterface *generator( void );

	const fugio::AudioGeneratorInterface *generator( void ) const;

private:
	fugio::AudioGeneratorInterface		*mGenerator;
};


#endif // AUDIOGENERATORPIN_H
