#ifndef AUDIOFILTERNODE_H
#define AUDIOFILTERNODE_H

#include <QObject>
#include <QLabel>

#include <fugio/nodecontrolbase.h>
#include <fugio/audio/audio_producer_interface.h>

#include "audiofilterdisplay.h"

class AudioFilterNode : public fugio::NodeControlBase, public fugio::AudioProducerInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::AudioProducerInterface )

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Audio filter" )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/Filter_(Audio)" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE AudioFilterNode( QSharedPointer<fugio::NodeInterface> pNode);

	virtual ~AudioFilterNode( void ) {}

	// NodeControlInterface interface

	virtual bool initialise() Q_DECL_OVERRIDE;
	virtual bool deinitialise() Q_DECL_OVERRIDE;

	virtual QWidget *gui() Q_DECL_OVERRIDE;

	virtual void inputsUpdated( qint64 pTimeStamp) Q_DECL_OVERRIDE;

	// InterfaceAudioProducer interface
public:
	virtual void audio( qint64 pSamplePosition, qint64 pSampleCount, int pChannelOffset, int pChannelCount, float **pBuffers, void *pInstanceData ) const Q_DECL_OVERRIDE;
	virtual void *allocAudioInstance( qreal pSampleRate, fugio::AudioSampleFormat pSampleFormat, int pChannels ) Q_DECL_OVERRIDE;
	virtual void freeAudioInstance(void *pInstanceData) Q_DECL_OVERRIDE;

signals:
	void updateDisplay( const QVector<float> &pTaps );

private slots:
	void onContextFrame( qint64 pTimeStamp );

protected:
	typedef struct AudioInstanceData
	{
		qreal									 mSampleRate;
		fugio::AudioSampleFormat				 mSampleFormat;
		int										 mChannels;

		qint64									 mAudPts;
		qint64									 mAudSmp;
		QVector<QVector<float>>					 mAudDat;
		void									*mAudIns;
	} AudioInstanceData;

	void updateTaps( void );

protected:
	QSharedPointer<fugio::PinInterface>			 mPinAudioInput;
	QSharedPointer<fugio::PinInterface>			 mPinFreqLower;
	QSharedPointer<fugio::PinInterface>			 mPinFreqUpper;
	QSharedPointer<fugio::PinInterface>			 mPinTaps;

	QSharedPointer<fugio::PinInterface>			 mPinAudioOutput;
	fugio::AudioProducerInterface				*mAudioOutput;

	QMutex										 mInstanceDataMutex;
	QList<AudioInstanceData *>					 mInstanceData;

	float										 mSampleRate;
	float										 mLowerFrequency;
	float										 mUpperFrequency;
	QVector<float>								 mTaps;

	qint64										 mLastDisplayChange;
	qint64										 mLastDisplayUpdate;
};

#endif // AUDIOFILTERNODE_H
