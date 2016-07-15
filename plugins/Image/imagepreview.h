#ifndef IMAGEPREVIEW_H
#define IMAGEPREVIEW_H

#include <QLabel>

#include <fugio/global.h>

//#include <fugio/mouse_source_interface.h>

FUGIO_NAMESPACE_BEGIN
class MouseInterface;
FUGIO_NAMESPACE_END

class ImagePreview : public QLabel //, public fugio::MouseSourceInterface
{
	Q_OBJECT
	//Q_INTERFACES( fugio::MouseSourceInterface )

public:
	explicit ImagePreview( QWidget * parent = 0, Qt::WindowFlags f = 0 );

	virtual ~ImagePreview( void );

	QPointF mousePosition( void ) const
	{
		return( mMousePosition );
	}

	bool mouseLeft( void ) const
	{
		return( mMouseLeft );
	}

	//-------------------------------------------------------------------------
	// InterfaceMouseSource

	// QWidget interface
protected:
	virtual void mousePressEvent(QMouseEvent *);
	virtual void mouseReleaseEvent(QMouseEvent *);
	virtual void mouseMoveEvent(QMouseEvent *);

	void updateMousePosition( const QPoint &pMP );

private:
//	fugio::MouseInterface		*mMouse;
	QPoint						 mMousePosition;
	bool						 mMouseLeft;
};

#endif // IMAGEPREVIEW_H
