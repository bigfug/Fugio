#ifndef CMDPINMAKEGLOBAL_H
#define CMDPINMAKEGLOBAL_H

#include <QUndoCommand>

#include "contextview.h"
#include "nodeitem.h"
#include "pinitem.h"

#include <fugio/pin_interface.h>

class CmdPinMakeGlobal : public QUndoCommand
{
public:
	explicit CmdPinMakeGlobal( ContextView *pContextView, QSharedPointer<fugio::PinInterface> pPin )
		: mContextView( pContextView ), mPin( pPin )
	{
		setText( QObject::tr( "Make pin global" ) );
	}

	virtual ~CmdPinMakeGlobal( void )
	{

	}

	virtual void undo( void )
	{
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

	virtual void redo( void )
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
	}

private:
	ContextView								*mContextView;
	QSharedPointer<fugio::PinInterface>		 mPin;
};

#endif // CMDPINMAKEGLOBAL_H
