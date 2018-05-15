#include "keyboardnode.h"

#include <QMainWindow>
#include <QPushButton>
#include <QApplication>
#include <QKeyEvent>

#include <fugio/context_interface.h>
#include <fugio/gui/keyboard_interface.h>
#include <fugio/global_interface.h>
#include <fugio/core/uuid.h>
#include <fugio/context_signals.h>
#include <fugio/node_signals.h>

#include "keyboarddialog.h"

KeyboardNode::KeyboardNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mListening( false )
{
	mPinKeyboard = pinInput( tr( "Keyboard" ) );
}

KeyboardNode::~KeyboardNode()
{

}

QWidget *KeyboardNode::gui()
{
	QPushButton				*GUI = new QPushButton( "Listen..." );

	if( GUI )
	{
		GUI->setCheckable( true );

		connect( GUI, SIGNAL(clicked(bool)), this, SLOT(onButton(bool)) );

		connect( this, SIGNAL(listening(bool)), GUI, SLOT(setChecked(bool)) );
	}

	return( GUI );
}

static QString keyToString( int K )
{
	switch( K )
	{
		case Qt::Key_Escape:			return( "Shift" );
		case Qt::Key_Tab:				return( "Tab" );
		case Qt::Key_Backtab:			return( "Backtab" );
		case Qt::Key_Backspace:			return( "Backspace" );
		case Qt::Key_Return:			return( "Return" );
		case Qt::Key_Enter:				return( "Enter" );
		case Qt::Key_Insert:			return( "Insert" );
		case Qt::Key_Delete:			return( "Delete" );
		case Qt::Key_Pause:				return( "Pause" );
		case Qt::Key_Print:				return( "Print" );
		case Qt::Key_SysReq:			return( "SysReq" );
		case Qt::Key_Clear:				return( "Clear" );
		case Qt::Key_Home:				return( "Home" );
		case Qt::Key_End:				return( "End" );
		case Qt::Key_Left:				return( "Left" );
		case Qt::Key_Up:				return( "Up" );
		case Qt::Key_Right:				return( "Right" );
		case Qt::Key_Down:				return( "Down" );
		case Qt::Key_PageUp:			return( "PageUp" );
		case Qt::Key_PageDown:			return( "PageDown" );
		case Qt::Key_Shift:				return( "Shift" );
		case Qt::Key_Control:			return( "Control" );
		case Qt::Key_Meta:				return( "Meta" );
		case Qt::Key_Alt:				return( "Alt" );
		case Qt::Key_AltGr:				return( "AltGr" );
		case Qt::Key_CapsLock:			return( "CapsLock" );
		case Qt::Key_NumLock:			return( "NumLock" );
		case Qt::Key_ScrollLock:		return( "ScrollLock" );
		case Qt::Key_Space:				return( "Space" );
		default:						return( ( QString( "%1" ).arg( QChar( K ) ) ).toUpper() );
	}

	return( QString() );
}

void KeyboardNode::onButton( bool pChecked )
{
	mListening = pChecked;
}

QList<QUuid> KeyboardNode::pinAddTypesOutput() const
{
	QList<QUuid>		PinLst;

	PinLst << PID_BOOL;

	return( PinLst );
}

bool KeyboardNode::canAcceptPin(fugio::PinInterface *pPin) const
{
	return( pPin->direction() == PIN_INPUT );
}

void KeyboardNode::inputsUpdated( qint64 pTimeStamp )
{
	Q_UNUSED( pTimeStamp )

	fugio::KeyboardInterface	*Keyboard = input<fugio::KeyboardInterface *>( mPinKeyboard );

	if( !Keyboard )
	{
		return;
	}

	for( fugio::KeyboardEvent KeyEvt : Keyboard->keyboardEvents() )
	{
		const QString	S = KeyEvt.mText;
		const QString	K = S.isEmpty() || KeyEvt.mCode == Qt::Key_Space ? keyToString( KeyEvt.mCode ) : S.toUpper();

		QSharedPointer<fugio::PinInterface>	P = mNode->findOutputPinByName( K );

		if( P && P->hasControl() )
		{
			fugio::VariantInterface		*V = qobject_cast<fugio::VariantInterface *>( P->control()->qobject() );

			if( V )
			{
				V->setVariant( KeyEvt.mType == fugio::KeyboardEvent::PRESS );

				pinUpdated( P );
			}
		}
		else if( mListening )
		{
			P = pinOutput( K, PID_BOOL, QUuid::createUuid() );

			P->setRemovable( true );

			mListening = false;

			emit listening( false );
		}
	}
}
