#ifndef CMDPINCONNECTGLOBAL_H
#define CMDPINCONNECTGLOBAL_H

#include <QUndoCommand>

#include "contextview.h"
#include "nodeitem.h"
#include "pinitem.h"

#include <fugio/pin_interface.h>

class CmdPinConnectGlobal : public QUndoCommand
{
public:
	explicit CmdPinConnectGlobal( ContextView *pContextView, QSharedPointer<fugio::PinInterface> pPin, QUuid pSrcPid )
		: mContextView( pContextView ), mPinDst( pPin ), mSrcPid( pSrcPid )
	{
		setText( QObject::tr( "Connect global pin" ) );
	}

	virtual ~CmdPinConnectGlobal( void )
	{

	}

	virtual void undo( void )
	{
		mContextView->widget()->saveRecovery();

		mContextView->context()->disconnectPins( mSrcPid, mPinDst->globalId() );

		if( mPinDst->updatable() )
		{
			mContextView->context()->pinUpdated( mPinDst );
		}

		mContextView->widget()->saveRecovery();
	}

	virtual void redo( void )
	{
		mContextView->widget()->saveRecovery();

		mContextView->context()->connectPins( mSrcPid, mPinDst->globalId() );

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

#endif // CMDPINCONNECTGLOBAL_H
