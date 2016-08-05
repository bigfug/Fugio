#ifndef RINGMODULATORNODE_H
#define RINGMODULATORNODE_H

#include <QObject>
#include <QLabel>

#include <fugio/nodecontrolbase.h>
#include <fugio/audio/audio_producer_interface.h>
#include <fugio/audio/audio_instance_base.h>

class RingModulatorNode : public fugio::NodeControlBase, public fugio::AudioProducerInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::AudioProducerInterface )

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Ring Modulator" )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/Ring_Modulator" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE RingModulatorNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~RingModulatorNode( void ) {}

	// NodeControlInterface interface
public:
	virtual bool initialise() Q_DECL_OVERRIDE;
	virtual bool deinitialise() Q_DECL_OVERRIDE;

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

	// InterfaceAudioProducer interface
public:
	virtual fugio::AudioInstanceBase *audioAllocInstance( qreal pSampleRate, fugio::AudioSampleFormat pSampleFormat, int pChannels ) Q_DECL_OVERRIDE;
	virtual int audioChannels() const Q_DECL_OVERRIDE;
	virtual qreal audioSampleRate() const Q_DECL_OVERRIDE;
	virtual fugio::AudioSampleFormat audioSampleFormat() const Q_DECL_OVERRIDE;
	virtual qint64 audioLatency() const Q_DECL_OVERRIDE;

	virtual bool isValid( fugio::AudioInstanceBase *pInstance ) const Q_DECL_OVERRIDE
	{
		return( pInstance->isValid() );
	}

protected slots:
	void audio1Linked( QSharedPointer<fugio::PinInterface> pPin );
	void audio1Unlinked( QSharedPointer<fugio::PinInterface> pPin );

	void audio2Linked( QSharedPointer<fugio::PinInterface> pPin );
	void audio2Unlinked( QSharedPointer<fugio::PinInterface> pPin );

protected:
	class AudioInstanceData : public fugio::AudioInstanceBase
	{
	public:
		AudioInstanceData( QSharedPointer<fugio::AudioProducerInterface> pProducer, qreal pSampleRate, fugio::AudioSampleFormat pSampleFormat, int pChannels )
			: fugio::AudioInstanceBase( pProducer, pSampleRate, pSampleFormat, pChannels )
		{

		}

		virtual ~AudioInstanceData( void )
		{
			if( mAudioIns1 )
			{
				delete mAudioIns1;

				mAudioIns1 = nullptr;
			}

			if( mAudioIns2 )
			{
				delete mAudioIns2;

				mAudioIns2 = nullptr;
			}
		}

		virtual void audio( qint64 pSamplePosition, qint64 pSampleCount, int pChannelOffset, int pChannelCount, void **pBuffers ) Q_DECL_OVERRIDE
		{
			QSharedPointer<RingModulatorNode>	API = qSharedPointerCast<RingModulatorNode>( mProducer );

			if( !API )
			{
				return;
			}

			API->audio( pSamplePosition, pSampleCount, pChannelOffset, pChannelCount, pBuffers, this );
		}

	public:
		qint64									 mAudPts;
		qint64									 mAudSmp;

		QVector<QVector<float>>					 mAudioDat1;
		fugio::AudioInstanceBase				*mAudioIns1;

		QVector<float>							 mAudioDat2;
		fugio::AudioInstanceBase				*mAudioIns2;
	};

	void audio( qint64 pSamplePosition, qint64 pSampleCount, int pChannelOffset, int pChannelCount, void **pBuffers, AudioInstanceData *pInstanceData ) const;

protected:
	QSharedPointer<fugio::PinInterface>			 mPinAudioInput1;
	QSharedPointer<fugio::PinInterface>			 mPinAudioInput2;

	QSharedPointer<fugio::PinInterface>			 mPinAudioOutput;
	fugio::AudioProducerInterface				*mAudioOutput;

	QMutex										 mInstanceDataMutex;
	QList<AudioInstanceData *>					 mInstanceData;
};

#endif // RINGMODULATORNODE_H
