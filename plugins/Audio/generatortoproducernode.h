#ifndef GENERATORTOPRODUCERNODE_H
#define GENERATORTOPRODUCERNODE_H

#include <fugio/nodecontrolbase.h>
#include <fugio/audio/audio_producer_interface.h>
#include <fugio/audio/audio_generator_interface.h>

class GeneratorToProducerNode : public fugio::NodeControlBase, public fugio::AudioProducerInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::AudioProducerInterface )

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "" )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/Generator_To_Producer_(Audio)" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE GeneratorToProducerNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~GeneratorToProducerNode( void ) {}

	// NodeControlInterface interface

//	virtual bool initialise() Q_DECL_OVERRIDE;
//	virtual bool deinitialise() Q_DECL_OVERRIDE;

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

		qint64									 mGeneratorSamplePosition;

		qint64									 mAudPts;
		qint64									 mAudSmp;
		QVector<QVector<float>>					 mAudDat;
		void									*mAudIns;
	} AudioInstanceData;

	static void convert8UIto32FS( const quint8 *pSrcDat, qint64 pSrcCnt, int pSrcChn, float **pDstDat, qint64 pDstOff, int pChannelOffset, int pChannelCount );

protected:
	QSharedPointer<fugio::PinInterface>			 mPinAudioGenerator;

	QSharedPointer<fugio::PinInterface>			 mPinAudioOutput;
	fugio::AudioProducerInterface				*mAudioOutput;

	QMutex										 mInstanceDataMutex;
	QList<AudioInstanceData *>					 mInstanceData;

	fugio::AudioProducerInterface				*mInputProducer;
	void										*mInputInstance;

	int											 mGeneratorSampleRate;
	int											 mGeneratorChannelCount;
	fugio::AudioSampleFormat					 mGeneratorSampleFormat;
};


#endif // GENERATORTOPRODUCERNODE_H
