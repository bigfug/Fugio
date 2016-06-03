#ifndef CMDLINKADD_H
#define CMDLINKADD_H

#include <QUndoCommand>

#include <fugio/global_interface.h>
#include <fugio/global.h>

#include "app.h"

#include "contextwidgetprivate.h"

class CmdLinkAdd : public QUndoCommand
{
public:
	explicit CmdLinkAdd( ContextWidgetPrivate *pContextWidget, QSharedPointer<fugio::PinInterface> pPinSrc, QSharedPointer<fugio::PinInterface> pPinDst )
		: mContextWidget( pContextWidget ), mPinSrc( pPinSrc ), mPinDst( pPinDst )
	{
		setText( QObject::tr( "Add Link" ) );

		mContext = mContextWidget->context();

		gApp->incrementStatistic( "link-added" );
	}

	virtual ~CmdLinkAdd( void )
	{

	}

	virtual void undo( void )
	{
		mContext->disconnectPins( mPinSrc->globalId(), mPinDst->globalId() );

		if( mPinDst->updatable() )
		{
			mContext->pinUpdated( mPinDst );
		}
	}

	virtual void redo( void )
	{
		mContextWidget->saveRecovery();

		mContext->connectPins( mPinSrc->globalId(), mPinDst->globalId() );

		if( mPinDst->updatable() )
		{
			mContext->pinUpdated( mPinDst );
		}

		mContextWidget->saveRecovery();;
	}

private:
	ContextWidgetPrivate					*mContextWidget;
	QSharedPointer<fugio::ContextInterface>	 mContext;
	QSharedPointer<fugio::PinInterface>		 mPinSrc;
	QSharedPointer<fugio::PinInterface>		 mPinDst;
};

#endif // CMDLINKADD_H
