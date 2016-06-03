#ifndef CMDGROUPADDINPUT_H
#define CMDGROUPADDINPUT_H

#include <QUndoCommand>
#include <QUuid>

#include <fugio/global_interface.h>
#include <fugio/context_interface.h>
#include <fugio/node_interface.h>

#include "../contextview.h"
#include "../nodeitem.h"

class CmdGroupAddInput : public QUndoCommand
{
public:
	explicit CmdGroupAddInput( ContextView *pContextView, const QUuid &pGroupId, QSharedPointer<fugio::PinInterface> pPin )
		: mContextView( pContextView ), mGroupId( pGroupId ), mPin( pPin )
	{
		setText( QObject::tr( "Group Add Input" ) );
	}

	virtual ~CmdGroupAddInput( void )
	{

	}

	virtual void undo( void )
	{
		QSharedPointer<NodeItem>	NI = mContextView->findNodeItem( mGroupId );

		if( NI )
		{
			NI->pinInputRemove( mPin->globalId() );

			NI->layoutPins();
		}
	}

	virtual void redo( void )
	{
		QSharedPointer<NodeItem>	NI = mContextView->findNodeItem( mGroupId );

		if( NI )
		{
			NI->pinInputAdd( mPin );

			NI->layoutPins();
		}
	}

private:
	ContextView								*mContextView;
	QUuid									 mGroupId;
	QSharedPointer<fugio::PinInterface>		 mPin;
};

#endif // CMDGROUPADDINPUT_H
