#ifndef MEDIANODE_H
#define MEDIANODE_H

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

class MediaNode : public fugio::NodeControlBase, public fugio::AudioProducerInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::AudioProducerInterface )

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "FFMPEG audio/video player" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Media_Player_(FFMPEG)" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit MediaNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~MediaNode( void );

	//-------------------------------------------------------------------------
	// NodeControlInterface interface

	virtual bool initialise() Q_DECL_OVERRIDE;
	virtual bool deinitialise() Q_DECL_OVERRIDE;

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
	virtual int audioChannels() const Q_DECL_OVERRIDE;
	virtual qreal audioSampleRate() const Q_DECL_OVERRIDE;
	virtual fugio::AudioSampleFormat audioSampleFormat() const Q_DECL_OVERRIDE;
	virtual qint64 audioLatency() const Q_DECL_OVERRIDE;

	virtual bool isValid( fugio::AudioInstanceBase *pInstance ) const Q_DECL_OVERRIDE
	{
		Q_UNUSED( pInstance )

		return( true );
	}

private slots:
	void onContextFrame( qint64 pTimeStamp );

private:
	bool loadMedia( const QString &pFileName );

	void unloadMedia( void );

	void setVideo( fugio::SegmentInterface *pSegment );

	void updateVideo( qreal pTimeCurr );

private:
	QSharedPointer<fugio::PinInterface>			 mPinTrigger;
	QSharedPointer<fugio::PinInterface>			 mPinFileName;
	QSharedPointer<fugio::PinInterface>			 mPinVolume;
	QSharedPointer<fugio::PinInterface>			 mPinPlay;
	QSharedPointer<fugio::PinInterface>			 mPinPause;
	QSharedPointer<fugio::PinInterface>			 mPinStop;
	QSharedPointer<fugio::PinInterface>			 mPinRewind;
	QSharedPointer<fugio::PinInterface>			 mPinLoopCount;
	QSharedPointer<fugio::PinInterface>			 mPinInputFrameNumber;
	QSharedPointer<fugio::PinInterface>			 mPinInputTime;

	QSharedPointer<fugio::PinInterface>			 mPinImage;
	fugio::VariantInterface						*mValImage;

	QSharedPointer<fugio::PinInterface>			 mPinAudio;
	fugio::AudioProducerInterface				*mValAudio;

	QSharedPointer<fugio::PinInterface>			 mPinLoaded;
	QSharedPointer<fugio::PinInterface>			 mPinPlaying;
	QSharedPointer<fugio::PinInterface>			 mPinPaused;
	QSharedPointer<fugio::PinInterface>			 mPinStopped;
	QSharedPointer<fugio::PinInterface>			 mPinLooped;
	QSharedPointer<fugio::PinInterface>			 mPinLoopNumber;

	fugio::VariantInterface						*mValLoaded;
	fugio::VariantInterface						*mValPlaying;
	fugio::VariantInterface						*mValPaused;
	fugio::VariantInterface						*mValStopped;
	fugio::PinControlInterface					*mValLooped;
	fugio::VariantInterface						*mValLoopNumber;

	fugio::SegmentInterface						*mSegment;

	bool										 mAudioMute;

	qreal										 mAudioVolume;
	qreal										 mVideoAlpha;

	qint64										 mTimeOffset;
	qint64										 mTimePause;
	qint64										 mTimeLast;

	int											 mTargetFrameNumber;
	qreal										 mTargetTime;

	typedef enum MediaState
	{
		STOP, PLAY, PAUSE, REWIND
	} MediaState;

	MediaState									 mCurrMediaState;
	MediaState									 mNextMediaState;

	int											 mLoopCount;

	class AudioInstanceData : public fugio::AudioInstanceBase
	{
	public:
		AudioInstanceData( QSharedPointer<fugio::AudioProducerInterface> pProducer, qreal pSampleRate, fugio::AudioSampleFormat pSampleFormat, int pChannels )
			: fugio::AudioInstanceBase( pProducer, pSampleRate, pSampleFormat, pChannels )
		{
			mSampleOffset = 0;
		}

		virtual ~AudioInstanceData( void )
		{
			QSharedPointer<MediaNode>	API = qSharedPointerCast<MediaNode>( mProducer );

			if( API )
			{
				API->audioRemoveInstance( this );
			}
		}

		virtual void audio( qint64 pSamplePosition, qint64 pSampleCount, int pChannelOffset, int pChannelCount, void **pBuffers ) Q_DECL_OVERRIDE
		{
			QSharedPointer<MediaNode>	API = qSharedPointerCast<MediaNode>( mProducer );

			if( !API )
			{
				return;
			}

			API->audio( pSamplePosition, pSampleCount, pChannelOffset, pChannelCount, pBuffers, this );
		}

	public:
		qint64							 mSampleOffset;
	};

	void audioRemoveInstance( AudioInstanceData *pInstance );

	void audioResetOffset( void );

	QMutex								 mInstanceMutex;
	QList<AudioInstanceData *>			 mInstanceData;

	void audio( qint64 pSamplePosition, qint64 pSampleCount, int pChannelOffset, int pChannelCount, void **pBuffers, AudioInstanceData *pInstanceData ) const;

	QString									mMediaFilename;
	qreal									mImgPts;
};

#endif // MEDIANODE_H
