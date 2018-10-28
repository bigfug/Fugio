#ifndef CMDLINKREMOVE_H
#define CMDLINKREMOVE_H

#include <QUndoCommand>

#include <fugio/global_interface.h>

#include "app.h"

class CmdLinkRemove : public QUndoCommand
{
public:
	explicit CmdLinkRemove( fugio::ContextInterface *pContext, QSharedPointer<fugio::PinInterface> pPinSrc, QSharedPointer<fugio::PinInterface> pPinDst )
		: mContext( pContext ), mPinSrc( pPinSrc ), mPinDst( pPinDst )
	{
		setText( QObject::tr( "Remove Link" ) );
	}

	virtual ~CmdLinkRemove( void )
	{

	}

	virtual void undo( void )
	{
		mContext->connectPins( mPinSrc->uuid(), mPinDst->uuid() );

		if( mPinDst->updatable() )
		{
			mContext->pinUpdated( mPinDst );
		}
	}

	virtual void redo( void )
	{
		mContext->disconnectPins( mPinSrc->uuid(), mPinDst->uuid() );

		if( mPinDst->updatable() )
		{
			mContext->pinUpdated( mPinDst );
		}
	}

private:
	fugio::ContextInterface					*mContext;
	QSharedPointer<fugio::PinInterface>		 mPinSrc;
	QSharedPointer<fugio::PinInterface>		 mPinDst;
};

#endif // CMDLINKREMOVE_H
