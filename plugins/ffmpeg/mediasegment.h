#ifndef MEDIASEGMENT_H
#define MEDIASEGMENT_H

#include <QString>
#include <QVector>
#include <QPair>
#include <QMap>
#include <QMutex>
#include <QMutexLocker>
#include <QSharedPointer>
#include <QThread>
#include <QDateTime>
#include <QDebug>

#include "segmentinterface.h"
#include "audiobuffer.h"

#if defined( FFMPEG_SUPPORTED )
#include "hap/source/hap.h"
#endif

#ifndef INT64_C
#define INT64_C(c) (c ## LL)
#define UINT64_C(c) (c ## ULL)
#endif

#if defined( FFMPEG_SUPPORTED )
extern "C"
{
#include <libavutil/rational.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/opt.h>
#include <libswscale/swscale.h>
#include <libavutil/pixdesc.h>
#include <libavutil/imgutils.h>
#if defined( TL_USE_LIB_AV )
#include <libavutil/samplefmt.h>
#include <libavutil/audioconvert.h>
#else
#include <libswresample/swresample.h>
#endif
}

#if LIBAVFORMAT_VERSION_MAJOR <= 56
#undef FFMPEG_SUPPORTED
#endif

#endif

class QImage;
class MediaAudioProcessor;

class MediaSegment : public QObject, public fugio::SegmentInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::SegmentInterface )

public:
	MediaSegment( void );

	virtual ~MediaSegment( void );

	virtual bool loadMedia( const QString &pFileName, bool pProcess ) Q_DECL_OVERRIDE;

	virtual void setPlayhead( qreal pTimeStamp ) Q_DECL_OVERRIDE;

	virtual void mixAudio( qint64 pSamplePosition, qint64 pSampleCount, int pChannelOffset, int pChannelCount, void **pBuffers, float pVol ) const Q_DECL_OVERRIDE;

	virtual qreal videoFrameTimeStamp( void ) const Q_DECL_OVERRIDE
	{
#if defined( FFMPEG_SUPPORTED )
		qreal	pts = mVideo.mCurrIdx != -1 ? mVidDat[ mVideo.mCurrIdx ].mPTS : mVideo.mPTS;

		return( pts );
#else
		return( 0 );
#endif
	}

	virtual qreal duration( void ) const Q_DECL_OVERRIDE
	{
#if defined( FFMPEG_SUPPORTED )
		return( mDuration );
#else
		return( 0 );
#endif
	}

	virtual qreal wavL( qreal pTimeStamp ) const Q_DECL_OVERRIDE;
	virtual qreal wavR( qreal pTimeStamp ) const Q_DECL_OVERRIDE;

	virtual QString filename( void ) const Q_DECL_OVERRIDE
	{
		return( mFileName );
	}

	virtual bool hasVideo( void ) const Q_DECL_OVERRIDE
	{
#if defined( FFMPEG_SUPPORTED )
		return( mVideo.mStreamId != -1 );
#else
		return( false );
#endif
	}

	virtual bool hasAudio( void ) const Q_DECL_OVERRIDE
	{
#if defined( FFMPEG_SUPPORTED )
		return( mAudio.mStreamId != -1 );
#else
		return( false );
#endif
	}

	virtual QSize imageSize( void ) const Q_DECL_OVERRIDE;

	virtual void setPreload( bool pPreload ) Q_DECL_OVERRIDE;

	virtual bool preload( void ) const Q_DECL_OVERRIDE;

	virtual const VidDat *viddat( void ) const Q_DECL_OVERRIDE;

	virtual int imageFormat( void ) const Q_DECL_OVERRIDE;

	virtual bool imageIsHap( void ) const Q_DECL_OVERRIDE;

	virtual qreal videoFrameRate( void ) const Q_DECL_OVERRIDE;

	virtual void readNext( void ) Q_DECL_OVERRIDE;

	virtual int audioChannels( void ) const Q_DECL_OVERRIDE
	{
#if defined( FFMPEG_SUPPORTED )
		return( mChannels );
#else
		return( 0 );
#endif
	}

	virtual qreal sampleRate( void ) const Q_DECL_OVERRIDE
	{
#if defined( FFMPEG_SUPPORTED )
		return( mSampleRate );
#else
		return( 0 );
#endif
	}

	virtual QString statusMessage( void ) const Q_DECL_OVERRIDE
	{
		return( mStatusMessage );
	}

private:
	bool readVideoFrame( qreal TargetPTS, bool pForce );

	bool readAudioFrame( qreal TargetPTS , bool pForce );

	bool readSubtitleFrame( qreal TargetPTS , bool pForce );

#if defined( FFMPEG_SUPPORTED )
	static int is_realtime( AVFormatContext *s )
	{
		if( !strcmp(s->iformat->name, "rtp")
				|| !strcmp(s->iformat->name, "rtsp")
				|| !strcmp(s->iformat->name, "sdp")
				)
			return 1;

		if(s->pb && (   !strncmp(s->filename, "rtp:", 4)
						|| !strncmp(s->filename, "udp:", 4)
						|| !strncmp(s->filename, "http:", 5)
						|| !strncmp(s->filename, "https:", 6)
						)
				)
			return 1;
		return 0;
	}
#endif

	void updateVideoFrames( qreal pPTS );

	bool haveVideoFrames( void ) const;

signals:
	void durationUpdated( qreal pDuration );

	void operate( const QString & );

protected:
	void clearSegment();

private:
	void unloadMedia( void );

	void processAudio();

	void decodeIFrames( bool pDecode );
	void decodeBFrames( bool pDecode );

	void clearVideo( void );

	void clearAudio( void );

	void processVideoFrame( qreal TargetPTS, qreal PacketTS, bool pForce );

	void processAudioFrame( qreal TargetPTS, qreal PacketTS, bool pForce );

	void processSubtitleFrame( qreal TargetPTS, qreal PacketTS, bool pForce );

	void removeVideoFrames( void );

	void removeAudioFrames( void );

	static QString av_err( const QString &pHeader, int pErrorCode );

	bool hapProcess( qreal TargetPTS, qreal PacketTS , bool pForce );

#if defined( FFMPEG_SUPPORTED )
	static void hapStaticDecodeCallback( HapDecodeWorkFunction function, void *p, unsigned int count, void *info );

	void hapDecodeCallback( HapDecodeWorkFunction function, void *p, unsigned int count );
#endif

	int findFrameIndex(qreal pTimeStamp) const;

	bool isVideoPicture() const;

	bool hasVideoFrame( qreal pPTS ) const;

	bool ptsInRange( qreal pPTS, qreal pTarget, qreal Min, qreal Max) const;

	void audioRange( qint64 &pMinPTS, qint64 &pMaxPTS ) const;
	void videoRange( qreal &pMinPTS, qreal &pMaxPTS ) const;

private:
	class timeout_handler
	{
	public:
		timeout_handler(unsigned int t = 10 ) : timeout_ms_(t){}

		void reset(unsigned int TimeoutMs )
		{
			timeout_ms_ = TimeoutMs;
			lastTime_ = QDateTime::currentMSecsSinceEpoch();
		}

		bool is_timeout(){
			const qint64 actualDelay = QDateTime::currentMSecsSinceEpoch() - lastTime_;
			return actualDelay > timeout_ms_;
		}

		static int check_interrupt(void * t) {
			return t && static_cast<timeout_handler *>(t)->is_timeout();
		}

	public:
		unsigned int timeout_ms_;
		qint64 lastTime_;
	};

private:
	QString				 mFileName;

#if defined( FFMPEG_SUPPORTED )
	AVFormatContext		*mFormatContext;

	typedef struct Stream
	{
		AVStream		*mStream;
		int				 mStreamId;
		AVCodecContext	*mCodecContext;
		AVCodec			*mCodec;
		AVFrame			*mFrame;
		double 			 mPTS;
		double			 mMaxPTS, mMaxDur;
		AVRational		 mFrameRate;

		Stream( void )
			: mStream( nullptr ), mStreamId( -1 ), mCodecContext( nullptr ), mCodec( nullptr ), mFrame( 0 ), mPTS( -1 ),
			  mMaxPTS( 0 ), mMaxDur( 0 )
		{

		}

		double duration( void ) const
		{
			return( mMaxDur );
		}

		bool open( AVStream *pStream, AVDictionary **pOpts )
		{	
			mFrame = av_frame_alloc();

			if( !mFrame )
			{
				return( false );
			}

			mCodec = avcodec_find_decoder( pStream->codecpar->codec_id );

			if( !mCodec )
			{
				qWarning() << "Couldn't avcodec_find_decoder for stream";

				close();

				return( true );
			}

			mCodecContext = avcodec_alloc_context3( NULL );

			if( !mCodecContext )
			{
				return( false );
			}

			avcodec_parameters_to_context( mCodecContext, pStream->codecpar );

			av_codec_set_pkt_timebase( mCodecContext, pStream->time_base );

			mCodecContext->framerate = pStream->avg_frame_rate;

			mFrameRate = mCodecContext->framerate;

			if( avcodec_open2( mCodecContext, mCodec, pOpts ) < 0 )
			{
				qWarning() << "Couldn't avcodec_open2 for stream";

				close();

				return( true );
			}

			mStream = pStream;

			if( mStream->duration != AV_NOPTS_VALUE )
			{
				mMaxDur = qreal( mStream->duration ) * av_q2d( mStream->time_base );
			}

			return( true );
		}

		void close( void )
		{
			avcodec_free_context( &mCodecContext );

			mStreamId = -1;

			mStream = nullptr;
			mCodec = nullptr;

			mPTS = -1;

			mMaxPTS = mMaxDur = 0;

			av_frame_free( &mFrame );
		}
	} Stream;

	typedef struct VideoStream : public Stream
	{
		int			mPrevIdx;
		int			mCurrIdx;
		int			mNxt1Idx;
		int			mNxt2Idx;

		VideoStream( void )
			: mPrevIdx( -1 ), mCurrIdx( -1 ), mNxt1Idx( -1 ), mNxt2Idx( -1 )
		{

		}

	} VideoStream;

	typedef struct AudioStream : public Stream
	{
		qreal				 mSamplePTS;

		AudioStream( void )
			: mSamplePTS( -1 )
		{

		}

	} AudioStream;


	typedef struct SubtitleStream : public Stream
	{
		SubtitleStream( void )
		{

		}

	} SubtitleStream;

	qreal				 mPlayHead;
	VideoStream			 mVideo;
	AudioStream			 mAudio;
	SubtitleStream		 mSubtitle;

	AVPacket			 mPacket;

	int					 mVideoSendResult;
	int					 mVideoRecvResult;
	int					 mAudioSendResult;
	int					 mAudioRecvResult;
	int					 mSubtitleSendResult;
	int					 mSubtitleRecvResult;

	timeout_handler		 mTimeoutHandler;

#if defined( TL_USE_LIB_AV )
#else
	SwrContext			*mSwrContext;
#endif

	QList<VidDat>		 mVidDat;

	bool				 mErrorState;

	qreal				 mDuration;

	mutable QMutex		 mAudDatLck;

	QList<AudioBuffer>	 mAudDat;

	bool				 mDecodeI;
	bool				 mDecodeB;

	uint64_t			 mChannelLayout;
	int					 mChannels;
	int					 mSampleRate;
	enum AVSampleFormat	 mSampleFmt;

	MediaAudioProcessor	*mAudioProcessor;
#endif

	QString				 mStatusMessage;
};

#endif // MEDIASEGMENT_H
