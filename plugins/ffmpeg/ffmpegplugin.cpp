#include "ffmpegplugin.h"

#include <QtPlugin>

#include <QDebug>

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

QList<QUuid>				 NodeControlBase::PID_UUID;

ffmpegPlugin				*ffmpegPlugin::mInstance = 0;

ffmpegPlugin::ffmpegPlugin( void )
	: mApp( 0 )
{
	mNodeEntries.append( ClassEntry( "Image Convert", "FFMPEG", NID_SWR_IMAGE_CONVERT, &ImageConvertNode::staticMetaObject ) );
	mNodeEntries.append( ClassEntry( "Media Player",   "FFMPEG", NID_MEDIA_PLAYER, &MediaNode::staticMetaObject ) );
}

ffmpegPlugin::~ffmpegPlugin( void )
{
}

PluginInterface::InitResult ffmpegPlugin::initialise( fugio::GlobalInterface *pApp, bool pLastChance )
{
	Q_UNUSED( pLastChance )

	mInstance = this;

	mApp = pApp;

	//-------------------------------------------------------------------------
	// initialise ffmpeg

	qInfo() << "Initialising ffmpeg";

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
