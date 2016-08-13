#ifndef INPUTEVENTSPIN_H
#define INPUTEVENTSPIN_H

#include <QObject>
#include <QMouseEvent>
#include <QTouchEvent>

#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>
#include <fugio/gui/input_events_interface.h>

#include <fugio/pincontrolbase.h>

class InputEventsPin : public fugio::PinControlBase, public fugio::InputEventsInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::InputEventsInterface )

public:
	Q_INVOKABLE explicit InputEventsPin( QSharedPointer<fugio::PinInterface> pPin );

	virtual ~InputEventsPin( void ) {}

	//-------------------------------------------------------------------------
	// fugio::PinControlInterface

	virtual QString toString( void ) const Q_DECL_OVERRIDE
	{
		return( QStringLiteral( "" ) );
	}

	virtual QString description( void ) const Q_DECL_OVERRIDE
	{
		return( QStringLiteral( "Input Events" ) );
	}

	//-------------------------------------------------------------------------
	// InputEventsInterface interface

public:
	virtual void inputProcessEvent(QInputEvent *pEvent) Q_DECL_OVERRIDE;
	virtual void inputFrameInitialise() Q_DECL_OVERRIDE;

	virtual const QList<QMouseEvent> &mouseEvents() const Q_DECL_OVERRIDE
	{
		return( mMouseEventsCurr );
	}

	virtual const QList<QTouchEvent> &touchEvents() const Q_DECL_OVERRIDE
	{
		return( mTouchEventsCurr );
	}

protected:
	QList<QMouseEvent>		mMouseEventsCurr;
	QList<QMouseEvent>		mMouseEventsNext;
	QList<QTouchEvent>		mTouchEventsCurr;
	QList<QTouchEvent>		mTouchEventsNext;
};

#endif // INPUTEVENTSPIN_H
