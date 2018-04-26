#ifndef MEDIAPROCESSORNODE_H
#define MEDIAPROCESSORNODE_H

#include <QObject>
#include <QImage>
#include <QThread>

#include <qmath.h>

#include <fugio/node_interface.h>
#include <fugio/node_control_interface.h>
#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>
#include <fugio/audio/audio_instance_base.h>

#include <fugio/ffmpeg/uuid.h>

#include <fugio/audio/audio_producer_interface.h>

#include <fugio/nodecontrolbase.h>

#include "mediaaudioprocessor.h"

FUGIO_NAMESPACE_BEGIN
class SegmentInterface;
FUGIO_NAMESPACE_END

class QPushButton;

#include "processoraudiobuffer.h"

class MediaProcessorNode : public fugio::NodeControlBase, public fugio::AudioProducerInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::AudioProducerInterface )

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "FFMPEG audio/video player" )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/Media_Processor" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit MediaProcessorNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~MediaProcessorNode( void );

	//-------------------------------------------------------------------------
	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

	//-------------------------------------------------------------------------
	// fugio::AudioProducerInterface

	virtual void setSource( fugio::AudioProducerInterface * )
	{
	}

	virtual bool hasSource() const
	{
		return( false );
	}

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

private:
	bool loadMedia( const QString &pFileName );

	void unloadMedia( void );

	void setVideo( fugio::SegmentInterface *pSegment );

private:
	QSharedPointer<fugio::PinInterface>			 mPinFileName;
	QSharedPointer<fugio::PinInterface>			 mPinTrigger;
	QSharedPointer<fugio::PinInterface>			 mPinRewind;

	QSharedPointer<fugio::PinInterface>			 mPinImage;
	fugio::VariantInterface						*mValImage;

	QSharedPointer<fugio::PinInterface>			 mPinAudio;
	fugio::AudioProducerInterface				*mValAudio;

	fugio::SegmentInterface							*mSegment;

	volatile qint64								 mSampleOffset;

	qint64										 mTimeOffset;
	qint64										 mTimePause;
	qint64										 mTimeLast;

	qreal										 mLstPts;

	ProcessorAudioBuffer								 AB;

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
			QSharedPointer<MediaProcessorNode>	API = qSharedPointerCast<MediaProcessorNode>( mProducer );

			if( !API )
			{
				return;
			}

			API->audio( pSamplePosition, pSampleCount, pChannelOffset, pChannelCount, pBuffers, this );
		}

	public:
	};

	void audio( qint64 pSamplePosition, qint64 pSampleCount, int pChannelOffset, int pChannelCount, void **pBuffers, AudioInstanceData *pInstanceData ) const;

	QString										mMediaFilename;
};


#endif // MEDIAPROCESSORNODE_H
