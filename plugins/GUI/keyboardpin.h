#ifndef KEYBOARDPIN_H
#define KEYBOARDPIN_H

#include <fugio/pincontrolbase.h>

#include <fugio/gui/keyboard_interface.h>

#include <fugio/serialise_interface.h>

class KeyboardPin : public fugio::PinControlBase, public fugio::KeyboardInterface, public fugio::SerialiseInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::KeyboardInterface fugio::SerialiseInterface )

public:
	Q_INVOKABLE explicit KeyboardPin( QSharedPointer<fugio::PinInterface> pPin );

	virtual ~KeyboardPin( void ) {}

	static void registerMetaType( void );

	//-------------------------------------------------------------------------
	// fugio::PinControlInterface

	virtual QString toString( void ) const Q_DECL_OVERRIDE
	{
		return( "" );
	}

	virtual QString description( void ) const Q_DECL_OVERRIDE
	{
		return( "Keyboard" );
	}

	//-------------------------------------------------------------------------
	// KeyboardInterface interface

	virtual QList<fugio::KeyboardEvent> keyboardEvents() const Q_DECL_OVERRIDE
	{
		return( mEvtLst );
	}

	virtual void keyboardAddEvent( const fugio::KeyboardEvent &pEvent ) Q_DECL_OVERRIDE;
	virtual void keyboardAddEvents( const QList<fugio::KeyboardEvent> &pEventList ) Q_DECL_OVERRIDE;
	virtual void keyboardClearEvents( void ) Q_DECL_OVERRIDE;

	//-------------------------------------------------------------------------
	// fugio::SerialiseInterface

	virtual void serialise( QDataStream &pDataStream ) const Q_DECL_OVERRIDE;
	virtual void deserialise( QDataStream &pDataStream ) Q_DECL_OVERRIDE;

private:
	QList<fugio::KeyboardEvent>		mEvtLst;

};

#endif // KEYBOARDPIN_H
