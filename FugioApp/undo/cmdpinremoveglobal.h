#ifndef CMDPINREMOVEGLOBAL_H
#define CMDPINREMOVEGLOBAL_H

#include <QUndoCommand>

#include "contextview.h"

#include <fugio/pin_interface.h>

class CmdPinRemoveGlobal : public QUndoCommand
{
public:
	explicit CmdPinRemoveGlobal( ContextView *pContextView, QSharedPointer<fugio::PinInterface> pPin )
		: mContextView( pContextView ), mPin( pPin )
	{
		setText( QObject::tr( "Remove global pin" ) );

		QList< QSharedPointer<fugio::PinInterface> >	PinLst = mPin->connectedPins();

		for( auto P : PinLst )
		{
			mDstPinIds.append( P->globalId() );
		}
	}

	virtual ~CmdPinRemoveGlobal( void )
	{

	}

	virtual void undo( void )
	{
		mPin->setSetting( "global", true );

		QSharedPointer<NodeItem>	NI = mContextView->findNodeItem( mPin->node()->uuid() );

		if( NI )
		{
			PinItem		*PI = NI->findPinOutput( mPin->globalId() );

			if( PI )
			{
				PI->update();
			}
		}

		mContextView->addGlobalPin( mPin->globalId() );

		for( QUuid Pid : mDstPinIds )
		{
			mContextView->context()->connectPins( mPin->globalId(), Pid );
		}
	}

	virtual void redo( void )
	{
		for( QUuid Pid : mDstPinIds )
		{
			mContextView->context()->disconnectPins( mPin->globalId(), Pid );
		}

		mPin->clearSetting( "global" );

		QSharedPointer<NodeItem>	NI = mContextView->findNodeItem( mPin->node()->uuid() );

		if( NI )
		{
			PinItem		*PI = NI->findPinOutput( mPin->globalId() );

			if( PI )
			{
				PI->update();
			}
		}

		mContextView->remGlobalPin( mPin->globalId() );
	}

private:
	ContextView								*mContextView;
	QSharedPointer<fugio::PinInterface>		 mPin;
	QList<QUuid>							 mDstPinIds;
};

#endif // CMDPINREMOVEGLOBAL_H
