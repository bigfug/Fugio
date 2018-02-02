#include "mediaplayervideopreview.h"
#include "ui_mediaplayervideopreview.h"
#include <fugio/image/image.h>

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

	const fugio::VariantInterface		*SRC = mMediaPlayer->image();

	fugio::Image	I = SRC->variant().value<fugio::Image>();

	if( !SRC || I.isEmpty() )
	{
		return;
	}

	QImage						 IMG = I.image().scaled( ui->mVideoImage->size(), Qt::KeepAspectRatio );

	if( IMG.isNull() )
	{
		return;
	}

	ui->mVideoImage->setPixmap( QPixmap::fromImage( IMG ) );

	mLastImageUpdate = mMediaPlayer->lastImageUpdate();
}
