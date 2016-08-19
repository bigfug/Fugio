#ifndef IMAGEPREVIEW_H
#define IMAGEPREVIEW_H

#include <QLabel>

#include <fugio/global.h>

#include <fugio/gui/input_events_interface.h>

//#include <fugio/mouse_source_interface.h>

FUGIO_NAMESPACE_BEGIN
class MouseInterface;
FUGIO_NAMESPACE_END

class ImagePreview : public QLabel
{
	Q_OBJECT

public:
	explicit ImagePreview( QWidget * parent = 0, Qt::WindowFlags f = 0 );

	virtual ~ImagePreview( void );

	void setInputEventsInterface( fugio::InputEventsInterface *pInputEvents )
	{
		mInputEvents = pInputEvents;
	}

	// QObject interface
public:
	virtual bool event( QEvent *pEvent ) Q_DECL_OVERRIDE;

private:
	fugio::InputEventsInterface		*mInputEvents;
};

#endif // IMAGEPREVIEW_H
