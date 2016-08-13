#ifndef INPUT_EVENTS_INTERFACE_H
#define INPUT_EVENTS_INTERFACE_H

#include <QUuid>
#include <QList>
#include <QInputEvent>
#include <QMouseEvent>
#include <QTouchEvent>

#include <fugio/global.h>

#define PID_INPUT_EVENTS		(QUuid("{6E22A4D3-D8E9-4E22-A0D4-D93182CA019C}"))

FUGIO_NAMESPACE_BEGIN

class InputEventsInterface
{
public:
	virtual ~InputEventsInterface( void ) {}

	virtual void inputProcessEvent( QInputEvent *pEvent ) = 0;

	virtual void inputFrameInitialise( void ) = 0;

	virtual const QList<QMouseEvent> &mouseEvents( void ) const = 0;

	virtual const QList<QTouchEvent> &touchEvents( void ) const = 0;
};

FUGIO_NAMESPACE_END

Q_DECLARE_INTERFACE( fugio::InputEventsInterface, "com.bigfug.fugio.input-events/1.0" )

#endif // INPUT_EVENTS_INTERFACE_H
