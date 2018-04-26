#ifndef NDIRECEIVENODE_H
#define NDIRECEIVENODE_H

#include <QObject>

#include <fugio/nodecontrolbase.h>

#if defined( NDI_SUPPORTED )

#if defined( Q_OS_WIN )
#include <windows.h>
#else
#include <Processing.NDI.compat.h>
#endif

#include <Processing.NDI.Lib.h>
#include <Processing.NDI.Find.h>
#include <Processing.NDI.Recv.h>
#include <Processing.NDI.Send.h>
#endif

#include <fugio/choice_interface.h>
#include <fugio/image/image.h>
#include <fugio/audio/audio_producer_interface.h>
#include <fugio/audio/audio_instance_base.h>

class NDIReceiveNode : public fugio::NodeControlBase, public fugio::AudioProducerInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::AudioProducerInterface )

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "NDI Receiver" )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/NDI_Receive" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE NDIReceiveNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~NDIReceiveNode( void ) {}

	// NodeControlInterface interface
public:
	virtual bool initialise() Q_DECL_OVERRIDE;
	virtual bool deinitialise() Q_DECL_OVERRIDE;

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

	virtual QWidget *gui() Q_DECL_OVERRIDE;

	// AudioProducerInterface interface
public:
	virtual int audioChannels() const Q_DECL_OVERRIDE;
	virtual qreal audioSampleRate() const Q_DECL_OVERRIDE;
	virtual fugio::AudioSampleFormat audioSampleFormat() const Q_DECL_OVERRIDE;
	virtual qint64 audioLatency() const Q_DECL_OVERRIDE;
	virtual fugio::AudioInstanceBase *audioAllocInstance(qreal pSampleRate, fugio::AudioSampleFormat pSampleFormat, int pChannels) Q_DECL_OVERRIDE;
	virtual bool isValid( fugio::AudioInstanceBase *pInstance ) const Q_DECL_OVERRIDE;

protected:
#if defined( NDI_SUPPORTED )
	void processVideoFrame( NDIlib_video_frame_t &pFrame );

	void processAudioFrame( NDIlib_audio_frame_t &pFrame );

	void clearAudio( void );
#endif

protected slots:
	void updateNDISources( void );

	void contextFrameStart( qint64 pTimeStamp );

protected:
#if defined( NDI_SUPPORTED )
	typedef struct AudioFrame
	{
		NDIlib_audio_frame_t	 mFrame;
		float					*mAudio;
		qint64					 mOffset;
	} AudioFrame;
#endif

	class AudioInstanceData : public fugio::AudioInstanceBase
	{
	public:
		AudioInstanceData( QSharedPointer<fugio::AudioProducerInterface> pProducer, qreal pSampleRate, fugio::AudioSampleFormat pSampleFormat, int pChannels )
			: fugio::AudioInstanceBase( pProducer, pSampleRate, pSampleFormat, pChannels )
		{
			mAudioOffset = 0;
			mSamplePosition = 0;
		}

		virtual ~AudioInstanceData( void ) {}

		virtual void audio( qint64 pSamplePosition, qint64 pSampleCount, int pChannelOffset, int pChannelCount, void **pBuffers ) Q_DECL_OVERRIDE
		{
			QSharedPointer<NDIReceiveNode>	API = qSharedPointerDynamicCast<NDIReceiveNode>( mProducer );

			if( !API )
			{
				return;
			}

			API->audio( pSamplePosition, pSampleCount, pChannelOffset, pChannelCount, pBuffers, this );
		}

	public:
		fugio::AudioInstanceBase			*mInstance;
		qint64								 mSamplePosition;
		qint64								 mAudioOffset;
	};

public:
	void audio( qint64 pSamplePosition, qint64 pSampleCount, int pChannelOffset, int pChannelCount, void **pBuffers, AudioInstanceData *pInstanceData ) const;

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInputSource;
	fugio::ChoiceInterface						*mValInputSource;

	QSharedPointer<fugio::PinInterface>			 mPinOutputImage;
	fugio::VariantInterface						*mValOutputImage;

	QSharedPointer<fugio::PinInterface>			 mPinOutputAudio;
	fugio::AudioProducerInterface				*mValOutputAudio;

	QString										 mNDISourceId;

	quint32										 mNDIAudioChannels;
	qreal										 mNDIAudioSampleRate;
	fugio::AudioSampleFormat					 mNDIAudioSampleFormat;

#if defined( NDI_SUPPORTED )
	NDIlib_find_instance_t						 mNDIFind;
	uint32_t									 mNDIFindTimeout;
	NDIlib_recv_instance_t						 mNDIInstance;

	mutable QMutex								 mAudioMutex;
	QList<AudioFrame>							 mAudioFrames;

	qint64										 mAudioOffset;
#endif
};

#endif // NDIRECEIVENODE_H
