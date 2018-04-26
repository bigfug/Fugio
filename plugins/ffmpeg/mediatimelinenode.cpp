#include "mediatimelinenode.h"

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
#include <fugio/timeline/uuid.h>
#include <fugio/image/uuid.h>
#include <fugio/context_signals.h>
#include <fugio/performance.h>

#include <fugio/global_interface.h>
#include <fugio/context_interface.h>
#include <fugio/image/image.h>
#include <fugio/core/variant_interface.h>
#include <fugio/file/filename_interface.h>
#include <fugio/audio/audio_producer_interface.h>
#include <fugio/editor_interface.h>
#include <fugio/timeline/timeline_interface.h>
#include <fugio/timeline/timeline_control_interface.h>
#include <fugio/timeline/timeline_widget_interface.h>
#include <fugio/timeline/keyframes_provider_interface.h>
#include <fugio/timeline/keyframes_time_interface.h>
#include <fugio/timeline/keyframes_editor_interface.h>
#include <fugio/timeline/keyframes_widget_interface.h>

#include <fugio/timeline/keyframedatatime.h>

#include "mediasegment.h"
#include "mediaaudioprocessor.h"
#include "mediaplayervideopreview.h"

#include "ffmpegplugin.h"

MediaTimelineNode::MediaTimelineNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mTimelineControl( 0 ), mSegment( 0 ), mKF( 0 ), mAudioMute( false ), mAudioVolume( 1 ),
	  mLastImageUpdate( 0 ), mPreloadAudio( false ), mFrameDecodeTime( 0 )
{
	FUGID( PIN_INPUT_FILENAME, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_INPUT_VOLUME, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );
//	FUGID( PIN_INPUT_ALPHA, "261cc653-d7fa-4c34-a08b-3603e8ae71d5" );
//	FUGID( PIN_INPUT_SCRATCH, "ce8d578e-c5a4-422f-b3c4-a1bdf40facdb" );
	FUGID( PIN_OUTPUT_IMAGE, "249f2932-f483-422f-b811-ab679f006381" );
	FUGID( PIN_OUTPUT_AUDIO, "e6bf944e-5f46-4994-bd51-13c2aa6415b7" );

	mPinFilename = pinInput( "Filename", PIN_INPUT_FILENAME );

	mPinVolume = pinInput( "Volume", PIN_INPUT_VOLUME );

//	mPinAlpha = pinInput( "Alpha", PIN_INPUT_ALPHA );

//	mPinScratch = pinInput( "Scratch", PIN_INPUT_SCRATCH );

	mValImage = pinOutput<fugio::VariantInterface *>( "Image", mPinImage, PID_IMAGE, PIN_OUTPUT_IMAGE );

	mValAudio = pinOutput<fugio::AudioProducerInterface *>( "Audio", mPinAudio, PID_AUDIO, PIN_OUTPUT_AUDIO );

	mPinVolume->setValue( 1.0 );
//	mPinAlpha->setValue( 1.0 );

	AB.setChannels( 2 );

	AB.clear();

	mPinFilename->setDescription( tr( "The filename of the media file to load" ) );

	mPinVolume->setDescription( tr( "The volume of the media playback (normally 0.0 to 1.0, but you can specify higher than 1.0 to increase gain" ) );

//	mPinAlpha->setDescription( tr( "Sets the alpha of the output image" ) );

	mPinImage->setDescription( tr( "The output image from the media file - link to an Image Preview node to see it" ) );

	mPinAudio->setDescription( tr( "The audio output from the media file - link to a PortAudio node to hear it" ) );

	TimelineInterface *TL = qobject_cast<TimelineInterface *>( ffmpegPlugin::instance()->app()->findInterface( IID_TIMELINE ) );

	if( TL )
	{
		mTimelineControl = TL->control( this );

		mKF = qobject_cast<KeyFramesTimeInterface *>( TL->keyframes( this, KID_TIME )->qobject() );
	}
}

MediaTimelineNode::~MediaTimelineNode( void )
{
	unloadMedia();
}

bool MediaTimelineNode::initialise()
{
	if( !NodeControlBase::initialise() )
	{
		return( false );
	}

	if( !mTimelineControl )
	{
		return( false );
	}

	if( !mTimelineControl->initialise() )
	{
		return( false );
	}

	TimelineWidgetInterface		*TLW = qobject_cast<TimelineWidgetInterface *>( mNode->context()->findInterface( IID_TIMELINE_WIDGET ) );

	if( TLW )
	{
		TLW->addTrack( this );
	}

	connect( mNode->context()->qobject(), SIGNAL(stateChanged(ContextInterface::TimeState)), this, SLOT(stateChanged(ContextInterface::TimeState)) );
	connect( mNode->context()->qobject(), SIGNAL(aboutToPlay()), this, SLOT(aboutToPlay()) );

	// Don't do this
	// mNode->context()->registerPlayhead( this );

	playheadMove( 0 );

	return( true );
}

bool MediaTimelineNode::deinitialise()
{
	if( !mTimelineControl->deinitialise() )
	{
		return( false );
	}

	// mNode->context()->unregisterPlayhead( this );

	TimelineWidgetInterface		*TLW = qobject_cast<TimelineWidgetInterface *>( mNode->context()->findInterface( IID_TIMELINE_WIDGET ) );

	if( TLW )
	{
		TLW->remTrack( this );
	}

	return( true );
}

QWidget *MediaTimelineNode::gui()
{
	QPushButton		*GUI = new QPushButton( tr( "Preload Audio" ) );

	GUI->setCheckable( true );

	GUI->setChecked( mPreloadAudio );

	connect( GUI, SIGNAL(clicked(bool)), this, SLOT(buttonPreload(bool)) );

	connect( this, SIGNAL(preloaded(bool)), GUI, SLOT(setChecked(bool)) );

	return( GUI );
}

void MediaTimelineNode::inputsUpdated( qint64 pTimeStamp )
{
	QString		FileName = ( mSegment != 0 ? mSegment->filename() : "" );

	QUrl	FileNameUrl;

	if( mPinFilename->isConnected() )
	{
		QSharedPointer<fugio::PinControlInterface>	 CTL = mPinFilename->connectedPin()->control();
		fugio::VariantInterface						*VAR = ( CTL == 0 ? 0 : qobject_cast<fugio::VariantInterface *>( CTL->qobject() ) );

		if( VAR != 0 )
		{
			FileName = VAR->variant().toString();
		}

		FilenameInterface							*IFN = input<FilenameInterface *>( mPinFilename );

		if( IFN )
		{
			FileName = IFN->filename();

			FileNameUrl = QUrl::fromLocalFile( FileName );
		}
	}
	else
	{
		FileName = mPinFilename->value().toString();
	}

	if( FileNameUrl.isEmpty() )
	{
		FileNameUrl = QUrl( FileName );
	}

	if( FileNameUrl.isLocalFile() )
	{
		FileName = FileNameUrl.toLocalFile();
	}

	if( FileName != ( mSegment != 0 ? mSegment->filename() : "" ) )
	{
		unloadMedia();

		loadMedia( FileName );
	}

	qreal		Volume = variant( mPinVolume ).toDouble();

	if( mAudioVolume != Volume )
	{
		mAudioVolume = Volume;
	}

//	qreal		Alpha = variant( mPinAlpha ).toDouble();

//	if( mVideoAlpha != Alpha )
//	{
//		mVideoAlpha = Alpha;
//	}

	if( mTimelineControl )
	{
		mTimelineControl->inputsUpdated( pTimeStamp );
	}
}

void MediaTimelineNode::loadSettings( QSettings &pSettings )
{
	pSettings.beginGroup( "timeline" );

	mTimelineControl->cfgLoad( pSettings );

	pSettings.endGroup();

	pSettings.beginGroup( "time" );

	mKF->cfgLoad( pSettings );

	pSettings.endGroup();

	mPreloadAudio = pSettings.value( "preload-audio", mPreloadAudio ).toBool();

	if( mSegment )
	{
		mSegment->setPreload( mPreloadAudio );
	}

	emit preloaded( mPreloadAudio );
}

void MediaTimelineNode::saveSettings( QSettings &pSettings ) const
{
	pSettings.beginGroup( "timeline" );

	mTimelineControl->cfgSave( pSettings );

	pSettings.endGroup();

	pSettings.beginGroup( "time" );

	mKF->cfgSave( pSettings );

	pSettings.endGroup();

	pSettings.setValue( "preload-audio", mPreloadAudio );
}

QList<fugio::NodeControlInterface::AvailablePinEntry> MediaTimelineNode::availableOutputPins() const
{
	QList<fugio::NodeControlInterface::AvailablePinEntry>		PinLst = NodeControlBase::availableOutputPins();

	PinLst.append( mTimelineControl->availableOutputPins() );

	std::sort( PinLst.begin(), PinLst.end() );

	return( PinLst );
}

QStringList MediaTimelineNode::availableInputPins() const
{
	QStringList		PinLst = NodeControlBase::availableInputPins();

	PinLst.append( mTimelineControl->availableInputPins() );

	PinLst.sort();

	return( PinLst );
}

qreal MediaTimelineNode::position() const
{
	return( mTimelineControl->position() );
}

qreal MediaTimelineNode::duration() const
{
	return( mNode->context()->duration() );
}

KeyFramesWidgetInterface *MediaTimelineNode::newTimelineGui()
{
	TimelineInterface *TL = qobject_cast<TimelineInterface *>( mNode->context()->global()->findInterface( IID_TIMELINE ) );

	if( TL )
	{
		KeyFramesEditorInterface	*KFE = qobject_cast<KeyFramesProviderInterface *>( mKF->qobject() )->newEditor();

		if( KFE )
		{
			KeyFramesWidgetInterface	*KFW = TL->keyframesWidget( this, mTimelineControl, KFE );

			if( KFW )
			{
				KFW->setBackground( this );

				return( KFW );
			}

			delete KFE;
		}
	}

	return( 0 );
}

void MediaTimelineNode::drawBackground( const KeyFramesWidgetInterface *pTrackWidget, const QRect &pUpdateRect, QImage &pBackImage ) const
{
	if( !mSegment )
	{
		return;
	}

	QPainter		Painter( &pBackImage );

	Painter.drawImage( pUpdateRect, pTrackWidget->viewImage(), pUpdateRect );

	Painter.setCompositionMode( QPainter::RasterOp_SourceXorDestination );

	Painter.setPen( Qt::red ); //colorAudio() );

	for( int i = pUpdateRect.left() ; i <= pUpdateRect.right() ; i++ )
	{
		const qint64	t1 = pTrackWidget->viewToTimestamp( i + 0 ) * 1000.0;
		const qint64	t2 = pTrackWidget->viewToTimestamp( i + 1 ) * 1000.0;

		qreal			l  = mSegment->wavL( mKF->time( qreal( t1 ) / 1000.0 ) );
		qreal			r  = mSegment->wavR( mKF->time( qreal( t1 ) / 1000.0 ) );

		for( qint64 j = t1 + 1 ; j <= t2 ; j++ )
		{
			qreal		t = mKF->time( qreal( j ) / 1000.0 );

			l = qMax( mSegment->wavL( t ), l );
			r = qMax( mSegment->wavR( t ), r );
		}

		l = l * l;
		r = r * r;

		qint32			y1 = pTrackWidget->valueToView( 0.5 + ( l * 0.5 ) );
		qint32			y2 = pTrackWidget->valueToView( 0.5 - ( r * 0.5 ) );

		if( y1 != y2 )
		{
			Painter.drawLine( i, y1, i, y2 );
		}
		else
		{
			Painter.drawPoint( i, pTrackWidget->valueToView( 0.5 ) );
		}
	}

	Painter.setCompositionMode( QPainter::CompositionMode_SourceOver );

	//-------------------------------------------------------------------------
	//

//	pPainter.setPen( Qt::red );

//	for( int i = UpdateRect.left() ; i <= UpdateRect.right() ; i++ )
//	{
//		const qreal	t = pTrackWidget->viewToTimestamp( i );
//		const qreal	v = mKF->time( t );
//		const int	y = pTrackWidget->valueToView( qBound( 0.0, qreal( v ) / MedDur, 1.0 ) );

//		pPainter.drawPoint( i, y );
//	}
}

bool MediaTimelineNode::loadMedia( const QString &pFileName )
{
	MediaSegment		*SV = new MediaSegment();

	if( SV == 0 )
	{
		return( false );
	}

	SV->setPreload( mPreloadAudio );

	if( !SV->loadMedia( pFileName, mNode->context()->global()->findInterface( IID_EDITOR ) != nullptr ) )
	{
		delete SV;

		return( false );
	}

	setVideo( SV );

	mTimelineControl->update();

	if( SV->duration() > mNode->context()->duration() )
	{
		mNode->context()->setDuration( SV->duration() );
	}

	playheadMove( 0 );

	return( true );
}

void MediaTimelineNode::setVideo( fugio::SegmentInterface *pSegment )
{
	unloadMedia();

	mSegment = pSegment;

	mKF->setTimeMax( mSegment != 0 && mSegment->duration() > 0 ? mSegment->duration() : -1 );

	mSegment->setPreload( mPreloadAudio );
}

void MediaTimelineNode::setInstanceSampleOffset(qint64 pSampleOffset)
{
	QMutexLocker		Lock( &mInstanceMutex );

	for( AudioInstanceData *AID : mInstanceList )
	{
		AID->mSampleOffset = pSampleOffset;
	}
}

void MediaTimelineNode::unloadMedia( void )
{
	if( mSegment != 0 )
	{
		delete mSegment;

		mSegment = 0;
	}
}

void MediaTimelineNode::updateVideo( qreal pTimeCurr )
{
//	static qint64	FRAME_TS = 1000;

//	qint64	ts = QDateTime::currentMSecsSinceEpoch();

//	if( !mFrameDecodeTime )
//	{
//		mFrameDecodeTime = ts;
//	}
//	else if( ts - mFrameDecodeTime < FRAME_TS )
//	{
//		return;
//	}
//	else
//	{
//		mFrameDecodeTime += FRAME_TS;
//	}

	fugio::Performance	Perf( mNode, "updateVideo", pTimeCurr );

	qreal	ImgPts = mSegment->videoFrameTimeStamp();

	mSegment->setPlayhead( mKF->time( pTimeCurr ) );

	if( mSegment->videoFrameTimeStamp() == ImgPts )
	{
		return;
	}

	fugio::Image	I = mValImage->variant().value<fugio::Image>();

	const SegmentInterface::VidDat	*VD = mSegment->viddat();

	if( VD )
	{
		I.setSize( mSegment->imageSize().width(), mSegment->imageSize().height() );

		I.setLineSizes( VD->mLineSizes );

		if( mSegment->imageIsHap() )
		{
#if defined( FFMPEG_SUPPORTED )
			switch( HapTextureFormat( mSegment->imageFormat() ) )
			{
				case HapTextureFormat_RGB_DXT1:
					I.setFormat( fugio::ImageFormat::DXT1 );
					break;

				case HapTextureFormat_RGBA_DXT5:
					I.setFormat( fugio::ImageFormat::DXT5 );
					break;

				case HapTextureFormat_YCoCg_DXT5:
					I.setFormat( fugio::ImageFormat::YCoCg_DXT5 );
					break;

				default:
					I.setFormat( fugio::ImageFormat::UNKNOWN );
					break;
			}
#endif
		}
		else
		{
			I.setInternalFormat( mSegment->imageFormat() );

#if defined( FFMPEG_SUPPORTED )
			switch( AVPixelFormat( mSegment->imageFormat() ) )
			{
				case AV_PIX_FMT_RGB24:
					I.setFormat( fugio::ImageFormat::RGB8 );
					break;

				case AV_PIX_FMT_RGBA:
					I.setFormat( fugio::ImageFormat::RGBA8 );
					break;

				case AV_PIX_FMT_BGR24:
					I.setFormat( fugio::ImageFormat::BGR8 );
					break;

				case AV_PIX_FMT_BGRA:
					I.setFormat( fugio::ImageFormat::BGRA8 );
					break;

				case AV_PIX_FMT_YUYV422:
					I.setFormat( fugio::ImageFormat::YUYV422 );
					break;

				case AV_PIX_FMT_UYVY422:
					I.setFormat( fugio::ImageFormat::UYVY422 );
					break;

				case AV_PIX_FMT_GRAY8:
					I.setFormat( fugio::ImageFormat::GRAY8 );
					break;

				case AV_PIX_FMT_GRAY16:
					I.setFormat( fugio::ImageFormat::GRAY16 );
					break;

				default:
					I.setFormat( fugio::ImageFormat::INTERNAL );
					break;
			}
#endif
		}

		I.setBuffers( VD->mData );
		I.setLineSizes( VD->mLineSizes );

		if( I.format() != fugio::ImageFormat::UNKNOWN )
		{
			pinUpdated( mPinImage );
		}
	}
	else
	{
		I.unsetBuffers();
	}
}

void MediaTimelineNode::buttonPreload( bool pState )
{
	mPreloadAudio = pState;

	if( mSegment == 0 )
	{
		return;
	}

	mSegment->setPreload( pState );
}

qreal MediaTimelineNode::latency() const
{
	return( mTimelineControl->latency() );
}

QList<KeyFramesControlsInterface *> MediaTimelineNode::editorControls()
{
	QList<KeyFramesControlsInterface *>	CtlLst;

	MediaPlayerVideoPreview	*CTL = new MediaPlayerVideoPreview( this );

	CTL->setObjectName( tr( "Video" ) );

	CtlLst.append( CTL );

	return( CtlLst );
}

void MediaTimelineNode::aboutToPlay()
{
	setInstanceSampleOffset( 0 );

	mTimeOffset   = mTimeLast;
}

void MediaTimelineNode::stateChanged( fugio::ContextInterface::TimeState pState )
{
	if( pState == fugio::ContextInterface::Stopped )
	{
		setInstanceSampleOffset( 0 );

		mTimeOffset   = mTimeLast;
	}
	else if( pState == fugio::ContextInterface::Playing )
	{
		setInstanceSampleOffset( 0 );

		mTimeOffset   = mTimeLast;
	}
}

void MediaTimelineNode::playheadMove( qreal pTimeStamp )
{
	mTimeOffset   = pTimeStamp;
	mTimeLast     = pTimeStamp;

	setInstanceSampleOffset( 0 );

	if( mSegment == 0 )
	{
		return;
	}

	updateVideo( pTimeStamp );
}

bool MediaTimelineNode::playheadPlay( qreal pTimePrev, qreal pTimeCurr )
{
	Q_UNUSED( pTimePrev )

	mTimeLast = pTimeCurr;

	if( !mTimelineControl->isPlaying() )
	{
		return( true );
	}

//	if( mSampleOffset < 0 )
//	{
//		mTimeOffset   = pTimeCurr;
//		mSampleOffset = -1;
//	}

	if( mSegment == 0 )
	{
		return( false );
	}

	updateVideo( pTimeCurr );

	return( true );
}

void MediaTimelineNode::setTimeOffset( qreal pTimeOffset )
{
	mTimelineControl->setTimeOffset( pTimeOffset );

//	mTimeOffset   = pTimeOffset;
//	mSampleOffset = -1;
}

void MediaTimelineNode::audio( qint64 pSamplePosition, qint64 pSampleCount, int pChannelOffset, int pChannelCount, void **pBuffers, AudioInstanceData *pInstanceData ) const
{
	AudioInstanceData		*InsDat = static_cast<AudioInstanceData *>( pInstanceData );

	if( !InsDat || !mTimelineControl->isPlaying() ) //!mNode->context()->isPlaying() ) //!isPlaying() || mAudioMute || !isSending() )
	{
		return;
	}

	if( !InsDat->mSampleOffset )
	{
		InsDat->mSampleOffset = pSamplePosition - qint64( mTimeOffset * 48000.0 );
	}

	if( mSegment == 0 )
	{
		return;
	}

	qreal				 TimeCurr;

	if( mTimelineControl->isTrigger() )
	{
		pSamplePosition = mTimelineControl->position() * 48000.0;
		TimeCurr        = mTimelineControl->position();
	}
	else
	{
		pSamplePosition = ( pSamplePosition - InsDat->mSampleOffset ) - qint64( mTimelineControl->timeoffset() * 48000.0 );
		TimeCurr        = qreal( pSamplePosition ) / 48000.0;
	}

	if( pSamplePosition < 0 || TimeCurr < 0 )
	{
		return;
	}

	KeyFrameDataTime	*CurKF;

	if( ( CurKF = mKF->keyframeTimeValue( TimeCurr ) ) != 0 && CurKF->interpolation() == TIME_PAUSE )
	{
		return;
	}

	KeyFrameDataTime	*PrvKF;

	if( ( PrvKF = mKF->keyframeTimeValue( mKF->keyframes()->keyframePrev( TimeCurr ) ) ) != 0 && PrvKF->interpolation() == TIME_PAUSE )
	{
		return;
	}

	pSamplePosition = mKF->time( qreal( pSamplePosition ) / 48000.0 ) * 48000.0;

	mSegment->mixAudio( pSamplePosition, pSampleCount, pChannelOffset, pChannelCount, pBuffers, mAudioVolume );
}

AudioInstanceBase *MediaTimelineNode::audioAllocInstance( qreal pSampleRate, fugio::AudioSampleFormat pSampleFormat, int pChannels )
{
	AudioInstanceData		*InsDat = new AudioInstanceData( qSharedPointerDynamicCast<fugio::AudioProducerInterface>( mNode->control() ), pSampleRate, pSampleFormat, pChannels );

	if( InsDat )
	{
		InsDat->mSampleOffset = 0;

		mInstanceMutex.lock();

		mInstanceList.append( InsDat );

		mInstanceMutex.unlock();
	}

	return( InsDat );
}

int MediaTimelineNode::audioChannels() const
{
	return( 0 );
}

qreal MediaTimelineNode::audioSampleRate() const
{
	return( 0 );
}

AudioSampleFormat MediaTimelineNode::audioSampleFormat() const
{
	return( fugio::AudioSampleFormat::FormatUnknown );
}

qint64 MediaTimelineNode::audioLatency() const
{
	return( 0 );
}
