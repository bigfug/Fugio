#ifndef CMDPINDISCONNECTGLOBAL_H
#define CMDPINDISCONNECTGLOBAL_H

#include <QUndoCommand>

#include "contextview.h"
#include "nodeitem.h"
#include "pinitem.h"

#include <fugio/pin_interface.h>

class CmdPinDisconnectGlobal : public QUndoCommand
{
public:
	explicit CmdPinDisconnectGlobal( ContextView *pContextView, QSharedPointer<fugio::PinInterface> pPin, QUuid pSrcPid )
		: mContextView( pContextView ), mPinDst( pPin ), mSrcPid( pSrcPid )
	{
		setText( QObject::tr( "Disonnect global pin" ) );
	}

	virtual ~CmdPinDisconnectGlobal( void )
	{

	}

	virtual void undo( void )
	{
		mContextView->widget()->saveRecovery();

		mContextView->context()->connectPins( mSrcPid, mPinDst->globalId() );

		if( mPinDst->updatable() )
		{
			mContextView->context()->pinUpdated( mPinDst );
		}

		mContextView->widget()->saveRecovery();
	}

	virtual void redo( void )
	{
		mContextView->widget()->saveRecovery();

		mContextView->context()->disconnectPins( mSrcPid, mPinDst->globalId() );

		if( mPinDst->updatable() )
		{
			mContextView->context()->pinUpdated( mPinDst );
		}

		mContextView->widget()->saveRecovery();
	}

private:
	ContextView								*mContextView;
	QSharedPointer<fugio::PinInterface>		 mPinDst;
	QUuid									 mSrcPid;
};

#endif // CMDPINDISCONNECTGLOBAL_H
