#include "inputeventspin.h"

InputEventsPin::InputEventsPin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin )
{

}

void InputEventsPin::inputFrameInitialise()
{
	mMouseEventsCurr.swap( mMouseEventsNext );
	mTouchEventsCurr.swap( mTouchEventsNext );

	mMouseEventsNext.clear();
	mTouchEventsNext.clear();
}

void InputEventsPin::inputProcessEvent( QInputEvent *pEvent )
{
	if( QMouseEvent *MouseEvent = dynamic_cast<QMouseEvent *>( pEvent ) )
	{
		mMouseEventsNext.append( *MouseEvent );
	}
	else if( QTouchEvent *TouchEvent = dynamic_cast<QTouchEvent *>( pEvent ) )
	{
		mTouchEventsNext.append( *TouchEvent );
	}
}
