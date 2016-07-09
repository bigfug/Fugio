#include "imagepreview.h"

//#include <fugio/mouse_interface.h>

#include <QMouseEvent>

#include <QDebug>

ImagePreview::ImagePreview( QWidget * parent, Qt::WindowFlags f )
	: QLabel( parent, f ), mMouseLeft( false )
{
	setMouseTracking( true );
}

ImagePreview::~ImagePreview( void )
{
}

void ImagePreview::mousePressEvent( QMouseEvent *pEvent )
{
	updateMousePosition( pEvent->pos() );

//	if( mMouse )
//	{
//		mMouse->updateMouseButtons( pEvent->buttons() );
//	}

	mMouseLeft = pEvent->buttons() & Qt::LeftButton;
}

void ImagePreview::mouseReleaseEvent( QMouseEvent *pEvent )
{
	updateMousePosition( pEvent->pos() );

//	if( mMouse )
//	{
//		mMouse->updateMouseButtons( pEvent->buttons() );
//	}

	mMouseLeft = pEvent->buttons() & Qt::LeftButton;
}

void ImagePreview::mouseMoveEvent( QMouseEvent *pEvent )
{
	updateMousePosition( pEvent->pos() );

//	if( mMouse )
//	{
//		mMouse->updateMouseButtons( pEvent->buttons() );
//	}

	mMouseLeft = pEvent->buttons() & Qt::LeftButton;
}

void ImagePreview::updateMousePosition( const QPoint &pMP )
{
	mMousePosition.setX( pMP.x() );
	mMousePosition.setY( pMP.y() );

//	if( mMouse )
//	{
//		mMouse->updateMousePosition( pMP );
//	}
}
