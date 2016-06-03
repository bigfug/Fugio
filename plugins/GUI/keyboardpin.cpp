#include "keyboardpin.h"

KeyboardPin::KeyboardPin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin )
{
}

void KeyboardPin::keyboardAddEvent(const fugio::KeyboardEvent &pEvent)
{
	mEvtLst << pEvent;
}

void KeyboardPin::keyboardAddEvents(const QList<fugio::KeyboardEvent> &pEventList)
{
	mEvtLst << pEventList;
}

void KeyboardPin::keyboardClearEvents()
{
	mEvtLst.clear();
}

void KeyboardPin::deserialise( QDataStream &pDataStream )
{
	Q_UNUSED( pDataStream )
}

void KeyboardPin::serialise( QDataStream &pDataStream )
{
	Q_UNUSED( pDataStream )
}
