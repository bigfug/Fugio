#ifndef AUDIOFILTERNODE_H
#define AUDIOFILTERNODE_H

#include <QObject>
#include <QLabel>

#include <fugio/nodecontrolbase.h>
#include <fugio/audio/audio_producer_interface.h>
#include <fugio/audio/audio_instance_base.h>

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
	Q_INVOKABLE AudioFilterNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~AudioFilterNode( void ) {}

	// NodeControlInterface interface

	virtual bool initialise() Q_DECL_OVERRIDE;
	virtual bool deinitialise() Q_DECL_OVERRIDE;

	virtual QWidget *gui() Q_DECL_OVERRIDE;

	virtual void inputsUpdated( qint64 pTimeStamp) Q_DECL_OVERRIDE;

	// InterfaceAudioProducer interface
public:
	virtual fugio::AudioInstanceBase *audioAllocInstance( qreal pSampleRate, fugio::AudioSampleFormat pSampleFormat, int pChannels ) Q_DECL_OVERRIDE;
//	virtual void audioFreeInstance(void *pInstanceData) Q_DECL_OVERRIDE;
	virtual int audioChannels() const Q_DECL_OVERRIDE;
	virtual qreal audioSampleRate() const Q_DECL_OVERRIDE;
	virtual fugio::AudioSampleFormat audioSampleFormat() const Q_DECL_OVERRIDE;
	virtual qint64 audioLatency() const Q_DECL_OVERRIDE;

	virtual bool isValid( fugio::AudioInstanceBase *pInstance ) const Q_DECL_OVERRIDE
	{
		Q_UNUSED( pInstance )

		return( true );
	}

signals:
	void updateDisplay( const QVector<float> &pTaps );

private slots:
	void onContextFrame( qint64 pTimeStamp );

protected:
	class AudioInstanceData : public fugio::AudioInstanceBase
	{
	public:
		AudioInstanceData( QSharedPointer<fugio::AudioProducerInterface> pProducer, qreal pSampleRate, fugio::AudioSampleFormat pSampleFormat, int pChannels )
			: fugio::AudioInstanceBase( pProducer, pSampleRate, pSampleFormat, pChannels )
		{

		}

		virtual ~AudioInstanceData( void ) {}

		virtual void audio( qint64 pSamplePosition, qint64 pSampleCount, int pChannelOffset, int pChannelCount, void **pBuffers ) Q_DECL_OVERRIDE
		{
			if( !isEnabled() )
			{
				return;
			}

			QSharedPointer<AudioFilterNode>	API = qSharedPointerCast<AudioFilterNode>( mProducer );

			if( !API )
			{
				return;
			}

			API->audio( pSamplePosition, pSampleCount, pChannelOffset, pChannelCount, pBuffers, this );
		}

	public:
		qint64									 mAudPts;
		qint64									 mAudSmp;
		QVector<QVector<float>>					 mAudDat;
		fugio::AudioInstanceBase				*mAudIns;
	};

	void audio( qint64 pSamplePosition, qint64 pSampleCount, int pChannelOffset, int pChannelCount, void **pBuffers, AudioInstanceData *pInstanceData ) const;

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
