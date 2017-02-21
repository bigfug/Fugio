#include "mediaplayervideopreview.h"
#include "ui_mediaplayervideopreview.h"
#include <fugio/image/image_interface.h>

MediaPlayerVideoPreview::MediaPlayerVideoPreview( MediaTimelineNode *pMediaPlayer )
	: ui(new Ui::MediaPlayerVideoPreview), mMediaPlayer( pMediaPlayer ),
	  mLastImageUpdate( 0 )
{
	ui->setupUi(this);
}

MediaPlayerVideoPreview::~MediaPlayerVideoPreview( void )
{
	delete ui;
}

void MediaPlayerVideoPreview::playheadPlay( qreal pTimeLast, qreal pTimeCurr )
{
	Q_UNUSED( pTimeLast )
	Q_UNUSED( pTimeCurr )

	if( !isVisible() )
	{
		return;
	}

	if( mMediaPlayer->lastImageUpdate() == mLastImageUpdate )
	{
		return;
	}

	const fugio::ImageInterface		*SRC = mMediaPlayer->image();

	if( !SRC || SRC->size().isEmpty() )
	{
		return;
	}

	QImage						 IMG = SRC->image().scaled( ui->mVideoImage->size(), Qt::KeepAspectRatio );

	if( IMG.isNull() )
	{
		return;
	}

	ui->mVideoImage->setPixmap( QPixmap::fromImage( IMG ) );

	mLastImageUpdate = mMediaPlayer->lastImageUpdate();
}
