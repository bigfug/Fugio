#ifndef CMDGROUPREMINPUT_H
#define CMDGROUPREMINPUT_H

#include <QUndoCommand>
#include <QUuid>

#include <fugio/global_interface.h>
#include <fugio/context_interface.h>
#include <fugio/node_interface.h>

#include "../contextview.h"
#include "../nodeitem.h"

class CmdGroupRemInput : public QUndoCommand
{
public:
	explicit CmdGroupRemInput( ContextView *pContextView, const QUuid &pGroupId, QSharedPointer<fugio::PinInterface> pPin )
		: mContextView( pContextView ), mGroupId( pGroupId ), mPin( pPin )
	{
		setText( QObject::tr( "Group Remove Input" ) );
	}

	virtual ~CmdGroupRemInput( void )
	{

	}

	virtual void undo( void )
	{
		QSharedPointer<NodeItem>	NI = mContextView->findNodeItem( mGroupId );

		if( NI )
		{
			NI->pinInputAdd( mPin );

			NI->layoutPins();

			mContextView->updatePinItem( mPin->globalId() );
		}
	}

	virtual void redo( void )
	{
		QSharedPointer<NodeItem>	NI = mContextView->findNodeItem( mGroupId );

		if( NI )
		{
			NI->pinInputRemove( mPin->globalId() );

			NI->layoutPins();

			mContextView->updatePinItem( mPin->globalId() );
		}
	}

private:
	ContextView								*mContextView;
	QUuid									 mGroupId;
	QSharedPointer<fugio::PinInterface>		 mPin;
};

#endif // CMDGROUPREMINPUT_H
