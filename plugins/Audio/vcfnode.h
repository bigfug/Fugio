#ifndef VCFNODE_H
#define VCFNODE_H

#include <QObject>
#include <QLabel>

#include <fugio/nodecontrolbase.h>
#include <fugio/audio/audio_producer_interface.h>

#include "audiofilterdisplay.h"

class VCFNode : public fugio::NodeControlBase, public fugio::AudioProducerInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::AudioProducerInterface )

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "VCF" )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/VCF" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE VCFNode( QSharedPointer<fugio::NodeInterface> pNode);

	virtual ~VCFNode( void ) {}

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

protected slots:
	void cutoffLinked( QSharedPointer<fugio::PinInterface> pPin );
	void cutoffUnlinked( QSharedPointer<fugio::PinInterface> pPin );

	void resonanceLinked( QSharedPointer<fugio::PinInterface> pPin );
	void resonanceUnlinked( QSharedPointer<fugio::PinInterface> pPin );

protected:
	typedef struct AudioChannelData
	{
		float									 y1, y2, y3, y4;
		float									 oldx, oldy1, oldy2, oldy3;
	} AudioChannelData;

	typedef struct AudioInstanceData
	{
		qreal									 mSampleRate;
		fugio::AudioSampleFormat				 mSampleFormat;
		int										 mChannels;

		qint64									 mAudPts;
		qint64									 mAudSmp;
		QVector<QVector<float>>					 mAudDat;
		void									*mAudIns;

		QVector<AudioChannelData>				 mChnDat;

		QVector<float>							 mCutoffData;
		void									*mCutoffInstance;

		QVector<float>							 mResonanceData;
		void									*mResonanceInstance;
	} AudioInstanceData;

	static void updateFilter( const float cutoff, const float res, float &p, float &k, float &r );

protected:
	QSharedPointer<fugio::PinInterface>			 mPinAudioInput;
	QSharedPointer<fugio::PinInterface>			 mPinCutoff;
	QSharedPointer<fugio::PinInterface>			 mPinResonance;

	QSharedPointer<fugio::PinInterface>			 mPinAudioOutput;
	fugio::AudioProducerInterface				*mAudioOutput;

	QMutex										 mInstanceDataMutex;
	QList<AudioInstanceData *>					 mInstanceData;

	float										 mCutoff;
	float										 mResonance;
};


#endif // VCFNODE_H
