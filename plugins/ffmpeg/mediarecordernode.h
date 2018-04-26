#ifndef MEDIARECORDERNODE_H
#define MEDIARECORDERNODE_H

#include <QObject>
#include <QVector>

#if defined( FFMPEG_SUPPORTED )
#ifndef INT64_C
#define INT64_C(c) (c ## LL)
#define UINT64_C(c) (c ## ULL)
#endif

extern "C"
{
	#include <libavutil/rational.h>
	#include <libavcodec/avcodec.h>
	#include <libavformat/avformat.h>
	#include <libavutil/opt.h>
	#include <libswscale/swscale.h>
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

#include <fugio/node_interface.h>
#include <fugio/node_control_interface.h>
#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>
#include <fugio/file/filename_interface.h>

#include <fugio/ffmpeg/uuid.h>

#include <fugio/nodecontrolbase.h>

FUGIO_NAMESPACE_BEGIN
class AudioInstanceBase;
FUGIO_NAMESPACE_END

class MediaRecorderForm;
class QDockWidget;
class MediaPresetInterface;

class MediaRecorderNode : public fugio::NodeControlBase
{
	Q_OBJECT

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "FFMPEG audio/video recorder" )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/Media_Recorder" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit MediaRecorderNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~MediaRecorderNode( void )
	{

	}

	// NodeControlInterface interface
public:
	virtual QWidget *gui( void ) Q_DECL_OVERRIDE;

	virtual void loadSettings( QSettings &pSettings ) Q_DECL_OVERRIDE;

	virtual void saveSettings( QSettings &pSettings ) const Q_DECL_OVERRIDE;

	virtual bool initialise() Q_DECL_OVERRIDE;
	virtual bool deinitialise() Q_DECL_OVERRIDE;

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

	//-------------------------------------------------------------------------

	void setMediaPreset( MediaPresetInterface *pMediaPreset );

	void setTimeStart( qreal pTimeStamp )
	{
		mTimeStart = pTimeStamp;
	}

	void setTimeDuration( qreal pTimeStamp )
	{
		mTimeDuration = pTimeStamp;
	}

	void setTimeEnd( qreal pTimeEnd )
	{
		mTimeEnd = pTimeEnd;
	}

	typedef enum FrameScale
	{
		FRAME_STRETCH,
		FRAME_FILL,
		FRAME_FIT
	} FrameScale;

	void setFrameScale( FrameScale pFrameScale )
	{
		mFrameScale = pFrameScale;
	}

	FrameScale frameScale( void ) const
	{
		return( mFrameScale );
	}

	qreal quality( void ) const
	{
		return( mQuality );
	}

	qreal speed( void ) const
	{
		return( mSpeed );
	}

	QString mediaPresetName( void ) const
	{
		return( mMediaPresetName );
	}

	qreal timeStart( void ) const
	{
		return( mTimeStart );
	}

	qreal timeDuration( void ) const
	{
		return( mTimeDuration );
	}

	qreal timeEnd( void ) const
	{
		return( mTimeEnd );
	}

signals:
	void recordingStarted( void );
	void recording( qreal pTimeRecorded );
	void recordingStopped( void );

public slots:
	void record( const QString &pFileName );
	void cancel( void );

	void setQuality( qreal pValue )
	{
		mQuality = pValue;
	}

	void setSpeed( qreal pValue )
	{
		mSpeed = pValue;
	}

	void setMediaPresetName( QString pPresetName )
	{
		mMediaPresetName = pPresetName;
	}

private slots:
	void recordEntry( void );

	void frameStart( qint64 pTimeStamp );
	void frameEnd( qint64 pTimeStamp );

	void onFormClicked( void );

private:
	static QImage cropImage( const QImage &pImage, const QSize &pSize );

	bool imageToFrame( void );

private:
	QSharedPointer<fugio::PinInterface>		 mPinInputFilename;
	QSharedPointer<fugio::PinInterface>		 mPinInputImage;
	QSharedPointer<fugio::PinInterface>		 mPinInputAudio;
	QSharedPointer<fugio::PinInterface>		 mPinInputStartTime;
	QSharedPointer<fugio::PinInterface>		 mPinInputDuration;
	QSharedPointer<fugio::PinInterface>		 mPinInputRecord;

	QSharedPointer<fugio::PinInterface>		 mPinOutputFilename;
	QSharedPointer<fugio::PinInterface>		 mPinOutputImageSize;
	QSharedPointer<fugio::PinInterface>		 mPinOutputStarted;
	QSharedPointer<fugio::PinInterface>		 mPinOutputFinished;

	fugio::FilenameInterface				*mValOutputFilename;
	fugio::VariantInterface					*mValOutputImageSize;

	QDockWidget					*mDockWidget;
	MediaRecorderForm			*mDockForm;

	Qt::DockWidgetArea			 mDockArea;

	QString						 mFilename;
	//IOInputImage				*mInputImage;
	qreal						 mTimePrev;
	qreal						 mTimeCurr;

#if defined( FFMPEG_SUPPORTED )
	AVFormatContext				*mFormatContext;

	AVDictionary				*mOptionsVideo;
	AVDictionary				*mOptionsAudio;
	AVCodec						*mCodecVideo;
	AVCodec						*mCodecAudio;
	AVCodecContext				*mCodecContextVideo;
	AVCodecContext				*mCodecContextAudio;
	AVStream					*mStreamVideo;
	AVStream					*mStreamAudio;

	AVOutputFormat				*mOutputFormat;

	AVPacket					 mPacketVideo;
	AVPacket					 mPacketAudio;

	AVFrame						*mFrameVideo;
	AVFrame						*mFrameAudio;
#endif
	//QVector<uint8_t>			 mBufferVideo;
	int64_t						 mFrameCount;
	qint64						 mAudioOffsetInput;
	qint64						 mAudioOffsetOutput;
	qint64						 mFrameCountAudio;

#if defined( FFMPEG_SUPPORTED )
	SwsContext					*mScaleContext;

#if defined( TL_USE_LIB_AV )
#else
	SwrContext					*mSwrContext;
#endif
#endif

	QVector<QVector<float>>		 mBufferAudio;
	QVector<QByteArray>			 mBufferAudioOutput;

	MediaPresetInterface		*mMediaPreset;

	qreal						 mTimeStart;
	qreal						 mTimeDuration;
	qreal						 mTimeEnd;

	FrameScale					 mFrameScale;

	bool						 mCancel;

	qreal						 mQuality;
	qreal						 mSpeed;

	QString						 mMediaPresetName;

	fugio::AudioInstanceBase	*mAudioInstance;
};

#endif // MEDIARECORDERNODE_H
