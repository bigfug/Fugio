#include "medianode.h"

#include <qmath.h>

#include <QSettings>
#include <QDebug>
#include <QFileInfo>
#include <QProgressDialog>
#include <QApplication>
#include <QMainWindow>
#include <QDateTime>
#include <QPainter>
#include <QUrl>
#include <QLabel>
#include <QPushButton>

#include <fugio/core/uuid.h>
#include <fugio/audio/uuid.h>
#include <fugio/image/uuid.h>
#include <fugio/context_signals.h>
#include <fugio/performance.h>

#include <fugio/global_interface.h>
#include <fugio/context_interface.h>
#include <fugio/image/image.h>
#include <fugio/core/variant_interface.h>
#include <fugio/file/filename_interface.h>
#include <fugio/audio/audio_producer_interface.h>

#include "mediasegment.h"
#include "mediaaudioprocessor.h"

#if defined( FFMPEG_SUPPORTED )
#include <hap/source/hap.h>
#endif

MediaNode::MediaNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mSegment( 0 ), mAudioMute( false ), mAudioVolume( 1 ),
	  mTimeOffset( 0 ), mTimePause( 0 ), mTimeLast( -1 ), mTargetFrameNumber( -1 ), mTargetTime( -1 ),
	  mCurrMediaState( STOP ), mNextMediaState( STOP )
{
	const static QUuid	PIN_FILENAME	= QUuid( "{43d2824f-7967-4b22-8b0f-c51358b65d17}" );
	const static QUuid	PIN_VOLUME		= QUuid( "{a7cdb70d-baab-4366-ab60-99ea50bf5e45}" );
	const static QUuid	PIN_PLAY		= QUuid( "{87fedf7e-6eab-4e0b-8ec0-31533c61fe84}" );
	const static QUuid	PIN_PAUSE		= QUuid( "{f4417658-4a43-4ac0-91bd-3e95c28acf24}" );
	const static QUuid	PIN_STOP		= QUuid( "{cf5c9adf-b127-4961-a46b-b0f3e89b5b15}" );
	const static QUuid	PIN_REWIND		= QUuid( "{03b392df-1847-4d36-a261-618f72e2eb17}" );
	const static QUuid	PIN_LOOP_COUNT	= QUuid( "{476c9cec-3d4e-4c89-b60d-6cff6e0ab575}" );
	const static QUuid	PIN_FRAME_NUM	= QUuid( "{7A794317-06F5-4B26-A9D3-095F85EB8DA1}" );
	const static QUuid	PIN_FRAME_TIME	= QUuid( "{F02BA11D-EFE9-41D7-9F6D-4236971C7A5D}" );

	const static QUuid	PIN_IMAGE		= QUuid( "{e0a3e13b-6669-4793-8eb0-e9a12afb0f6b}" );
	const static QUuid	PIN_AUDIO		= QUuid( "{864cae6d-87a4-4f26-8f64-fd0185dad2cf}" );
	const static QUuid	PIN_LOADED		= QUuid( "{b80704bd-000e-4871-9a69-2580e75643e9}" );
	const static QUuid	PIN_PLAYING		= QUuid( "{8cc6495d-f647-4a49-bd33-5b4ed0da869e}" );
	const static QUuid	PIN_PAUSED		= QUuid( "{01029511-b023-4c03-ae6b-8561b7887d78}" );
	const static QUuid	PIN_STOPPED		= QUuid( "{73efbd84-e0c1-483d-aed1-4521c0ac78f1}" );
	const static QUuid	PIN_LOOPED		= QUuid( "{920d9df7-869c-4df1-aff6-991032b32ae2}" );
	const static QUuid	PIN_LOOP_NUMBER	= QUuid( "{6ca2277a-9ca4-4ba5-ad47-87635e4f1db2}" );

	mPinTrigger = pinInput( "Trigger", PID_FUGIO_NODE_TRIGGER );

	mPinFileName = pinInput( "Filename", PIN_FILENAME );

	mPinVolume = pinInput( "Volume", PIN_VOLUME );

	mPinPlay   = pinInput( "Play",   PIN_PLAY );
	mPinPause  = pinInput( "Pause",  PIN_PAUSE );
	mPinStop   = pinInput( "Stop",   PIN_STOP );
	mPinRewind = pinInput( "Rewind", PIN_REWIND );

	mPinLoopCount = pinInput( "Loop Count",	PIN_LOOP_COUNT );

	mPinInputFrameNumber = pinInput( "Frame #", PIN_FRAME_NUM );

	mPinInputTime = pinInput( "Time", PIN_FRAME_TIME );

	mValImage = pinOutput<fugio::VariantInterface *>( "Image", mPinImage, PID_IMAGE, PIN_IMAGE );

	mValAudio = pinOutput<fugio::AudioProducerInterface *>( "Audio", mPinAudio, PID_AUDIO, PIN_AUDIO );

	mValLoaded  = pinOutput<fugio::VariantInterface *>( "Loaded",  mPinLoaded,  PID_BOOL, PIN_LOADED );
	mValPlaying = pinOutput<fugio::VariantInterface *>( "Playing", mPinPlaying, PID_BOOL, PIN_PLAYING );
	mValPaused  = pinOutput<fugio::VariantInterface *>( "Paused",  mPinPaused,  PID_BOOL, PIN_PAUSED );
	mValStopped = pinOutput<fugio::VariantInterface *>( "Stopped", mPinStopped, PID_BOOL, PIN_STOPPED );

	mValLooped		= pinOutput<fugio::PinControlInterface *>( "Looped", mPinLooped, PID_TRIGGER, PIN_LOOPED );
	mValLoopNumber	= pinOutput<fugio::VariantInterface *>( "Loop Number", mPinLoopNumber, PID_INTEGER, PIN_LOOP_NUMBER );

	mPinVolume->setValue( 1.0 );

	mPinLoopCount->setValue( 0 );

	mPinFileName->setDescription( tr( "The filename of the media file to load" ) );

	mPinVolume->setDescription( tr( "The volume of the media playback (normally 0.0 to 1.0, but you can specify higher than 1.0 to increase gain" ) );

	mPinPlay->setDescription( tr( "Trigger this pin to start the media playing" ) );
	mPinPause->setDescription( tr( "Trigger this pin to pause the media playback (trigger Play to start again)" ) );
	mPinStop->setDescription( tr( "Trigger this pin to stop playback and rewind to the beginning of the media" ) );
	mPinRewind->setDescription( tr( "Trigger this pin to rewind to the beginning of the media (without stopping playback)" ) );

	mPinLoopCount->setDescription( tr( "Set how many loops we should play this media for (0 = loop forever, 1 = play once, etc)" ) );

	mPinImage->setDescription( tr( "The output image from the media file - link to an Image Preview node to see it" ) );

	mPinAudio->setDescription( tr( "The audio output from the media file - link to an audio output node to hear it" ) );

	mPinLoaded->setDescription( tr( "This is true when the media file has been successfully loaded" ) );
	mPinPlaying->setDescription( tr( "This pin is true when the media file is playing, false otherwise" ) );
	mPinPaused->setDescription( tr( "This pin is true when the media file is paused, false otherwise" ) );
	mPinStopped->setDescription( tr( "This pin is true when the media file is stopped, false otherwise" ) );

	mPinLooped->setDescription( tr( "This pin is triggered everytime the media file is looped" ) );
	mPinLoopNumber->setDescription( tr( "This pin is the number of times the media has looped since it started playback" ) );
}

MediaNode::~MediaNode( void )
{
	unloadMedia();
}

bool MediaNode::initialise()
{
	if( !fugio::NodeControlBase::initialise() )
	{
		return( false );
	}

	mValLoaded->setVariant( false );
	mValPlaying->setVariant( false );
	mValPaused->setVariant( false );
	mValStopped->setVariant( true );

	connect( mNode->context()->qobject(), SIGNAL(frameStart(qint64)), this, SLOT(onContextFrame(qint64)) );

	return( true );
}

bool MediaNode::deinitialise()
{
	disconnect( mNode->context()->qobject(), SIGNAL(frameStart(qint64)), this, SLOT(onContextFrame(qint64)) );

	return( NodeControlBase::deinitialise() );
}

void MediaNode::inputsUpdated( qint64 pTimeStamp )
{
	if( !mSegment || mPinFileName->isUpdated( pTimeStamp ) )
	{
		QString		FileName = ( mSegment != 0 ? mSegment->filename() : "" );

		QUrl		FileNameUrl;

		fugio::FilenameInterface	*IFN = input<fugio::FilenameInterface *>( mPinFileName );

		if( IFN )
		{
			FileName = IFN->filename();

			FileNameUrl = QUrl::fromLocalFile( FileName );
		}
		else
		{
			FileName = variant( mPinFileName ).toString();
		}

		if( FileNameUrl.isEmpty() )
		{
			FileNameUrl = QUrl( FileName );
		}

		if( FileNameUrl.isLocalFile() )
		{
			FileName = FileNameUrl.toLocalFile();
		}

		if( FileName != mMediaFilename )
		{
			unloadMedia();

			if( loadMedia( FileName ) )
			{
				pinUpdated( mPinAudio );
			}

			mNextMediaState = STOP;

			mMediaFilename = FileName;
		}
	}

	mAudioVolume = qMax( 0.0, variant( mPinVolume ).toReal() );

	if( mPinInputFrameNumber->isUpdated( pTimeStamp ) )
	{
		mTargetFrameNumber = variant( mPinInputFrameNumber ).toInt();
	}

	if( mPinInputTime->isUpdated( pTimeStamp ) )
	{
		mTargetTime = variant( mPinInputTime ).toReal();
	}

	if( !pTimeStamp )
	{
		return;
	}

	if( mPinPlay->isUpdated( pTimeStamp ) )
	{
		mNextMediaState = PLAY;
	}

	if( mPinPause->isUpdated( pTimeStamp ) )
	{
		mNextMediaState = PAUSE;
	}

	if( mPinStop->isUpdated( pTimeStamp ) )
	{
		mNextMediaState = STOP;
	}

	if( mPinRewind->isUpdated( pTimeStamp ) )
	{
		mNextMediaState = REWIND;
	}

	if( mPinTrigger->isUpdated( pTimeStamp ) )
	{
		pinUpdated( mPinImage );
		pinUpdated( mPinAudio );
	}
}

void MediaNode::onContextFrame( qint64 pTimeStamp )
{
	// update the loaded status

	if( mValLoaded->variant().toBool() && !mSegment )
	{
		mValLoaded->setVariant( false );

		pinUpdated( mPinLoaded );
	}
	else if( !mValLoaded->variant().toBool() && mSegment )
	{
		mValLoaded->setVariant( true );

		pinUpdated( mPinLoaded );
	}

	// record the various status

	const bool		CurrPlay  = mValPlaying->variant().toBool();
	const bool		CurrPause = mValPaused->variant().toBool();
	const bool		CurrStop  = mValStopped->variant().toBool();

	// if we don't have any media there's nothing more to do

	if( !mSegment )
	{
		return;
	}

	const qint64		SegDur = qint64( mSegment->duration() * 1000.0 );
	qint64				TimCur = ( pTimeStamp - mTimeOffset ) + mTimePause;

	if( SegDur > 0 )
	{
		TimCur %= SegDur;
	}

	// process the next media state

	if( mCurrMediaState != mNextMediaState )
	{
		if( mNextMediaState == REWIND )
		{
			mTimeOffset   = pTimeStamp;
			mTimePause    = 0;
			mLoopCount    = 0;

			audioResetOffset();

			mNextMediaState = mCurrMediaState;
		}
		else if( mNextMediaState == PLAY )
		{
			if( mCurrMediaState == STOP )
			{
				mTimeOffset   = pTimeStamp;
				mTimePause    = 0;
				mLoopCount    = 0;

				audioResetOffset();
			}
			else if( mCurrMediaState == PAUSE )
			{
				mTimeOffset   = pTimeStamp;
				mTimePause    = TimCur;

				audioResetOffset();
			}
		}
		else if( mNextMediaState == PAUSE )
		{
			mTimeOffset = pTimeStamp;
			mTimePause  = TimCur;
		}
		else if( mNextMediaState == STOP )
		{
//			int i = 0;
		}

		mCurrMediaState = mNextMediaState;
	}

	if( mCurrMediaState == STOP )
	{
		mTimeOffset = pTimeStamp;
	}
	else if( mCurrMediaState == PAUSE )
	{
		mTimeOffset = pTimeStamp;
	}
	else if( mCurrMediaState == PLAY )
	{

	}

	if( mTargetFrameNumber >= 0 )
	{
		mTimeOffset = pTimeStamp;
		mTimePause  = qint64( mSegment->videoFrameRate() * qreal( mTargetFrameNumber ) );

		mTargetFrameNumber = -1;
	}

	if( mTargetTime >= 0 )
	{
		mTimeOffset = pTimeStamp;
		mTimePause  = mTargetTime * 1000.0;

		mTargetTime = -1;
	}

	TimCur = ( pTimeStamp - mTimeOffset ) + mTimePause;

	if( SegDur > 0 && TimCur >= SegDur )
	{
		//audioResetOffset();

		if( mCurrMediaState == PLAY )
		{
			mLoopCount = TimCur / SegDur;

			if( variant( mPinLoopCount ).toInt() > 0 && variant( mPinLoopCount ).toInt() <= mLoopCount )
			{
				mCurrMediaState = STOP;

				mValLoopNumber->setVariant( mLoopCount );

				pinUpdated( mPinLoopNumber );

				return;
			}
		}
		else
		{
			mLoopCount = 0;
		}

		if( mValLoopNumber->variant().toInt() != mLoopCount )
		{
			mValLoopNumber->setVariant( mLoopCount );

			pinUpdated( mPinLoopNumber );

			pinUpdated( mPinLooped );
		}

		TimCur %= SegDur;
	}

	if( TimCur != mTimeLast )
	{
		fugio::Performance	Perf( mNode, "onContextFrame", pTimeStamp );

		updateVideo( qreal( TimCur ) / 1000.0 );

		mTimeLast = TimCur;
	}

	// update the status

	switch( mCurrMediaState )
	{
		case STOP:
			mValPlaying->setVariant( false );
			mValPaused->setVariant( false );
			mValStopped->setVariant( true );
			break;

		case PLAY:
			mValPlaying->setVariant( true );
			mValPaused->setVariant( false );
			mValStopped->setVariant( false );
			break;

		case PAUSE:
			mValPlaying->setVariant( false );
			mValPaused->setVariant( true );
			mValStopped->setVariant( false );
			break;

		default:
			break;
	}

	// set the new playing status flags if changed

	if( mValPlaying->variant().toBool() != CurrPlay )
	{
		pinUpdated( mPinPlaying );
	}

	if( mValPaused->variant().toBool() != CurrPause )
	{
		pinUpdated( mPinPaused );
	}

	if( mValStopped->variant().toBool() != CurrStop )
	{
		pinUpdated( mPinStopped );
	}
}

bool MediaNode::loadMedia( const QString &pFileName )
{
	//fugio::Performance	Perf( mNode, "loadMedia", pTimeStamp );

	MediaSegment		*SV = new MediaSegment();

	if( !SV )
	{
		return( false );
	}

	if( !SV->loadMedia( pFileName, false ) )
	{
		delete SV;

		return( false );
	}

	setVideo( SV );

	return( true );
}

void MediaNode::setVideo( fugio::SegmentInterface *pSegment )
{
	unloadMedia();

	mSegment = pSegment;
}

void MediaNode::unloadMedia( void )
{
	if( mSegment )
	{
		delete mSegment;

		mSegment = 0;
	}

	mTimeOffset = 0;
	mTimePause  = 0;
	mTimeLast   = -1;
	mTargetFrameNumber = -1;
}

void MediaNode::updateVideo( qreal pTimeCurr )
{
#if defined( FFMPEG_SUPPORTED )
	mSegment->setPlayhead( pTimeCurr );

	const fugio::SegmentInterface::VidDat	*VD = mSegment->viddat();

	fugio::Image		SrcImg = mValImage->variant().value<fugio::Image>();

	if( !VD )
	{
		SrcImg.unsetBuffers();

		return;
	}

	if( VD->mPTS == mImgPts )
	{
		return;
	}

	mImgPts = VD->mPTS;

	SrcImg.setSize( mSegment->imageSize().width(), mSegment->imageSize().height() );

	SrcImg.setLineSizes( VD->mLineSizes );

	if( mSegment->imageIsHap() )
	{
		switch( HapTextureFormat( mSegment->imageFormat() ) )
		{
			case HapTextureFormat_RGB_DXT1:
				SrcImg.setFormat( fugio::ImageFormat::DXT1 );
				break;

			case HapTextureFormat_RGBA_DXT5:
				SrcImg.setFormat( fugio::ImageFormat::DXT5 );
				break;

			case HapTextureFormat_YCoCg_DXT5:
				SrcImg.setFormat( fugio::ImageFormat::YCoCg_DXT5 );
				break;

			default:
				SrcImg.setFormat( fugio::ImageFormat::UNKNOWN );
				break;
		}
	}
	else
	{
		SrcImg.setInternalFormat( mSegment->imageFormat() );

		switch( AVPixelFormat( mSegment->imageFormat() ) )
		{
			case AV_PIX_FMT_RGB24:
				SrcImg.setFormat( fugio::ImageFormat::RGB8 );
				break;

			case AV_PIX_FMT_RGBA:
				SrcImg.setFormat( fugio::ImageFormat::RGBA8 );
				break;

			case AV_PIX_FMT_BGR24:
				SrcImg.setFormat( fugio::ImageFormat::BGR8 );
				break;

			case AV_PIX_FMT_BGRA:
				SrcImg.setFormat( fugio::ImageFormat::BGRA8 );
				break;

			case AV_PIX_FMT_YUYV422:
				SrcImg.setFormat( fugio::ImageFormat::YUYV422 );
				break;

			case AV_PIX_FMT_GRAY8:
				SrcImg.setFormat( fugio::ImageFormat::GRAY8 );
				break;

			case AV_PIX_FMT_GRAY16:
				SrcImg.setFormat( fugio::ImageFormat::GRAY16 );
				break;

			default:
				SrcImg.setFormat( fugio::ImageFormat::INTERNAL );
				break;
		}
	}

	SrcImg.setBuffers( VD->mData );
	SrcImg.setLineSizes( VD->mLineSizes );

	if( SrcImg.format() != fugio::ImageFormat::UNKNOWN )
	{
		pinUpdated( mPinImage );
	}
#endif
}

void MediaNode::audio( qint64 pSamplePosition, qint64 pSampleCount, int pChannelOffset, int pChannelCount, void **pBuffers, AudioInstanceData *pInstanceData ) const
{
	if( !pInstanceData || mCurrMediaState != PLAY || !mSegment )
	{
		return;
	}

	if( !pInstanceData->mSampleOffset )
	{
		pInstanceData->mSampleOffset = ( mTimeOffset - mTimePause ) * 48;

//		qDebug() << "Offset" << mTimeOffset << "Pause" << mTimePause << "SampleOffset" << pInstanceData->mSampleOffset << "SamplePosition" << ( pSamplePosition - pInstanceData->mSampleOffset );
	}

	pSamplePosition -= pInstanceData->mSampleOffset;

	const qint64		SegDur = qint64( mSegment->duration() * 48000.0 );

	if( SegDur > 0 )
	{
		pSamplePosition %= SegDur;
	}

	if( pSamplePosition < 0 )
	{
		return;
	}

	if( mSegment->hasAudio() )
	{
		mSegment->mixAudio( pSamplePosition, pSampleCount, pChannelOffset, pChannelCount, pBuffers, mAudioVolume );
	}
}

fugio::AudioInstanceBase *MediaNode::audioAllocInstance( qreal pSampleRate, fugio::AudioSampleFormat pSampleFormat, int pChannels )
{
	AudioInstanceData		*InsDat = new AudioInstanceData( qSharedPointerDynamicCast<fugio::AudioProducerInterface>( mNode->control() ), pSampleRate, pSampleFormat, pChannels );

	if( InsDat )
	{
		QMutexLocker	MutLck( &mInstanceMutex );

		mInstanceData.append( InsDat );
	}

	return( InsDat );
}

int MediaNode::audioChannels() const
{
	return( mSegment ? mSegment->audioChannels() : 0 );
}

qreal MediaNode::audioSampleRate() const
{
	return( mSegment ? mSegment->sampleRate() : 0 );
}

fugio::AudioSampleFormat MediaNode::audioSampleFormat() const
{
	return( fugio::AudioSampleFormat::Format32FS );
}

qint64 MediaNode::audioLatency() const
{
	return( 0 );
}

void MediaNode::audioRemoveInstance( AudioInstanceData *pInstance )
{
	QMutexLocker		MutLck( &mInstanceMutex );

	mInstanceData.removeAll( pInstance );
}

void MediaNode::audioResetOffset()
{
	QMutexLocker		MutLck( &mInstanceMutex );

	for( AudioInstanceData *pInstance : mInstanceData )
	{
		pInstance->mSampleOffset = 0;
	}
}
