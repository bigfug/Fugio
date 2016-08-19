#include "imagepreview.h"

//#include <fugio/mouse_interface.h>

#include <QMouseEvent>

#include <QDebug>

ImagePreview::ImagePreview( QWidget * parent, Qt::WindowFlags f )
	: QLabel( parent, f ), mInputEvents( nullptr )
{
	setMouseTracking( true );
}

ImagePreview::~ImagePreview( void )
{
}

bool ImagePreview::event( QEvent *pEvent )
{
	if( QInputEvent *InputEvent = dynamic_cast<QInputEvent *>( pEvent ) )
	{
		if( mInputEvents )
		{
			mInputEvents->inputProcessEvent( InputEvent );
		}
	}

	return( QLabel::event( pEvent ) );
}
