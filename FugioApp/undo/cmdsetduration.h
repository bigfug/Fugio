#ifndef CMDSETDURATION_H
#define CMDSETDURATION_H

#include <QUndoCommand>

#include <fugio/global_interface.h>

#include <fugio/global.h>

#include "app.h"

class CmdSetDuration : public QUndoCommand
{
public:
	explicit CmdSetDuration( QSharedPointer<fugio::ContextInterface> pContext, qreal pDuration )
		: mContext( pContext ), mDurNew( pDuration ), mDurPrv( mContext->duration() )
	{
		setText( QObject::tr( "Set duration" ) );
	}

	virtual ~CmdSetDuration( void )
	{

	}

	virtual void undo( void )
	{
		mContext->setDuration( mDurPrv );
	}

	virtual void redo( void )
	{
		mContext->setDuration( mDurNew );
	}

private:
	QSharedPointer<fugio::ContextInterface>		 mContext;
	qreal									 mDurNew, mDurPrv;
};

#endif // CMDSETDURATION_H
