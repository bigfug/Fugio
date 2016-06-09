#ifndef AUDIOMIXERNODE_H
#define AUDIOMIXERNODE_H

#include <QObject>

#include <fugio/node_interface.h>
#include <fugio/node_control_interface.h>
#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>
#include <fugio/audio/audio_producer_interface.h>

#include <fugio/audio/uuid.h>

#include <fugio/nodecontrolbase.h>

class AudioMixerNode : public fugio::NodeControlBase, public fugio::AudioProducerInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::AudioProducerInterface )

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Mixes multiple audio inputs into single output" )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/Audio_Mixer" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit AudioMixerNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~AudioMixerNode( void );

	//-------------------------------------------------------------------------
	// fugio::NoteControlInterface

	virtual QList<QUuid> pinAddTypesInput() const Q_DECL_OVERRIDE;
	virtual bool canAcceptPin(fugio::PinInterface *pPin) const Q_DECL_OVERRIDE;

	//-------------------------------------------------------------------------
	// InterfaceAudioProducer

	virtual void audio( qint64 pSamplePosition, qint64 pSampleCount, int pChannelOffset, int pChannelCount, float **pBuffers, qint64 pLatency, void *pInstanceData ) const Q_DECL_OVERRIDE;

	virtual void *allocAudioInstance( qreal pSampleRate, fugio::AudioSampleFormat pSampleFormat, int pChannels ) Q_DECL_OVERRIDE;
	virtual void freeAudioInstance(void *pInstanceData) Q_DECL_OVERRIDE;

protected slots:
	void pinLinked( QSharedPointer<fugio::PinInterface> pPinSrc, QSharedPointer<fugio::PinInterface> pPinDst );
	void pinUnlinked( QSharedPointer<fugio::PinInterface> pPinSrc, QSharedPointer<fugio::PinInterface> pPinDst );

protected:
	static inline float mix( float f1, float f2, qreal v )
	{
		return( ( f1 * v ) + ( f2 * ( 1.0 - v ) ) );
	}

private:
	QSharedPointer<fugio::PinInterface>			 mPinOutput;
	fugio::AudioProducerInterface				*mValOutput;

	typedef struct AudioInstanceData
	{
		qreal									mSampleRate;
		fugio::AudioSampleFormat				mSampleFormat;
		int										mChannels;

		QMutex											mMutex;
		QMap<fugio::AudioProducerInterface *,void *>	mInstanceData;
	} AudioInstanceData;

	QList<AudioInstanceData *>					 mInstanceData;
};

#endif // AUDIOMIXERNODE_H
