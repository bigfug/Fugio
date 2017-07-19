#ifndef KEYBOARD_INTERFACE_H
#define KEYBOARD_INTERFACE_H

#include <QUuid>
#include <QList>
#include <QMetaObject>

#include <fugio/global.h>

#define PID_KEYBOARD		(QUuid("{f3aab2ab-78bd-4e31-b6f1-9a017fe7289b}"))

FUGIO_NAMESPACE_BEGIN

typedef struct KeyboardEvent
{
	typedef enum Type
	{
		PRESS, RELEASE
	} Type;

	enum Modifier
	{
		NONE		= 0,
		SHIFT		= 1 << 0,
		CTRL		= 1 << 1,
		ALT			= 1 << 2,
		META		= 1 << 3,
		KEYPAD		= 1 << 4,
		GROUPSWITCH = 1 << 5
	};

	Q_DECLARE_FLAGS( Modifiers, Modifier )

	Type			mType;
	Modifiers		mModifiers;
	int				mCode;
	QString			mText;
} KeyboardEvent;

Q_DECLARE_OPERATORS_FOR_FLAGS( KeyboardEvent::Modifiers )

class KeyboardInterface
{
public:
	virtual ~KeyboardInterface( void ) {}

	virtual void keyboardAddEvent( const KeyboardEvent &pEvent ) = 0;

	virtual void keyboardAddEvents( const QList<KeyboardEvent> &pEventList ) = 0;

	virtual QList<KeyboardEvent> keyboardEvents( void ) const = 0;

	virtual void keyboardClearEvents( void ) = 0;
};

FUGIO_NAMESPACE_END

Q_DECLARE_METATYPE( fugio::KeyboardEvent )

Q_DECLARE_INTERFACE( fugio::KeyboardInterface, "com.bigfug.fugio.keyboard/1.0" )

#endif // KEYBOARD_INTERFACE_H
