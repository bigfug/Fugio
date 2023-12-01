#include "mainwindownode.h"

#include <QApplication>
#include <QKeyEvent>

#include <fugio/context_signals.h>

MainWindowNode::MainWindowNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	mKeyboard = pinOutput<fugio::KeyboardInterface *>( "Keyboard", mPinKeyboard, PID_KEYBOARD );
}

bool MainWindowNode::initialise()
{
	if( !fugio::NodeControlBase::initialise() )
	{
		return( false );
	}

	qApp->installEventFilter( this );

	connect( mNode->context()->qobject(), SIGNAL(frameStart()), this, SLOT(contextFrameStart()) );

	return( true );
}

bool MainWindowNode::deinitialise()
{
	disconnect( mNode->context()->qobject(), SIGNAL(frameStart()), this, SLOT(contextFrameStart()) );

	qApp->removeEventFilter( this );

	return( NodeControlBase::deinitialise() );
}

bool MainWindowNode::eventFilter( QObject *pObject, QEvent *pEvent )
{
	if( pEvent->type() == QEvent::KeyPress )
	{
		QKeyEvent *keyEvent = static_cast<QKeyEvent *>( pEvent );

		if( !keyEvent->isAutoRepeat() )
		{
			fugio::KeyboardEvent		KeyEvt;

			KeyEvt.mType = fugio::KeyboardEvent::PRESS;
			KeyEvt.mText = keyEvent->text();
			KeyEvt.mCode = keyEvent->key();

			KeyEvt.translateModifiers( keyEvent->modifiers() );

			mEvtLst << KeyEvt;
		}
	}
	else if( pEvent->type() == QEvent::KeyRelease )
	{
		QKeyEvent *keyEvent = static_cast<QKeyEvent *>( pEvent );

		if( !keyEvent->isAutoRepeat() )
		{
			fugio::KeyboardEvent		KeyEvt;

			KeyEvt.mType = fugio::KeyboardEvent::RELEASE;
			KeyEvt.mText = keyEvent->text();
			KeyEvt.mCode = keyEvent->key();

			KeyEvt.translateModifiers( keyEvent->modifiers() );

			mEvtLst << KeyEvt;
		}
	}

	// standard event processing
	return( QObject::eventFilter( pObject, pEvent ) );
}

void MainWindowNode::contextFrameStart()
{
	mKeyboard->keyboardClearEvents();

	if( !mEvtLst.isEmpty() )
	{
		mKeyboard->keyboardAddEvents( mEvtLst );

		pinUpdated( mPinKeyboard );

		mEvtLst.clear();
	}
}
