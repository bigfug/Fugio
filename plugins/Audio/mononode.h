#ifndef MONONODE_H
#define MONONODE_H

#include <QObject>
#include <QLabel>

#include <fugio/nodecontrolbase.h>
#include <fugio/audio/audio_producer_interface.h>

#include "audiofilterdisplay.h"

class MonoNode : public fugio::NodeControlBase, public fugio::AudioProducerInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::AudioProducerInterface )

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Mixes audio channels to mono (single channel)" )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/Mono_(Audio)" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE MonoNode( QSharedPointer<fugio::NodeInterface> pNode);

	virtual ~MonoNode( void ) {}

	// NodeControlInterface interface
public:
	virtual bool initialise() Q_DECL_OVERRIDE;
	virtual bool deinitialise() Q_DECL_OVERRIDE;

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

	// InterfaceAudioProducer interface
public:
	virtual void audio( qint64 pSamplePosition, qint64 pSampleCount, int pChannelOffset, int pChannelCount, float **pBuffers, qint64 pLatency, void *pInstanceData ) const Q_DECL_OVERRIDE;
	virtual void *allocAudioInstance( qreal pSampleRate, fugio::AudioSampleFormat pSampleFormat, int pChannels ) Q_DECL_OVERRIDE;
	virtual void freeAudioInstance(void *pInstanceData) Q_DECL_OVERRIDE;

protected:
	typedef struct AudioInstanceData
	{
		qreal									 mSampleRate;
		fugio::AudioSampleFormat				 mSampleFormat;
		int										 mChannels;

		void									*mAudIns;
	} AudioInstanceData;

protected:
	QSharedPointer<fugio::PinInterface>			 mPinAudioInput;

	QSharedPointer<fugio::PinInterface>			 mPinAudioOutput;
	fugio::AudioProducerInterface				*mAudioOutput;

	QMutex										 mInstanceDataMutex;
	QList<AudioInstanceData *>					 mInstanceData;
};


#endif // MONONODE_H
