#ifndef MEDIAPLAYERVIDEOPREVIEW_H
#define MEDIAPLAYERVIDEOPREVIEW_H

#include <QWidget>

#include "mediatimelinenode.h"

#include <fugio/interface_keyframes_controls.h>

namespace Ui {
	class MediaPlayerVideoPreview;
}

class MediaPlayerVideoPreview : public QWidget, public InterfaceKeyFramesControls
{
	Q_OBJECT
	Q_INTERFACES( InterfaceKeyFramesControls )

public:
	explicit MediaPlayerVideoPreview( MediaTimelineNode *pMediaPlayer );

	virtual ~MediaPlayerVideoPreview( void );

	virtual QWidget *widget( void )
	{
		return( this );
	}

	virtual void playheadPlay( qreal pTimeLast, qreal pTimeCurr );

private:
	Ui::MediaPlayerVideoPreview *ui;

	MediaTimelineNode				*mMediaPlayer;
	qreal						 mLastImageUpdate;
};

#endif // MEDIAPLAYERVIDEOPREVIEW_H
