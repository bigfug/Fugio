#include "keyboardpin.h"

QDataStream &operator <<( QDataStream &DS, const fugio::KeyboardEvent &KE )
{
	DS << quint32( KE.mType ) << quint32( KE.mModifiers ) << KE.mCode << KE.mText;

	return( DS );
}

QDataStream &operator >>( QDataStream &DS, fugio::KeyboardEvent &KE )
{
	quint32		Type;
	quint32		Modifiers;

	DS >> Type >> Modifiers >> KE.mCode >> KE.mText;

	KE.mType      = fugio::KeyboardEvent::Type( Type );
	KE.mModifiers = fugio::KeyboardEvent::Modifiers( Modifiers );

	return( DS );
}

KeyboardPin::KeyboardPin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin )
{
}

void KeyboardPin::registerMetaType()
{
	qRegisterMetaTypeStreamOperators<fugio::KeyboardEvent>( "fugio::KeyboardEvent" );
}

void KeyboardPin::keyboardAddEvent( const fugio::KeyboardEvent &pEvent)
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
	pDataStream >> mEvtLst;
}

void KeyboardPin::serialise( QDataStream &pDataStream ) const
{
	pDataStream << mEvtLst;
}
