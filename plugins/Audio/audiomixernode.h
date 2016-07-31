#ifndef AUDIOMIXERNODE_H
#define AUDIOMIXERNODE_H

#include <QObject>

#include <fugio/node_interface.h>
#include <fugio/node_control_interface.h>
#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>
#include <fugio/audio/audio_producer_interface.h>

#include <fugio/audio/uuid.h>
#include <fugio/audio/audio_instance_base.h>

#include <fugio/nodecontrolbase.h>

class AudioMixerNode : public fugio::NodeControlBase, public fugio::AudioProducerInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::AudioProducerInterface )

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Mixes multiple audio inputs into single output" )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/Mixer_(Audio)" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit AudioMixerNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~AudioMixerNode( void ) {}

	//-------------------------------------------------------------------------
	// fugio::NoteControlInterface

	virtual QList<QUuid> pinAddTypesInput() const Q_DECL_OVERRIDE;
	virtual bool canAcceptPin(fugio::PinInterface *pPin) const Q_DECL_OVERRIDE;
	virtual bool pinShouldAutoRename(fugio::PinInterface *pPin) const Q_DECL_OVERRIDE;

	//-------------------------------------------------------------------------
	// InterfaceAudioProducer

	virtual fugio::AudioInstanceBase *audioAllocInstance( qreal pSampleRate, fugio::AudioSampleFormat pSampleFormat, int pChannels ) Q_DECL_OVERRIDE;
	//virtual void audioFreeInstance(void *pInstanceData) Q_DECL_OVERRIDE;
	virtual int audioChannels() const Q_DECL_OVERRIDE;
	virtual qreal audioSampleRate() const Q_DECL_OVERRIDE;
	virtual fugio::AudioSampleFormat audioSampleFormat() const Q_DECL_OVERRIDE;
	virtual qint64 audioLatency() const Q_DECL_OVERRIDE;

	virtual bool isValid( fugio::AudioInstanceBase *pInstance ) const Q_DECL_OVERRIDE
	{
		Q_UNUSED( pInstance )

		return( true );
	}

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

	class AudioInstanceData : public fugio::AudioInstanceBase
	{
	public:
		AudioInstanceData( QSharedPointer<fugio::AudioProducerInterface> pProducer, qreal pSampleRate, fugio::AudioSampleFormat pSampleFormat, int pChannels )
			: fugio::AudioInstanceBase( pProducer, pSampleRate, pSampleFormat, pChannels )
		{

		}

		virtual ~AudioInstanceData( void )
		{
			mMutex.lock();

			for( auto it = mInstanceData.begin() ; it != mInstanceData.end() ; it++ )
			{
				delete it.value();
			}

			mInstanceData.clear();

			mMutex.unlock();
		}

		virtual void audio( qint64 pSamplePosition, qint64 pSampleCount, int pChannelOffset, int pChannelCount, void **pBuffers ) Q_DECL_OVERRIDE
		{
			QSharedPointer<AudioMixerNode>	API = qSharedPointerCast<AudioMixerNode>( mProducer );

			if( !API )
			{
				return;
			}

			API->audio( pSamplePosition, pSampleCount, pChannelOffset, pChannelCount, pBuffers, this );
		}

	public:
		mutable QMutex														mMutex;
		QMap<fugio::AudioProducerInterface *,fugio::AudioInstanceBase *>	mInstanceData;
	};

	void audio( qint64 pSamplePosition, qint64 pSampleCount, int pChannelOffset, int pChannelCount, void **pBuffers, const AudioInstanceData *pInstanceData ) const;

	QList<AudioInstanceData *>					 mInstanceData;
};

#endif // AUDIOMIXERNODE_H
