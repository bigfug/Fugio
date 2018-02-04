#include "ffmpegplugin.h"

#include <QtPlugin>

#include <QDebug>

#include <QTranslator>
#include <QApplication>

#ifndef INT64_C
#define INT64_C(c) (c ## LL)
#define UINT64_C(c) (c ## ULL)
#endif

#if defined( FFMPEG_SUPPORTED )
extern "C"
{
	#include <libavformat/avformat.h>
	#include <libavdevice/avdevice.h>
}
#endif

#include <fugio/ffmpeg/uuid.h>

#include "medianode.h"
#include "imageconvertnode.h"
#include "mediarecordernode.h"
#include "mediaprocessornode.h"
#include "mediatimelinenode.h"

QList<QUuid>				 NodeControlBase::PID_UUID;

ffmpegPlugin				*ffmpegPlugin::mInstance = 0;

ffmpegPlugin::ffmpegPlugin( void )
	: mApp( 0 )
{
	//-------------------------------------------------------------------------
	// Install translator

	static QTranslator		Translator;

	if( Translator.load( QLocale(), QLatin1String( "translations" ), QLatin1String( "_" ), ":/" ) )
	{
		qApp->installTranslator( &Translator );
	}

	mNodeEntries.append( ClassEntry( "Image Convert", "FFMPEG", NID_SWR_IMAGE_CONVERT, &ImageConvertNode::staticMetaObject ) );
	mNodeEntries.append( ClassEntry( "Media Player",   "FFMPEG", NID_MEDIA_PLAYER, &MediaNode::staticMetaObject ) );
	mNodeEntries.append( ClassEntry( "Media Recorder",   "FFMPEG", NID_MEDIA_RECORDER, &MediaRecorderNode::staticMetaObject ) );
	mNodeEntries.append( ClassEntry( "Media Processor",   "FFMPEG", NID_MEDIA_PROCESSOR, &MediaProcessorNode::staticMetaObject ) );
	mNodeEntries.append( ClassEntry( "Media Timeline",   "FFMPEG", NID_MEDIA_TIMELINE, &MediaTimelineNode::staticMetaObject ) );
}

ffmpegPlugin::~ffmpegPlugin( void )
{
}

QString ffmpegPlugin::av_err( const QString &pHeader, int pErrorCode)
{
#if defined( FFMPEG_SUPPORTED )
	char	errbuf[ AV_ERROR_MAX_STRING_SIZE ];

	av_make_error_string( errbuf, AV_ERROR_MAX_STRING_SIZE, pErrorCode );

	return( QString( "%1: %2" ).arg( pHeader ).arg( QString::fromLatin1( errbuf ) ) );
#else
	return( "" );
#endif
}

QString ffmpegPlugin::av_err( int pErrorCode )
{
#if defined( FFMPEG_SUPPORTED )
	char	errbuf[ AV_ERROR_MAX_STRING_SIZE ];

	av_make_error_string( errbuf, AV_ERROR_MAX_STRING_SIZE, pErrorCode );

	return( QString::fromLatin1( errbuf ) );
#else
	return( "" );
#endif
}

PluginInterface::InitResult ffmpegPlugin::initialise( fugio::GlobalInterface *pApp, bool pLastChance )
{
	Q_UNUSED( pLastChance )

	mInstance = this;

	mApp = pApp;

	//-------------------------------------------------------------------------
	// initialise ffmpeg

#if defined( FFMPEG_SUPPORTED )
	av_log_set_flags( AV_LOG_SKIP_REPEATED );

	avcodec_register_all();

	avdevice_register_all();

	av_register_all();

	avformat_network_init();
#endif

	//-------------------------------------------------------------------------

	mApp->registerNodeClasses( mNodeEntries );

	mApp->registerPinClasses( mPinEntries );

	return( INIT_OK );
}

void ffmpegPlugin::deinitialise()
{
	mApp->unregisterNodeClasses( mNodeEntries );

	mApp->unregisterPinClasses( mPinEntries );

#if defined( FFMPEG_SUPPORTED )
	avformat_network_deinit();
#endif
}
