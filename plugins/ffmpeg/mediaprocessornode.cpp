#include "mediaprocessornode.h"

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
//#include <fugio/timeline/uuid.h>
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
//#include "mediaplayervideopreview.h"
#include "mediaaudioprocessor.h"

#if defined( FFMPEG_SUPPORTED )
#include "hap/source/hap.h"
#endif

MediaProcessorNode::MediaProcessorNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mSegment( 0 ),
	  mTimeOffset( 0 ), mTimePause( 0 ), mTimeLast( -1 ), mLstPts( -1 )
{
	FUGID( PIN_FILENAME, "43d2824f-7967-4b22-8b0f-c51358b65d17" );
	FUGID( PIN_REWIND, "ae66182c-d914-49cb-85d4-615db69cf3e2" );

	FUGID( PIN_IMAGE, "e0a3e13b-6669-4793-8eb0-e9a12afb0f6b" );
	FUGID( PIN_AUDIO, "864cae6d-87a4-4f26-8f64-fd0185dad2cf" );

	mPinFileName = pinInput( "Filename", PIN_FILENAME );

	mPinTrigger = pinInput( "Trigger", PID_FUGIO_NODE_TRIGGER );

	mPinRewind = pinInput( "Rewind", PIN_REWIND );

	mValImage = pinOutput<fugio::VariantInterface *>( "Image", mPinImage, PID_IMAGE, PIN_IMAGE );

	mValAudio = pinOutput<fugio::AudioProducerInterface *>( "Audio", mPinAudio, PID_AUDIO, PIN_AUDIO );

	AB.setChannels( 2 );

	AB.clear();

	mPinFileName->setDescription( tr( "The filename of the media file to load" ) );

	mPinImage->setDescription( tr( "The output image from the media file - link to an Image Preview node to see it" ) );

	mPinAudio->setDescription( tr( "The audio output from the media file - link to a PortAudio node to hear it" ) );
}

MediaProcessorNode::~MediaProcessorNode( void )
{
	unloadMedia();
}

void MediaProcessorNode::inputsUpdated( qint64 pTimeStamp )
{
	QString		FileName = ( mSegment != 0 ? mSegment->filename() : "" );

	QUrl	FileNameUrl;

	fugio::FilenameInterface						*IFN = input<fugio::FilenameInterface *>( mPinFileName );

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

		loadMedia( FileName );

		mMediaFilename = FileName;
	}

	if( !pTimeStamp )
	{
		return;
	}

	// if we don't have any media there's nothing more to do

	if( !mSegment )
	{
		return;
	}

	if( mPinRewind->isUpdated( pTimeStamp ) )
	{
		mSegment->setPlayhead( 0 );

		mLstPts = -1;
	}

	// process the next media state

	fugio::Performance	Perf( mNode, "onContextFrame", pTimeStamp );

	if( mSegment->hasVideo() )
	{
		const fugio::SegmentInterface::VidDat	*VD = mSegment->viddat();

		if( !VD || mLstPts == VD->mPTS )
		{
			mSegment->readNext();
		}

		VD = mSegment->viddat();

		if( !VD || VD->mPTS == mLstPts )
		{
			return;
		}

		mLstPts = VD->mPTS;

		fugio::Image		I = mValImage->variant().value<fugio::Image>();

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
#else
			I.setFormat( fugio::ImageFormat::UNKNOWN );
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

				case AV_PIX_FMT_YUV420P:
					I.setFormat( fugio::ImageFormat::YUV420P );
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
#else
			I.setFormat( fugio::ImageFormat::INTERNAL );
#endif
		}

		I.setBuffers( VD->mData );
		I.setLineSizes( VD->mLineSizes );

		if( I.format() != fugio::ImageFormat::UNKNOWN )
		{
			pinUpdated( mPinImage );
		}
	}
}

bool MediaProcessorNode::loadMedia( const QString &pFileName )
{
	//fugio::Performance	Perf( mNode, "loadMedia", pTimeStamp );

	MediaSegment		*SV = new MediaSegment();

	if( !SV )
	{
		return( false );
	}

	if( !SV->loadMedia( pFileName, false ) )
	{
		mNode->setStatus( fugio::NodeInterface::Error );

		mNode->setStatusMessage( SV->statusMessage() );

		delete SV;

		return( false );
	}

	mNode->setStatus( fugio::NodeInterface::Initialised );

	mNode->setStatusMessage( "" );

	setVideo( SV );

	return( true );
}

void MediaProcessorNode::setVideo( fugio::SegmentInterface *pSegment )
{
	unloadMedia();

	mSegment = pSegment;
}

void MediaProcessorNode::unloadMedia( void )
{
	if( mSegment )
	{
		delete mSegment;

		mSegment = 0;
	}
}

void MediaProcessorNode::audio( qint64 pSamplePosition, qint64 pSampleCount, int pChannelOffset, int pChannelCount, void **pBuffers, AudioInstanceData *pInstanceData ) const
{
	if( !pInstanceData || !mSegment )
	{
		return;
	}

	if( !mSampleOffset )
	{
		// TODO: FIXME
		//		mSampleOffset = pSamplePosition + pLatency;
	}

	qreal				TimeCurr;

	const qint64		SegDur = qint64( mSegment->duration() * 48000.0 );

	pSamplePosition = ( pSamplePosition - mSampleOffset + ( mTimePause * 48 ) ) % SegDur;
	TimeCurr        = qreal( pSamplePosition ) / 48000.0;

	if( pSamplePosition < 0 || TimeCurr < 0 )
	{
		return;
	}

#if 1
	mSegment->mixAudio( pSamplePosition, pSampleCount, pChannelOffset, pChannelCount, pBuffers, 1 );
#else
	static const int		 AUD_BUF_SAMPLES = 4096;
	static const int		 AUD_BUF_BYTES   = AUD_BUF_SAMPLES * sizeof( float );
	static const int		 AUD_MIX_SAMPLES = 4096;
	static const int		 AUD_MIX_BYTES   = AUD_MIX_SAMPLES * sizeof( float );

	const int				 SAMPLES_BYTES    = pSampleCount * sizeof( float );

	AB.setChannels( pChannels );

	for( QVector<float> &V : AB.AudMix )
	{
		V.resize( AUD_MIX_SAMPLES );
	}

	for( QVector<float> &V : AB.AudDat )
	{
		V.resize( AUD_BUF_SAMPLES + AUD_MIX_SAMPLES );
	}

	QVector<float *>		VP;

	VP.resize( pChannels );

	while( AB.AudBuf.first().size() < pSampleCount )
	{
		for( int c = 0 ; c < pChannels ; c++ )
		{
			VP[ c ] = AB.AudDat[ c ].data();

			memset( VP[ c ], 0, AUD_BUF_BYTES + AUD_MIX_BYTES );
		}

		mSegment->mixAudio( pSamplePosition + AB.AudBuf.first().size(), AUD_BUF_SAMPLES + AUD_MIX_SAMPLES, pChannels, VP.data(), pVol * mAudioVolume );

		for( int c = 0 ; c < pChannels ; c++ )
		{
#if 1
			if( AUD_MIX_SAMPLES > 0 )
			{
				float		*AudDat = AB.AudDat[ c ].data();
				float		*AudMix = AB.AudMix[ c ].data();

				for( int i = 0 ; i < AUD_MIX_SAMPLES ; i++ )
				{
					const float	v = float( i ) / float( AUD_MIX_SAMPLES );

					AudDat[ i ] = AudioBuffer3::mix( AudDat[ i ], AudMix[ i ], v );
				}
			}
#endif

			const int		AudBufIdx = AB.AudBuf[ c ].size();

			AB.AudBuf[ c ].resize( AudBufIdx + AUD_BUF_SAMPLES );

			memcpy( &AB.AudBuf[ c ][ AudBufIdx ], AB.AudDat[ c ].data(), AUD_BUF_BYTES );

			// copy the next samples to the mix buffer

			memcpy( AB.AudMix[ c ].data(), &AB.AudDat[ c ][ AUD_BUF_SAMPLES ], AUD_MIX_BYTES );
		}
	}

	for( int c = 0 ; c < pChannels ; c++ )
	{
		memcpy( pBuffers[ c ], AB.AudBuf[ c ].data(), SAMPLES_BYTES );
	}

	for( QVector<float> &V : AB.AudBuf )
	{
		V.remove( 0, pSampleCount );
	}
#endif
}

fugio::AudioInstanceBase *MediaProcessorNode::audioAllocInstance( qreal pSampleRate, fugio::AudioSampleFormat pSampleFormat, int pChannels )
{
	AudioInstanceData		*InsDat = new AudioInstanceData( qSharedPointerDynamicCast<fugio::AudioProducerInterface>( mNode->control() ), pSampleRate, pSampleFormat, pChannels );

	if( InsDat )
	{
	}

	return( InsDat );
}

int MediaProcessorNode::audioChannels() const
{
	return( 0 );
}

qreal MediaProcessorNode::audioSampleRate() const
{
	return( 0 );
}

fugio::AudioSampleFormat MediaProcessorNode::audioSampleFormat() const
{
	return( fugio::AudioSampleFormat::FormatUnknown );
}

qint64 MediaProcessorNode::audioLatency() const
{
	return( 0 );
}
